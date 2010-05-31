/* ------------------------------------------------------------------------- */
/*
 *  text_helper.h
 *
 *  Copyright (c) 2009, Four and More. All rights reserved.
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
 *  Last-modified: Sat 18 Apr 2009 01:25:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PDF_TEXT_HELPER_H
#define FAML_PDF_TEXT_HELPER_H

#include <string>
#include <stdexcept>
#include "font_property.h"
#include "text_helper_mbcs.h"
#include "text_helper_utf16.h"

namespace faml {
	namespace pdf {
		/* ------------------------------------------------------------- */
		//  is_ascii
		/* ------------------------------------------------------------- */
		template <class InIter>
		bool is_ascii(InIter first, InIter last, int cset) {
			if (first == last) return false;
			
			switch (cset) {
			case charset::win:
			case charset::mac:
			case charset::euc:
			{
				unsigned char c = *first;
				if (!((c >= 0x81 && c <= 0x9f) || (c >= 0xe0 && c <= 0xfc))) return true;
				break;
			}
			case charset::utf16:
			{
				unsigned char c1 = *first;
				++first;
				if (first == last) break;
				unsigned char c2 = *first;
				if (c1 == 0x00 && c2 <= 0x7f) return true;
				break;
			}
			default:
				break;
			}
			return false;
		}
		
		/* ----------------------------------------------------------------- */
		/*
		 *  chomp
		 *
		 *  The chomp function erases the characters of LF/CR code from
		 *  the end of given string. The given string may store
		 *  multi-byte characters to the uni-byte string, so the function
		 *  also erases the 0x00 character.
		 */
		/* ----------------------------------------------------------------- */
		std::string& chomp(std::string& src, int cset) {
			switch (cset) {
			case charset::win:
			case charset::mac:
			case charset::euc:
				return mbcs::chomp(src);
				break;
			case charset::utf16:
				return utf16::chomp(src);
				break;
			default:
				break;
			}
			
			return src;
		}
		
		/* ----------------------------------------------------------------- */
		/*
		 *  strip
		 *
		 *  The parameter of the strip function should be UTF-16 (single-
		 *  byte) string. The function strips the higer byte of the
		 *  character that code is 0x00 0xXX.
		 */
		/* ----------------------------------------------------------------- */
		std::string strip(const std::string& src, int cset) {
			if (cset != charset::utf16) return src;
			std::string dest;
			std::string::const_iterator pos = src.begin();
			while (pos != src.end()) {
				char c = *pos;
				++pos;
				if (pos == src.end()) break;
				if (!(c == 0x00 && static_cast<unsigned char>(*pos) <= 0x7f)) dest += c;
				dest += *pos;
				++pos;
			}
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  text_helper
		/* ----------------------------------------------------------------- */
		class text_helper {
		public:
			typedef size_t size_type;
			typedef char char_type;
			typedef unsigned char byte_type;
			typedef std::basic_string<char> string_type;
			typedef basic_font_property<char> font_property;
			
			explicit text_helper(const font_property& prop, int cset) :
				prop_(prop), cset_(cset) {}
			
			/* ------------------------------------------------------------- */
			//  count
			/* ------------------------------------------------------------- */
			template <class InIter>
			size_type count(InIter first, InIter last) {
				switch (cset_) {
				case charset::win:
				case charset::mac:
				case charset::euc:
					return mbcs::count(first, last);
				case charset::utf16:
					return utf16::count(first, last);
				default:
					break;
				}
				return 0;
			}
			
			/* ------------------------------------------------------------- */
			//  width
			/* ------------------------------------------------------------- */
			template <class InIter>
			size_type width(InIter first, InIter last) {
				switch (cset_) {
				case charset::win:
				case charset::mac:
				case charset::euc:
					return mbcs::width(first, last, prop_);
				case charset::utf16:
					return utf16::width(first, last, prop_);
				default:
					break;
				}
				return 0;
			}
			
			/* ------------------------------------------------------------- */
			//  getline
			/* ------------------------------------------------------------- */
			template <class InIter, class OutIter>
			size_type getline(InIter& first, InIter last, OutIter out, size_type limit, bool wrap = true) {
				//if (first != last && limit < 1000) {
				//	throw std::runtime_error("assigned area is too small");
				//}
				
				switch (cset_) {
				case charset::win:
				case charset::mac:
				case charset::euc:
					return mbcs::getline(first, last, out, limit, wrap, prop_);
				case charset::utf16:
					return utf16::getline(first, last, out, limit, wrap, prop_);
				default:
					break;
				}
				return 0;
			}
			
			/* ------------------------------------------------------------- */
			//  substr
			/* ------------------------------------------------------------- */
			template <class InIter>
			string_type substr(InIter& first, InIter last, bool ascii) {
				string_type dest;
				while (first != last) {
					byte_type c = static_cast<byte_type>(*first);
					if ((cset_ & (charset::win | charset::mac | charset::euc))) {
						if ((c >= 0x81 && c <= 0x9f) || (c >= 0xe0 && c <= 0xfc)) {
							if (ascii) break;
							++first;
							if (first == last) break;
							dest += static_cast<char_type>(c);
							dest += static_cast<char_type>(*first);
							++first;
						}
						else {
							if (!ascii) break;
							dest += static_cast<char_type>(c);
							++first;
						}
					}
					else if (cset_ == charset::utf16) {
						InIter pos = first;
						++pos;
						byte_type c2 = (pos != last) ? static_cast<byte_type>(*pos) : 0x00;
						if (((!ascii) && c == 0x00 && c2 <= 0x7f) ||
							(ascii && !(c == 0x00 && c2 <= 0x7f))) break;
						++first;
						if (first == last) break;
						dest += static_cast<char_type>(c);
						dest += static_cast<char_type>(*first);
						++first;
					}
					else break;
				}
				
				return dest;
			}
			
		private:
			const font_property& prop_;
			int cset_;
		};
	}
}

#endif // FAML_PDF_TEXT_HELPER_H
