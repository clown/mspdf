/* ------------------------------------------------------------------------- */
/*
 *  png.h
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
 *  Last-modified: Wed 22 Apr 2009 20:46:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PDF_PNG_H
#define FAML_PDF_PNG_H

#include <cmath>
#include <cstring>
#include <string>
#include <vector>
#include <istream>
#include <sstream>
#include "image.h"
#include "utility.h"
#include <clx/mpl/bitmask.h>
#include "clx/literal.h"
#include "clx/format.h"
#include "clx/lexical_cast.h"
#include "clx/vstream.h"
#include "clx/zstream.h"

namespace faml {
	namespace pdf {
		namespace detail {
			static const unsigned char signature[8] = { 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a };
			static const unsigned char ihdr[4] = { 0x49, 0x48, 0x44, 0x52 };
			static const unsigned char plte[4] = { 0x50, 0x4c, 0x54, 0x45 };
			static const unsigned char idat[4] = { 0x49, 0x44, 0x41, 0x54 };
			static const unsigned char iend[4] = { 0x49, 0x45, 0x4e, 0x44 };
			static const unsigned char trns[4] = { 0x74, 0x52, 0x4e, 0x53 };
		}
		
		/* ----------------------------------------------------------------- */
		//  png_resource
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class png_resource {
		public:
			typedef size_t size_type;
			typedef unsigned char byte_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef std::basic_istream<CharT, Traits> istream_type;
			typedef clx::basic_zstream<Z_DEFAULT_COMPRESSION, CharT, Traits> zstream;
			typedef clx::basic_unzstream<CharT, Traits> unzstream;
			
			png_resource(const png_resource& cp) :
				in_(cp.in_), index_(cp.index_), label_(cp.label()),
				size_(cp.size_), width_(cp.width_), height_(cp.height_), colors_(cp.colors_) {}
			
			png_resource(istream_type& in) :
				in_(in), index_(0), label_(),
				size_(0), width_(0), height_(0), colors_() {
				label_ = makelabel(LITERAL("Im"));
			}
			
			template <class OutStream, class PDFManager>
			bool operator()(OutStream& out, PDFManager& pm) {
				if (in_.bad()) throw std::runtime_error("cannot find input file");
				this->xproperty(in_);
				this->xobject(out, pm);
				return true;
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
			typedef std::vector<byte_type> byte_array;
			
			istream_type& in_;
			size_type index_;
			string_type label_;
			size_type size_;
			size_type width_;
			size_type height_;
			std::vector<size_type> colors_;
			std::vector<byte_type> alphas_;
			
			byte_type type_;
			byte_type depth_;
			byte_type compress_;
			byte_type filter_;
			byte_type interlace_;
			
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
				if (!this->xsignature(in)) throw std::runtime_error("unmatch signature");
				if (!this->xheader(in)) throw std::runtime_error("wrong header");
				
				if (type_ != 3) {
					size_ = width_ * height_ * 3;
					return true;
				}
				
				uint32_t size;
				byte_array type(4);
				typedef typename istream_type::traits_type traits;
				while (!traits::eq_int_type(traits::to_int_type(in.peek()), traits::eof())) {
					size_type pos = in.tellg();
					this->xget(in, size);
					in.read(reinterpret_cast<char_type*>(&type[0]), type.size());
					if (in.gcount() < static_cast<int>(type.size()) || this->xtype(type, detail::iend)) return false;
					if (this->xtype(type, detail::idat)) {
						in.seekg(pos);
						break;
					}
					
					if (this->xtype(type, detail::plte)) {
						if (!this->xpalette(in, size)) throw std::runtime_error("cannot find palette");
					}
					else if (this->xtype(type, detail::trns)) this->xgetrns(in, size);
					else in.seekg(size, std::ios_base::cur);
					in.seekg(4, std::ios_base::cur); // CRC bits
				}
				size_ = width_ * height_;
				
				return true;
			}
			
			
			/* ------------------------------------------------------------- */
			//  xmakestream
			/* ------------------------------------------------------------- */
			template <class OutStream, class PDFManager>
			bool xmakestream(OutStream& out, PDFManager& pm, istream_type& in) {
				zstream ozf(out);
				byte_array data;
				if (!this->xgetdata(in, data)) return false;
				std::cout << "size: " << data.size() << std::endl;
				clx::basic_ivstream<CharT, Traits> vs(data);
				
				size_type bpp = this->xbpp(type_, depth_);
				size_type size = (depth_ * width_ * this->xbytes(type_) + 7) / 8;
				byte_array prev, line;
				for (size_type i = 0; i < height_; ++i) {
					if (!this->xdecode(vs, line, size, bpp, prev)) return false;
					this->xwrite(ozf, line);
					prev.assign(line.begin(), line.end());
				}
				ozf.finish();
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xbytes
			/* ------------------------------------------------------------- */
			size_type xbytes(size_type type) {
				switch (type) {
				case 0: return 1;
				case 2: return 3;
				case 3: return 1;
				case 4: return 2;
				case 6: return 4;
				default:
					break;
				}
				return 0;
			}
			
			/* ------------------------------------------------------------- */
			//  xbpp
			/* ------------------------------------------------------------- */
			size_type xbpp(size_type type, size_type depth) {
				switch (type) {
				case 0: // gray
					if (depth & 0x0f) return 1;
					else if (depth == 16) return 2;
					break;
				case 2: // color, non-palette
					if (depth & 0x07) return 1;
					else if (depth == 8) return 3;
					else if (depth == 16) return 6;
					break;
				case 3: // color, palette
					if (depth & 0x07) return 1;
					break;
				case 4: // gray, alpha-channel
					if (depth == 8) return 1;
					else if (depth == 16) return 2;
					break;
				case 6: // color, alpha-channel
					if (depth == 8) return 4;
					else if (depth == 16) return 8;
					break;
				default:
					break;
				}
				return 0;
			}
			
			/* ------------------------------------------------------------- */
			//  xpeath
			/* ------------------------------------------------------------- */
			byte_type xpeath(byte_type left, byte_type up, byte_type lu) {
				int p0 = static_cast<int>(left) + static_cast<int>(up) - static_cast<int>(lu);
				int p1 = std::abs(p0 - static_cast<int>(left));
				int p2 = std::abs(p0 - static_cast<int>(up));
				int p3 = std::abs(p0 - static_cast<int>(lu));
				if (p1 <= p2 && p1 <= p3) return left;
				else if (p2 <= p3) return up;
				else return lu;
			}
			
			/* ------------------------------------------------------------- */
			//  xsignature
			/* ------------------------------------------------------------- */
			bool xsignature(istream_type& in) {
				for (size_type i = 0; i < sizeof(detail::signature); ++i) {
					byte_type xu8;
					this->xget(in, xu8);
					if (xu8 != detail::signature[i]) return false;
				}
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xtype
			/* ------------------------------------------------------------- */
			bool xtype(const byte_array& s1, const byte_type* s2) {
				if (std::memcmp(&s1[0], s2, s1.size()) != 0) return false;
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xtype
			/* ------------------------------------------------------------- */
			bool xtype(istream_type& in, const byte_type* type) {
				byte_type data[4];
				in.read(reinterpret_cast<char_type*>(&data[0]), 4);
				if (std::memcmp(&data[0], type, 4) != 0) return false;
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xheader
			/* ------------------------------------------------------------- */
			bool xheader(istream_type& in) {
				uint32_t size;
				this->xget(in, size);
				if (size != 13) return false;
				if (!this->xtype(in, detail::ihdr)) return false;
				this->xget(in, width_);
				this->xget(in, height_);
				this->xget(in, depth_);
				this->xget(in, type_);
				this->xget(in, compress_);
				this->xget(in, filter_);
				this->xget(in, interlace_);
				in.seekg(4, std::ios_base::cur); // CRC
				return in.good();
			}
			
			/* ------------------------------------------------------------- */
			//  xpalette
			/* ------------------------------------------------------------- */
			bool xpalette(istream_type& in, size_type size) {
				if (size > 256 * 3 || size % 3 > 0) return false;
				for (size_type i = 0; i < size / 3; ++i) {
					byte_type red, green, blue;
					this->xget(in, red);
					this->xget(in, green);
					this->xget(in, blue);
					size_type rgb = 0;
					rgb |= (static_cast<size_type>(red) << 16);
					rgb |= (static_cast<size_type>(green) << 8);
					rgb |= (static_cast<size_type>(blue));
					colors_.push_back(rgb);
				}
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xgetrns
			/* ------------------------------------------------------------- */
			bool xgetrns(istream_type& in, size_type n) {
				alphas_.resize(n, 0);
				in.read(reinterpret_cast<char_type*>(&alphas_.at(0)), alphas_.size());
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xgetdata
			/* ------------------------------------------------------------- */
			bool xgetdata(istream_type& in, byte_array& dest) {
				uint32_t size;
				byte_array type(4);
				while (1) {
					this->xget(in, size);
					in.read(reinterpret_cast<char_type*>(&type[0]), type.size());
					if (in.gcount() < static_cast<int>(type.size()) || this->xtype(type, detail::iend)) return false;
					if (this->xtype(type, detail::idat)) break;
					in.seekg(size + 4, std::ios_base::cur);
					if (in.fail()) return false;
				}
				
				// read all chunk data of IDAT field.
				std::basic_stringstream<CharT, Traits> ss;
				do {
					dest.resize(size);
					in.read(reinterpret_cast<char_type*>(&dest[0]), dest.size());
					ss.write(reinterpret_cast<char_type*>(&dest[0]), dest.size());
					in.seekg(4, std::ios_base::cur); // CRC fields.
					size_type pos = in.tellg();
					this->xget(in, size);
					in.read(reinterpret_cast<char_type*>(&type[0]), type.size());
					if (in.gcount() < static_cast<int>(type.size())) return false;
					if (!this->xtype(type, detail::idat)) {
						in.seekg(pos);
						break;
					}
				} while (1);
				
				unzstream z(ss, 65536);
				char_type data[65536];
				dest.clear();
				do {
					z.read(data, 65536);
					dest.insert(dest.end(), data, data + z.gcount());
				} while (z.gcount() > 0);
				
				return in.good();
			}
			
			/* ------------------------------------------------------------- */
			//  xdecode
			/* ------------------------------------------------------------- */
			bool xdecode(istream_type& in, byte_array& dest, size_type size,
				size_type bpp, const byte_array& prev) {
				typedef typename byte_array::value_type value_type;
				byte_type filter;
				this->xget(in, filter);
				
				dest.clear();
				byte_type c;
				switch (filter) {
				case 0: // None
					dest.resize(size);
					in.read(reinterpret_cast<char_type*>(&dest[0]), dest.size());
					if (in.gcount() < static_cast<int>(dest.size())) return false;
					break;
				case 1: // Sub
					dest.resize(bpp);
					in.read(reinterpret_cast<char_type*>(&dest[0]), dest.size());
					if (in.gcount() < static_cast<int>(dest.size())) return false;
					for (size_type i = bpp; i < size; ++i) {
						this->xget(in, c);
						c += dest.at(i - bpp);
						dest.push_back(c);
					}
					break;
				case 2: // Up
					if (!prev.empty() && size > prev.size()) return false;
					for (size_type i = 0; i < size; ++i) {
						this->xget(in, c);
						if (!prev.empty()) c += prev.at(i);
						dest.push_back(c);
					}
					break;
				case 3: // Average
					if (!prev.empty() && size > prev.size()) return false;
					for (size_type i = 0; i < size; ++i) {
						this->xget(in, c);
						if (i < bpp) {
							if (!prev.empty()) c += prev.at(i) / 2;
						}
						else {
							byte_type up = (!prev.empty()) ? prev.at(i) : 0;
							c += (dest.at(i - bpp) + up) / 2;
						}
						dest.push_back(c);
					}
					break;
				case 4: // Peath
					if (!prev.empty() && size > prev.size()) return false;
					for (size_type i = 0; i < size; ++i) {
						this->xget(in, c);
						if (i < bpp) {
							if (!prev.empty()) c += prev.at(i);
						}
						else {
							if (prev.empty()) c += dest.at(i - bpp);
							else c += this->xpeath(dest.at(i - bpp), prev.at(i), prev.at(i - bpp));
						}
						dest.push_back(c);
					}
					break;
				default:
					return false;
					break;
				}
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xwrite
			/* ------------------------------------------------------------- */
			template <class OutStream>
			bool xwrite(OutStream& out, const byte_array& src) {
				if (!colors_.empty()) {
					for (size_type i = 0; i < src.size(); ++i) {
						byte_type c = src.at(i);
						// Memo: 暫定．ビットの順序が反対になっている場合があるように思える．
						if (src.at(i) >= colors_.size()) {
							c = (((src.at(i) & 0x01) << 7) |
								 ((src.at(i) & 0x02) << 5) |
								 ((src.at(i) & 0x04) << 3) |
								 ((src.at(i) & 0x08) << 1) |
								 ((src.at(i) & 0x10) >> 1) |
								 ((src.at(i) & 0x20) >> 3) |
								 ((src.at(i) & 0x40) >> 5) |
								 ((src.at(i) & 0x80) >> 7));
						}
						
						if (depth_ < 8) {
							size_type n = 8 / depth_;
							for (size_type j = 0; j < n; ++j) {
								byte_type cc = c & clx::mpl::lower_mask<1>::value;
								out << static_cast<char_type>(cc);
								c >>= 1;
							}
						}
						else {
							out << static_cast<char_type>(c);
						}
						//out << static_cast<char_type>(c);
					}
					//out.write(reinterpret_cast<const char_type*>(&src[0]), src.size());
				}
				else if (type_ & 0x02) { // color
					size_type i = 0;
					while (i + 2 < src.size()) {
						out.write(reinterpret_cast<const char_type*>(&src[i]), 3);
						i += 3;
						if (type_ & 0x04) ++i;
					}
				}
				else { // gray
					size_type i = 0;
					while (i < src.size()) {
						out << static_cast<char_type>(src.at(i));
						out << static_cast<char_type>(src.at(i));
						out << static_cast<char_type>(src.at(i));
						++i;
						if (type_ & 0x04) ++i;
					}
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
		
		/* ----------------------------------------------------------------- */
		/*
		 *  alpha_png_resource
		 *
		 *  透過処理を追加した PNG．暫定クラス．
		 */
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class alpha_png_resource {
		public:
			typedef size_t size_type;
			typedef unsigned char byte_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef std::basic_istream<CharT, Traits> istream_type;
			typedef clx::basic_zstream<Z_DEFAULT_COMPRESSION, CharT, Traits> zstream;
			typedef clx::basic_unzstream<CharT, Traits> unzstream;
			
			alpha_png_resource(const alpha_png_resource& cp) :
				in_(cp.in_), index_(cp.index_), label_(cp.label()),
				size_(cp.size_), width_(cp.width_), width_prev_(cp.width_prev_),
				height_(cp.height_), height_prev_(cp.height_prev_),
				colors_(cp.colors_), alphas_(cp.alphas_) {}
			
			alpha_png_resource(istream_type& in) :
				in_(in), index_(0), label_(),
				size_(0), width_(0), width_prev_(0),
				height_(0), height_prev_(0), colors_(), alphas_() {
				label_ = makelabel(LITERAL("Im"));
			}
			
			template <class OutStream, class PDFManager>
			bool operator()(OutStream& out, PDFManager& pm) {
				if (in_.bad()) throw std::runtime_error("cannot find input file");
				this->xproperty(in_);
				this->xobject(out, pm);
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			const string_type& label() const { return label_; }
			size_type index() const { return index_; }
			size_type size() const { return size_; }
			size_type width() const { return width_; }
			size_type height() const { return height_; }
			size_type worigin() const { return width_prev_; }
			size_type horigin() const { return height_prev_; }
			
		private:
			typedef clx::basic_format<CharT, Traits> fmt;
			typedef std::vector<byte_type> byte_array;
			
			istream_type& in_;
			size_type index_;
			string_type label_;
			size_type size_;
			size_type width_;
			size_type width_prev_;
			size_type height_;
			size_type height_prev_;
			std::vector<size_type> colors_;
			std::vector<byte_type> alphas_;
			
			byte_type type_;
			byte_type depth_;
			byte_type compress_;
			byte_type filter_;
			byte_type interlace_;
			//std::string alpha_string_;
			std::vector<char_type> alpha_string_;
			size_type pic_len_;
			
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
				//out << LITERAL("/Width ") << pic_len_ << std::endl;
				//out << LITERAL("/Height ") << pic_len_ << std::endl;
				//if ((type_ & 0x02) && (type_ & 0x04)) {
				if (((type_ & 0x02) && (type_ & 0x04)) ||
					((type_ & 0x03) && !alphas_.empty())) {
					out << fmt(LITERAL("/SMask %d 0 R")) % (pm.index() + 2) << std::endl;
				}
				out << LITERAL("/BitsPerComponent 8") << std::endl;
				//out << LITERAL("/Filter [ /ASCIIHexDecode ]") << std::endl;
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
				out << LITERAL("endstream") << std::endl;
				out << LITERAL("endobj") << std::endl;
				out << std::endl;
				
				out << fmt(LITERAL("%d 0 obj")) % pm.newindex() << std::endl;
				out << size << std::endl;
				out << LITERAL("endobj") << std::endl;
				out << std::endl;
				
				// SMask start
				//if ((type_ & 0x02) && (type_ & 0x04)) {
				if (((type_ & 0x02) && (type_ & 0x04)) ||
					((type_ & 0x03) && !alphas_.empty())) {
					out << fmt(LITERAL("%d 0 obj")) % pm.newindex() << std::endl;
					out << LITERAL("<<") << std::endl;
					out << LITERAL("/Type /XObject") << std::endl;
					out << LITERAL("/Subtype /Image") << std::endl;
					out << LITERAL("/Width ") << pic_len_ << std::endl;
					out << LITERAL("/Height ") << pic_len_ << std::endl;
					out << LITERAL("/BitsPerComponent 8") << std::endl;
					out << LITERAL("/Filter [ /FlateDecode ]") << std::endl;
					//out << LITERAL("/Filter [ /ASCIIHexDecode ]") << std::endl;
					out << LITERAL("/ColorSpace /DeviceGray") << std::endl;
					out << fmt(LITERAL("/Length %d 0 R")) % (pm.index() + 1) << std::endl;
					out << LITERAL(">>") << std::endl;
					out << LITERAL("stream") << std::endl;
					pos = static_cast<size_type>(out.tellp());
					//out << alpha_string_;
					{
						zstream ozf(out);
						ozf.write(reinterpret_cast<char_type*>(&alpha_string_[0]), alpha_string_.size());
						ozf.finish();
					}
					out << std::endl;
					size = static_cast<size_type>(out.tellp()) - pos;
					out << LITERAL("endstream") << std::endl;
					out << LITERAL("endobj") << std::endl;
					out << std::endl;
					
					out << fmt(LITERAL("%d 0 obj")) % pm.newindex() << std::endl;
					out << size << std::endl;
					out << LITERAL("endobj") << std::endl;
					out << std::endl;
				}
				// SMask end
				
				return status;
			}
			
			/* ------------------------------------------------------------- */
			//  xproperty
			/* ------------------------------------------------------------- */
			bool xproperty(istream_type& in) {
				if (in.bad() || !this->xsignature(in)) return false;
				if (!this->xheader(in)) return false;
				
				// 2^n * 2^n start
				width_prev_ = width_;
				height_prev_ = height_;
				if ((type_ & 0x02) && (type_ & 0x04)) {
					size_type pic_w = 1;
					size_type pic_h = 1;
					while (pic_w < width_) pic_w *= 2;
					while (pic_h < height_) pic_h *= 2;
					pic_len_ = std::max(pic_w, pic_h);
					
					width_ = pic_len_;
					height_ = pic_len_;
				}
				// 2^n * 2^n end
				
				if (type_ != 3) {
					//size_ = width_ * height_;
					size_ = width_ * height_ * 3;
					return true;
				}
				
				uint32_t size;
				byte_array type(4);
				typedef typename istream_type::traits_type traits;
				while (!traits::eq_int_type(traits::to_int_type(in.peek()), traits::eof())) {
					size_type pos = in.tellg();
					this->xget(in, size);
					in.read(reinterpret_cast<char_type*>(&type[0]), type.size());
					if (in.gcount() < static_cast<int>(type.size()) || this->xtype(type, detail::iend)) return false;
					if (this->xtype(type, detail::idat)) {
						in.seekg(pos);
						break;
					}
					
					if (this->xtype(type, detail::plte)) {
						if (!this->xpalette(in, size)) throw std::runtime_error("cannot find palette");
					}
					else if (this->xtype(type, detail::trns)) this->xgetrns(in, size);
					else in.seekg(size, std::ios_base::cur);
					in.seekg(4, std::ios_base::cur); // CRC bits
				}
				//size_ = width_ * height_ * 3;
				
				if ((type_ & 0x03) && !alphas_.empty()) {
					size_type pic_w = 1;
					size_type pic_h = 1;
					while (pic_w < width_) pic_w *= 2;
					while (pic_h < height_) pic_h *= 2;
					pic_len_ = std::max(pic_w, pic_h);
					
					width_ = pic_len_;
					height_ = pic_len_;
				}
				size_ = width_ * height_;
				//size_ = pic_len_ * pic_len_;
				
				return true;
			}
			
			
			/* ------------------------------------------------------------- */
			//  xmakestream
			/* ------------------------------------------------------------- */
			template <class OutStream, class PDFManager>
			bool xmakestream(OutStream& out, PDFManager& pm, istream_type& in) {
				zstream ozf(out);
				byte_array data;
				if (!this->xgetdata(in, data)) return false;
				clx::basic_ivstream<CharT, Traits> vs(data);
				
				size_type bpp = this->xbpp(type_, depth_);
				size_type size = (depth_ * width_prev_ * this->xbytes(type_) + 7) / 8;
				byte_array prev, line;
				for (size_type i = 0; i < height_prev_; ++i) {
					if (!this->xdecode(vs, line, size, bpp, prev)) return false;
					this->xwrite(ozf, line);
					prev.assign(line.begin(), line.end());
					//out << std::endl;
					//if (type_ & 0x04) alpha_string_ += "\n";
				}
				
				// Alpha pading
				//if (type_ & 0x04) {
				if ((type_ & 0x02) && (type_ & 0x04)) {
					for (size_type j = height_prev_; j < pic_len_; ++j) {
						for (size_type k = 0; k < pic_len_; ++k) {
							ozf << (CharT)(0);
							ozf << (CharT)(0);
							ozf << (CharT)(0);
							//out << "000000";
							//alpha_string_ += "00";
							alpha_string_.push_back(0);
						}
						//out << std::endl;
						//alpha_string_ += "\n";
					}
				}
				else if ((type_ & 0x03) && !alphas_.empty()) {
					for (size_type j = height_prev_; j < pic_len_; ++j) {
						for (size_type k = 0; k < pic_len_; ++k) {
							ozf << (CharT)(0);
							alpha_string_.push_back(0);
						}
					}
				}
				ozf.finish();
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xbytes
			/* ------------------------------------------------------------- */
			size_type xbytes(size_type type) {
				switch (type) {
				case 0: return 1;
				case 2: return 3;
				case 3: return 1;
				case 4: return 2;
				case 6: return 4;
				default:
					break;
				}
				return 0;
			}
			
			/* ------------------------------------------------------------- */
			//  xbpp
			/* ------------------------------------------------------------- */
			size_type xbpp(size_type type, size_type depth) {
				switch (type) {
				case 0: // gray
					if (depth & 0x0f) return 1;
					else if (depth == 16) return 2;
					break;
				case 2: // color, non-palette
					if (depth & 0x07) return 1;
					else if (depth == 8) return 3;
					else if (depth == 16) return 6;
					break;
				case 3: // color, palette
					if (depth & 0x07) return 1;
					break;
				case 4: // gray, alpha-channel
					if (depth == 8) return 1;
					else if (depth == 16) return 2;
					break;
				case 6: // color, alpha-channel
					if (depth == 8) return 4;
					else if (depth == 16) return 8;
					break;
				default:
					break;
				}
				
				return 0;
			}
			
			/* ------------------------------------------------------------- */
			//  xpeath
			/* ------------------------------------------------------------- */
			byte_type xpeath(byte_type left, byte_type up, byte_type lu) {
				int p0 = static_cast<int>(left) + static_cast<int>(up) - static_cast<int>(lu);
				int p1 = std::abs(p0 - static_cast<int>(left));
				int p2 = std::abs(p0 - static_cast<int>(up));
				int p3 = std::abs(p0 - static_cast<int>(lu));
				if (p1 <= p2 && p1 <= p3) return left;
				else if (p2 <= p3) return up;
				else return lu;
			}
			
			/* ------------------------------------------------------------- */
			//  xsignature
			/* ------------------------------------------------------------- */
			bool xsignature(istream_type& in) {
				for (size_type i = 0; i < sizeof(detail::signature); ++i) {
					byte_type xu8;
					if (!this->xget(in, xu8)) return false;
					if (xu8 != detail::signature[i]) return false;
				}
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xtype
			/* ------------------------------------------------------------- */
			bool xtype(const byte_array& s1, const byte_type* s2) {
				if (std::memcmp(&s1[0], s2, s1.size()) != 0) return false;
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xtype
			/* ------------------------------------------------------------- */
			bool xtype(istream_type& in, const byte_type* type) {
				byte_type data[4];
				in.read(reinterpret_cast<char_type*>(&data[0]), 4);
				if (std::memcmp(&data[0], type, 4) != 0) return false;
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xheader
			/* ------------------------------------------------------------- */
			bool xheader(istream_type& in) {
				uint32_t size;
				if (!this->xget(in, size)) return false;
				if (size != 13) return false;
				if (!this->xtype(in, detail::ihdr)) return false;
				this->xget(in, width_);
				this->xget(in, height_);
				this->xget(in, depth_);
				this->xget(in, type_);
				this->xget(in, compress_);
				this->xget(in, filter_);
				this->xget(in, interlace_);
				in.seekg(4, std::ios_base::cur); // CRC
				return in.good();
			}
			
			/* ------------------------------------------------------------- */
			//  xpalette
			/* ------------------------------------------------------------- */
			bool xpalette(istream_type& in, size_type size) {
				if (size > 256 * 3 || size % 3 > 0) return false;
				for (size_type i = 0; i < size / 3; ++i) {
					byte_type red, green, blue;
					this->xget(in, red);
					this->xget(in, green);
					this->xget(in, blue);
					size_type rgb = 0;
					rgb |= (static_cast<size_type>(red) << 16);
					rgb |= (static_cast<size_type>(green) << 8);
					rgb |= (static_cast<size_type>(blue));
					colors_.push_back(rgb);
				}
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xgetrns
			/* ------------------------------------------------------------- */
			bool xgetrns(istream_type& in, size_type n) {
				alphas_.resize(n, 0);
				in.read(reinterpret_cast<char_type*>(&alphas_.at(0)), alphas_.size());
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xgetdata
			/* ------------------------------------------------------------- */
			bool xgetdata(istream_type& in, byte_array& dest) {
				uint32_t size;
				byte_array type(4);
				while (1) {
					if (!this->xget(in, size)) return false;
					in.read(reinterpret_cast<char_type*>(&type[0]), type.size());
					if (in.gcount() < static_cast<int>(type.size()) || this->xtype(type, detail::iend)) return false;
					if (this->xtype(type, detail::idat)) break;
					in.seekg(size + 4, std::ios_base::cur);
					if (in.fail()) return false;
				}
				
				// read all chunk data of IDAT field.
				std::basic_stringstream<CharT, Traits> ss;
				do {
					dest.resize(size);
					in.read(reinterpret_cast<char_type*>(&dest[0]), dest.size());
					ss.write(reinterpret_cast<char_type*>(&dest[0]), dest.size());
					in.seekg(4, std::ios_base::cur); // CRC fields.
					size_type pos = in.tellg();
					if (!this->xget(in, size)) return false;
					in.read(reinterpret_cast<char_type*>(&type[0]), type.size());
					if (in.gcount() < static_cast<int>(type.size())) return false;
					if (!this->xtype(type, detail::idat)) {
						in.seekg(pos);
						break;
					}
				} while (1);
				
				unzstream z(ss, 65536);
				char_type data[65536];
				dest.clear();
				do {
					z.read(data, 65536);
					dest.insert(dest.end(), data, data + z.gcount());
				} while (z.gcount() > 0);
				
				return in.good();
			}
			
			/* ------------------------------------------------------------- */
			//  xdecode
			/* ------------------------------------------------------------- */
			bool xdecode(istream_type& in, byte_array& dest, size_type size,
				size_type bpp, const byte_array& prev) {
				typedef typename byte_array::value_type value_type;
				byte_type filter;
				if (!this->xget(in, filter)) return false;
				
				dest.clear();
				byte_type c;
				switch (filter) {
				case 0: // None
					dest.resize(size);
					in.read(reinterpret_cast<char_type*>(&dest[0]), dest.size());
					if (in.gcount() < static_cast<int>(dest.size())) return false;
					break;
				case 1: // Sub
					dest.resize(bpp);
					in.read(reinterpret_cast<char_type*>(&dest[0]), dest.size());
					if (in.gcount() < static_cast<int>(dest.size())) return false;
					for (size_type i = bpp; i < size; ++i) {
						if (!this->xget(in, c)) return false;
						c += dest.at(i - bpp);
						dest.push_back(c);
					}
					break;
				case 2: // Up
					if (!prev.empty() && size > prev.size()) return false;
					for (size_type i = 0; i < size; ++i) {
						if (!this->xget(in, c)) return false;
						if (!prev.empty()) c += prev.at(i);
						dest.push_back(c);
					}
					break;
				case 3: // Average
					if (!prev.empty() && size > prev.size()) return false;
					for (size_type i = 0; i < size; ++i) {
						if (!this->xget(in, c)) return false;
						if (i < bpp) {
							if (!prev.empty()) c += prev.at(i) / 2;
						}
						else {
							byte_type up = (!prev.empty()) ? prev.at(i) : 0;
							c += (dest.at(i - bpp) + up) / 2;
						}
						dest.push_back(c);
					}
					break;
				case 4: // Peath
					if (!prev.empty() && size > prev.size()) return false;
					for (size_type i = 0; i < size; ++i) {
						if (!this->xget(in, c)) return false;
						if (i < bpp) {
							if (!prev.empty()) c += prev.at(i);
						}
						else {
							if (prev.empty()) c += dest.at(i - bpp);
							else c += this->xpeath(dest.at(i - bpp), prev.at(i), prev.at(i - bpp));
						}
						dest.push_back(c);
					}
					break;
				default:
					return false;
					break;
				}
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xwrite
			/* ------------------------------------------------------------- */
			template <class OutStream>
			bool xwrite(OutStream& out, const byte_array& src) {
				if (!colors_.empty()) {
					for (size_type i = 0; i < src.size(); ++i) {
						byte_type c = src.at(i);
						// Memo: 暫定．ビットの順序が反対になっている場合があるように思える．
						if (src.at(i) >= colors_.size()) {
							c = (((src.at(i) & 0x01) << 7) |
								 ((src.at(i) & 0x02) << 5) |
								 ((src.at(i) & 0x04) << 3) |
								 ((src.at(i) & 0x08) << 1) |
								 ((src.at(i) & 0x10) >> 1) |
								 ((src.at(i) & 0x20) >> 3) |
								 ((src.at(i) & 0x40) >> 5) |
								 ((src.at(i) & 0x80) >> 7));
						}
						
						if (depth_ < 8) {
							size_type n = 8 / depth_;
							for (size_type j = 0; j < n; ++j) {
								byte_type cc = c & clx::mpl::lower_mask<1>::value;
								out << static_cast<char_type>(cc);
								if (!alphas_.empty()) alpha_string_.push_back(alphas_.at(cc));
								c >>= 1;
							}
						}
						else {
							out << static_cast<char_type>(c);
							if (!alphas_.empty()) alpha_string_.push_back(alphas_.at(c));
						}
					}
					//out.write(reinterpret_cast<const char_type*>(&src[0]), src.size());
					
					// Alpha pading
					if (!alphas_.empty()) {
						for(size_type i = width_prev_; i < pic_len_; ++i){
							out << (CharT)(0);
							alpha_string_.push_back(0);
						}
					}
				}
				else if (type_ & 0x02) { // color
					size_type i = 0;
					while (i + 2 < src.size()) {
						out.write(reinterpret_cast<const char_type*>(&src[i]), 3);
						i += 3;
						
						// Alpha
						if (type_ & 0x04) {
							alpha_string_.push_back(src[i]);
							++i;
						}
					}
					
					// Alpha pading
					if (type_ & 0x04) {
						for(size_type i = width_prev_; i < pic_len_; ++i){
							//out << "000000";
							out << (CharT)(0);
							out << (CharT)(0);
							out << (CharT)(0);
							//alpha_string_ += "00";
							alpha_string_.push_back(0);
						}
					}
				}
				else { // gray
					size_type i = 0;
					while (i < src.size()) {
						out << static_cast<char_type>(src.at(i));
						out << static_cast<char_type>(src.at(i));
						out << static_cast<char_type>(src.at(i));
						++i;
						if (type_ & 0x04) ++i;
					}
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
		
		typedef basic_image<png_resource<char>, char> png;
		typedef basic_image<alpha_png_resource<char>, char> alpha_png;
	}
}

#endif // FAML_PDF_PNG_H
