/* ------------------------------------------------------------------------- */
/*
 *  bitmap.h
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
 *  Last-modified: Tue 03 Feb 2009 13:48:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PDF_BITMAP_H
#define FAML_PDF_BITMAP_H

#include <cmath>
#include <string>
#include <iterator>
#include <istream>
#include <sstream>
#include "image.h"
#include "clx/literal.h"
#include "clx/format.h"
#include "clx/lexical_cast.h"
#include "clx/zstream.h"

namespace faml {
	namespace pdf {
		/* ----------------------------------------------------------------- */
		//  bitmap_resource
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class bitmap_resource {
		public:
			typedef size_t size_type;
			typedef unsigned char byte_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef std::basic_istream<CharT, Traits> istream_type;
			typedef clx::basic_zstream<Z_DEFAULT_COMPRESSION, CharT, Traits> zstream;
			
			bitmap_resource(istream_type& in) :
				in_(in), index_(0), label_(),
				size_(0), width_(0), height_(0),
				offset_(0), bpp_(0), colors_() {}
			
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
			long width() const { return width_; }
			long height() const { return height_; }
			
		private:
			typedef clx::basic_format<CharT, Traits> fmt;
			typedef std::vector<byte_type> byte_array;
			
			istream_type& in_;
			size_type index_;
			string_type label_;
			size_type size_;
			long width_;
			long height_;
			
			// internal information
			size_type offset_;
			uint16_t bpp_;
			std::vector<size_type> colors_;
			
			/* ------------------------------------------------------------- */
			//  xobject
			/* ------------------------------------------------------------- */
			template <class OutStream, class PDFManager>
			bool xobject(OutStream& out, PDFManager& pm) {
				index_ = pm.newindex();
				label_ = LITERAL("Im") + clx::lexical_cast<string_type>(index_);
				
				out << fmt(LITERAL("%d 0 obj")) % index_ << std::endl;
				out << LITERAL("<<") << std::endl;
				out << LITERAL("/Type /XObject") << std::endl;
				out << LITERAL("/Subtype /Image") << std::endl;
				out << LITERAL("/Name /") << label_ << std::endl;
				out << LITERAL("/Width ") << width_ << std::endl;
				out << LITERAL("/Height ") << height_ << std::endl;
				out << LITERAL("/BitsPerComponent 8") << std::endl;
				out << LITERAL("/Filter [ /FlateDecode ]") << std::endl;
				if (colors_.empty()) out << LITERAL("/ColorSpace /DeviceRGB") << std::endl;
				else {
					out << fmt(LITERAL("/ColorSpace [ /Indexed /DeviceRGB %d < ")) % (colors_.size() - 1);
					for (size_type i = 0; i < colors_.size(); ++i) {
						out << fmt(LITERAL("%06X ")) % colors_.at(i);
					}
					out << LITERAL("> ]") << std::endl;
				}
				out << fmt(LITERAL("/Length %d 0 R")) % (pm.index() + 1) << std::endl;
				out << LITERAL(">>") << std::endl;
				out << LITERAL("stream") << std::endl;
				size_type pos = static_cast<size_type>(out.tellp());
				bool status = this->xmakestream(out, pm, in_);
				out << std::endl;
				size_type size = static_cast<size_type>(out.tellp()) - pos;
				out << std::endl;
				out << LITERAL("endstream") << std::endl;
				out << LITERAL("endobj") << std::endl;
				out << std::endl;
				
				out << fmt(LITERAL("%d 0 obj")) % pm.newindex() << std::endl;
				out << size << std::endl;
				out << LITERAL("endobj") << std::endl;
				out << std::endl;
				
				return status;
			}
			
			/* ------------------------------------------------------------- */
			//  xproperty
			/* ------------------------------------------------------------- */
			bool xproperty(istream_type& in) {
				unsigned short type = 0;
				if (!this->xget(in, type)) return false;
				if (type != 0x4d42) return false;
				in.seekg(8, std::ios_base::cur);
				if (in.fail()) return false;
				if (!this->xget(in, offset_)) return false;
				
				unsigned long inf = 0;
				if (!this->xget(in, inf)) return false;
				size_type bytes;
				switch (inf) {
				case 12: // OS/2 bitmap
					bytes = 2;
					break;
				case 40: // Windows bitmap
					bytes = 4;
					break;
				default: // unknown
					return false;
					break;
				}
				
				in.read(reinterpret_cast<char_type*>(&width_), bytes);
				if (in.gcount() < static_cast<int>(bytes)) return false;
				in.read(reinterpret_cast<char_type*>(&height_), bytes);
				if (in.gcount() < static_cast<int>(bytes)) return false;
				in.seekg(2, std::ios_base::cur);
				if (!this->xget(in, bpp_)) return false;
				int ncolors = (bpp_ < 16) ? (1 << bpp_) : 0;
				size_ = width_ * height_ * bpp_ / 8;
				
				if (inf == 40) in_.seekg(24, std::ios_base::cur);
				bytes = (inf == 14) ? 3 : 4;
				for (int i = 0; i < ncolors; ++i) {
					uint32_t rgb = 0;
					in.read(reinterpret_cast<char_type*>(&rgb), bytes);
					if (in.gcount() < static_cast<int>(bytes)) return false;
					colors_.push_back(rgb);
				}
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xmakestream
			/* ------------------------------------------------------------- */
			template <class OutStream, class PDFManager>
			bool xmakestream(OutStream& out, PDFManager& pm, istream_type& in) {
				zstream z(out);
				size_type height = std::abs(height_);
				size_type line = width_ * bpp_ / 8;
				size_type size = (line % 4 == 0) ? line : (line / 4 + 1) * 4;
				
				byte_array data(size);
				for (size_type i = 0; i < height; ++i) {
					if (height_ > 0) {
						if (!this->xseek(in, offset_, height_, size, i)) return false;
					}
					in.read(reinterpret_cast<char_type*>(&data[0]), data.size());
					if (in.gcount() < static_cast<int>(size)) return false;
					
					switch (bpp_) {
					case 1:
					case 4:
					case 8:
						z.write(reinterpret_cast<char_type*>(&data[0]), line);
						break;
					case 24:
					case 32:
					{
						size_type bytes = bpp_ / 8;
						for (size_type j = 0; j < line; j += bytes) {
							for (size_type k = bytes; k > 0; --k) z << data.at(j + k - 1);
						}
						break;
					}
					default:
						break;
					}
				}
				z.finish();
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xseek
			/* ------------------------------------------------------------- */
			bool xseek(istream_type& in, size_type off, long height, size_type bytes, size_type index) {
				size_type pos = off + (height - (index + 1)) * bytes;
				in.seekg(pos);
				if (in.fail()) return false;
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xget
			/* ------------------------------------------------------------- */
			template <class Type>
			bool xget(istream_type& in, Type& dest) {
				in.read(reinterpret_cast<char_type*>(&dest), sizeof(Type));
				if (in.gcount() < static_cast<int>(sizeof(Type))) return false;
				return true;
			}
		};
		
		typedef basic_image<bitmap_resource<char>, char> bitmap;
	}
}

#endif // FAML_PDF_BITMAP_H
