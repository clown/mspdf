/* ------------------------------------------------------------------------- */
/*
 *  true_type.h
 *
 *  Copyright (c) 2009, clown. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    - Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    - Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    - No names of its contributors may be used to endorse or promote
 *      products derived from this software without specific prior written
 *      permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  Last-modified: Wed 15 Apr 2009 20:18:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLIFF_TRUE_TYPE_H
#define CLIFF_TRUE_TYPE_H

#include <iostream>
#include <istream>
#include <fstream>
#include <iterator>
#include <string>
#include <map>
#include <babel/babel.h>
#include <clx/container_wrapper.h>
#include <clx/code_convert.h>
#include <clx/utility.h>
#include <clx/hexdump.h>

namespace cliff {
	class true_type {
	public:
		typedef std::size_t size_type;
		typedef char char_type;
		typedef std::string string_type;
		typedef std::istream istream_type;
		
		true_type() :
			header_(), fullnames_() {
			babel::init_babel();
		}
		
		bool read(istream_type& in) {
			if (!this->read_header(in)) return false;
			return this->read_name(in);
		}
		
		std::map<int, string_type>& fullnames() { return fullnames_; }
		
	private:
		struct header_type {
			unsigned short count;
			std::pair<size_type, size_type> name;
		};
		
		header_type header_;
		std::map<int, string_type> fullnames_;
		
		bool read_header(istream_type& in) {
			size_type x = 0;
			
			clx::get(in, x, clx::endian::big);
			if (x == clx::combine('t', 't', 'c', 'f')) return false;
			clx::get(in, header_.count, clx::endian::big);
			
			unsigned short sr, es, rs;
			clx::get(in, sr, clx::endian::big);
			clx::get(in, es, clx::endian::big);
			clx::get(in, rs, clx::endian::big);
			
			if (!this->check_version(x)) return false;
			
			for (size_type i = 0; i < header_.count; ++i) {
				size_type tag = 0, checksum = 0, offset, length = 0;
				clx::get(in, tag, clx::endian::big);
				clx::get(in, checksum, clx::endian::big);
				clx::get(in, offset, clx::endian::big);
				clx::get(in, length, clx::endian::big);
				
				if (tag == clx::combine('n', 'a', 'm', 'e')) {
					header_.name = std::make_pair(offset, length);
				}
			}
			
			return true;
		}
		
		bool read_name(istream_type& in) {
			in.seekg(header_.name.first);
			if (in.fail()) return false;
			
			unsigned short format, count, taboff;
			clx::get(in, format, clx::endian::big);
			clx::get(in, count, clx::endian::big);
			clx::get(in, taboff, clx::endian::big);
			
			for (size_type i = 0; i < count; ++i) {
				unsigned short platform, encoding, language, id;
				clx::get(in, platform, clx::endian::big);
				clx::get(in, encoding, clx::endian::big);
				clx::get(in, language, clx::endian::big);
				clx::get(in, id, clx::endian::big);
				
				unsigned short offset, length;
				clx::get(in, length, clx::endian::big);
				clx::get(in, offset, clx::endian::big);
				
				size_type cur = in.tellg();
				if (id == 4) {
					in.seekg(header_.name.first + taboff + offset);
					if (in.fail()) return false;
					char tmp[4096];
					in.read(tmp, length);
					string_type s(tmp, length);
					if (platform == 3) {
						string_type tmp2 = babel::unicode_to_utf8(clx::widen(s));
						s = tmp2;
					}
					fullnames_[language] = s;
				}
				in.seekg(cur);
			}
			
			return true;
		}
		
		bool check_version(size_type x) {
			if (x == clx::combine('S', 'p', 'l', 'i') ||
				x == clx::combine('%', '!', 'P', 'S') ||
				x == clx::combine('t', 'y', 'p', '1')) {
				return false;
			}
			else if (x >= 0x80000000 && header_.count == 0) return false;
			else if ((x >> 24) == 1 && ((x >> 16) & 0xff) == 0 && ((x >> 8) & 0xff) == 4) return false; // CFF?
			
			return true; // "OTTO", "true", or others.
		}
	};
	
	class true_type_collection : public clx::vector_wrapper<true_type> {
	private:
		typedef clx::vector_wrapper<true_type> super;
	public:
		typedef std::size_t size_type;
		typedef char char_type;
		typedef std::string string_type;
		typedef std::istream istream_type;
		
		true_type_collection() : super() {}
		
		bool read(std::istream& in) {
			size_type x = 0;
			clx::get(in, x, clx::endian::big);
			if (x != clx::combine('t', 't', 'c', 'f')) return false;
			
			size_type count = 0;
			std::vector<size_type> offset;
			clx::get(in, x, clx::endian::big); // reserve?
			clx::get(in, count, clx::endian::big);
			for (size_type i = 0; i < count; ++i) {
				clx::get(in, x, clx::endian::big);
				offset.push_back(x);
			}
			
			for (size_type i = 0; i < offset.size(); ++i) {
				true_type elem;
				in.seekg(offset.at(i));
				if (in.fail()) return false;
				elem.read(in);
				member.push_back(elem);
			}
			
			return true;
		}
	};
}

#endif // CLIFF_TRUE_TYPE_H
