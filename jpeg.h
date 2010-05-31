/* ------------------------------------------------------------------------- */
/*
 *  jpeg.h
 *
 *  Copyright (c) 2009, Four and More, Inc. All rights reserved.
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
 *  Last-modified: Thu 23 Apr 2009 14:49:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PDF_JPEG_H
#define FAML_PDF_JPEG_H

#include <string>
#include <iterator>
#include <istream>
#include <sstream>
#include "image.h"
#include "utility.h"
#include "clx/literal.h"
#include "clx/format.h"
#include "clx/lexical_cast.h"

namespace faml {
	namespace pdf {
		/* ----------------------------------------------------------------- */
		//  jpeg_resource
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class jpeg_resource {
		public:
			typedef size_t size_type;
			typedef unsigned char byte_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef std::basic_istream<CharT, Traits> istream_type;
			
			jpeg_resource(const jpeg_resource& cp) :
				in_(cp.in_), index_(cp.index_), label_(cp.label()),
				size_(cp.size_), width_(cp.width_), height_(cp.height_), rgb_(cp.rgb_) {}
			
			jpeg_resource(istream_type& in) :
				in_(in), index_(0), label_(string_type()),
				size_(0), width_(0), height_(0), rgb_(true) {
				label_ = makelabel(LITERAL("Im"));
			}
			
			template <class OutStream, class PDFManager>
			bool operator()(OutStream& out, PDFManager& pm) {
				if (in_.bad() || !this->xproperty(in_)) return false;
				return this->xobject(out, pm);
			}
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			const string_type& label() const { return label_; }
			size_type index() const { return index_; }
			size_type size() const { return size_; }
			size_type width() const { return width_; }
			size_type height() const { return height_; }
			
		private:
			typedef clx::basic_format<CharT, Traits> fmt;
			
			istream_type& in_;
			size_type index_;
			string_type label_;
			size_type size_;
			size_type width_;
			size_type height_;
			bool rgb_;
			
			/* ------------------------------------------------------------- */
			//  xobject
			/* ------------------------------------------------------------- */
			template <class OutStream, class PDFManager>
			bool xobject(OutStream& out, PDFManager& pm) {
				index_ = pm.newindex();
				
				out << fmt(LITERAL("%d 0 obj")) % index_ << std::endl;
				out << LITERAL("<<") << std::endl;
				out << LITERAL("/Type /XObject") << std::endl;
				out << LITERAL("/Subtype /Image") << std::endl;
				out << LITERAL("/Name /") << label_ << std::endl;
				out << LITERAL("/Width ") << width_ << std::endl;
				out << LITERAL("/Height ") << height_ << std::endl;
				out << LITERAL("/BitsPerComponent 8") << std::endl;
				out << LITERAL("/Filter [ /DCTDecode ]") << std::endl;
				if (rgb_) out << LITERAL("/ColorSpace /DeviceRGB") << std::endl;
				else {
					out << LITERAL("/ColorSpace /DeviceCMYK") << std::endl;
					out << LITERAL("/Decode[1 0 1 0 1 0 1 0]") << std::endl;
				}
				out << LITERAL("/Length ") << size_ << std::endl;
				out << LITERAL(">>") << std::endl;
				out << LITERAL("stream") << std::endl;
				in_.seekg(0);
				out << in_.rdbuf();
				out << std::endl;
				out << LITERAL("endstream") << std::endl;
				out << LITERAL("endobj") << std::endl;
				out << std::endl;
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xproperty
			/* ------------------------------------------------------------- */
			bool xproperty(istream_type& in) {
				uint16_t xu16;
				if (!this->xget(in, xu16)) return false;
				if (xu16 != 0xffd8) return false;
				
				while (1) {
					if (!this->xget(in, xu16)) return false;
					if (xu16 == 0xffc0 || xu16 == 0xffc2) {
						if (!this->xheader(in)) return false;
						break;
					}
					else if (xu16 == 0xffff || xu16 == 0xffd9) return false;
					
					// skip segment
					if (!this->xget(in, xu16)) return false;
					in_.seekg(xu16 - 2, std::ios_base::cur);
					if (in_.fail()) return false;
				}
				
				size_type cur = in_.tellg();
				in_.seekg(0, std::ios_base::end);
				size_ = in_.tellg();
				in_.seekg(cur);
				return in.good();
			}
			
			/* ------------------------------------------------------------- */
			//  xheader
			/* ------------------------------------------------------------- */
			bool xheader(istream_type& in) {
				in_.seekg(3, std::ios_base::cur);
				if (in_.fail()) return false;
				
				uint16_t size;
				if (!this->xget(in, size)) return false;
				height_ = size;
				if (!this->xget(in, size)) return false;
				width_ = size;
				
				byte_type sampling = 0;
				if (!this->xget(in, sampling)) return false;
				switch (sampling) {
				case 3:
					rgb_ = true; // RGB
					break;
				case 4:
					rgb_ = false; // CMYK
					break;
				default:
					return false;
				}
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xget
			/* ------------------------------------------------------------- */
			template <class Type>
			bool xget(istream_type& in, Type& dest) {
				dest = 0;
				for (size_type i = 0; i < sizeof(Type); ++i) {
					byte_type c;
					in.read(reinterpret_cast<char_type*>(&c), 1);
					if (in.fail()) return false;
					dest <<= 8;
					dest |= c;
				}
				return true;
			}
		};
		
		typedef basic_image<jpeg_resource<char>, char> jpeg;
	}
}

#endif // FAML_PDF_JPEG_H
