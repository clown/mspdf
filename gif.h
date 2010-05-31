/* ------------------------------------------------------------------------- */
/*
 *  gif.h
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
 *  Last-modified: Sat 24 Jan 2009 13:27:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PDF_GIF_H
#define FAML_PDF_GIF_H

#include <string>
#include <deque>
#include <vector>
#include <iterator>
#include <istream>
#include <sstream>
#include <stdexcept>
#include "clx/literal.h"
#include "clx/format.h"
#include "clx/lexical_cast.h"
#include "clx/zstream.h"

#include "image.h"

namespace faml {
	namespace pdf {
		/* ----------------------------------------------------------------- */
		//  gif_decoder
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class gif_decoder {
		public:
			typedef size_t size_type;
			typedef unsigned char byte_type;
			typedef CharT char_type;
			typedef std::basic_istream<CharT, Traits> istream_type;
			
			enum { nbits = 12, nmax = 4095, nempty = 4098 };
			
			gif_decoder(istream_type& in) :
				in_(in), depth_(0), shift_state_(0), shift_data_(0),
				buffer_(), stack_(),
				suffix_(static_cast<size_type>(nmax + 1), static_cast<size_type>(nempty)),
				prefix_(static_cast<size_type>(nmax + 1), static_cast<size_type>(nempty)) {
				int n = 1 / sizeof(char_type);
				in_.read(reinterpret_cast<char_type*>(&depth_), n);
				if (in_.gcount() < n) throw std::runtime_error("wrong format");
				clear_code_ = 1 << depth_;
				eof_code_ = clear_code_ + 1;
				this->xclear();
			}
			
			template <class OutIterator>
			OutIterator operator()(OutIterator dest, size_type n) {
				size_type i = 0;
				while (!stack_.empty()) {
					*dest = stack_.back();
					stack_.pop_back();
					++dest;
					++i;
				}
				
				while (i < n) {
					size_type cur = xnext();
					if (cur == eof_code_) {
						if (i != n - 1) throw std::runtime_error("wrong format");
						++i;
					}
					else if (cur == clear_code_) this->xclear();
					else {
						if (cur < clear_code_) {
							*dest = cur;
							++dest;
							++i;
						}
						else {
							size_type cur_prefix = 0;
							
							if (prefix_.at(cur) == nempty) {
								if (cur == run_code_ - 2) {
									cur_prefix = prev_code_;
									stack_.push_back(this->xprefix(prev_code_));
									suffix_.at(run_code_ - 2) = stack_.back();
								}
								else throw std::runtime_error("wrong format");
							}
							else cur_prefix = cur;
							
							size_type j = 0;
							while (cur_prefix > clear_code_) {
								if (j >= nempty || cur_prefix > nempty) {
									throw std::runtime_error("wrong format");
								}
								stack_.push_back(suffix_.at(cur_prefix));
								cur_prefix = prefix_.at(cur_prefix);
								++j;
							}
							stack_.push_back(cur_prefix);
							
							while (!stack_.empty() && i < n) {
								*dest = stack_.back();
								stack_.pop_back();
								++dest;
								++i;
							}
						}
						
						if (prev_code_ != nempty) {
							prefix_.at(run_code_ - 2) = prev_code_;
							if (cur == run_code_ - 2) suffix_.at(run_code_ - 2) = this->xprefix(prev_code_);
							else suffix_.at(run_code_ - 2) = this->xprefix(cur);
						}
						prev_code_ = cur;
					}
				}
				
				return dest;
			}
			
		private:
			istream_type& in_;
			
			size_type depth_;
			
			size_type clear_code_;
			size_type eof_code_;
			size_type run_code_;
			size_type prev_code_;
			
			size_type run_bits_;
			size_type limit_code_;
			
			size_type shift_state_;
			size_type shift_data_;
			
			std::deque<byte_type> buffer_;
			std::deque<size_type> stack_;
			std::deque<size_type> suffix_;
			std::deque<size_type> prefix_;
			
			void xclear() {
				run_code_ = eof_code_ + 1;
				run_bits_ = depth_ + 1;
				limit_code_ = 1 << run_bits_;
				prev_code_ = nempty;
				for (size_type i = 0; i < prefix_.size(); ++i) prefix_.at(i) = nempty;
			}
			
			size_type xprefix(size_type code) {
				for (size_type i = 0; i < prefix_.size(); ++i) {
					if (code <= clear_code_) break;
					code = prefix_.at(code);
				}
				return code;
			}
			
			byte_type xnext_byte() {
				if (buffer_.empty()) {
					size_type size = 0;
					int n = 1 / sizeof(char_type);
					in_.read(reinterpret_cast<char_type*>(&size), n);
					if (in_.gcount() < n || size == 0) return 0;
					
					byte_type tmp[256];
					n = static_cast<int>(size) / sizeof(char_type);
					in_.read(reinterpret_cast<char_type*>(tmp), n);
					if (in_.gcount() < n) return 0;
					std::back_insert_iterator<std::deque<byte_type> > it(buffer_);
					std::copy(tmp, tmp + size, it);
				}
				
				byte_type dest = buffer_.front();
				buffer_.pop_front();
				return dest;
			}
			
			size_type xnext() {
				static const int mask[] = {
					0x0000, 0x0001, 0x0003, 0x0007,
					0x000f, 0x001f, 0x003f, 0x007f,
					0x00ff, 0x01ff, 0x03ff, 0x07ff,
					0x0fff
				};
				
				size_type code = nempty;
				
				while (shift_state_ < run_bits_) {
					byte_type c = xnext_byte();
					shift_data_ |= static_cast<unsigned long>(c) << shift_state_;
					shift_state_ += 8;
				}
				
				code = shift_data_ & mask[run_bits_];
				shift_data_ >>= run_bits_;
				shift_state_ -= run_bits_;
				
				if (++run_code_ > limit_code_ && run_bits_ < nbits) {
					limit_code_ <<= 1;
					++run_bits_;
				}
				
				return code;
			}
		};
		
		/* ----------------------------------------------------------------- */
		//  gif_resource
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class gif_resource {
		public:
			typedef size_t size_type;
			typedef unsigned char byte_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef std::basic_istream<CharT, Traits> istream_type;
			typedef clx::basic_zstream<Z_DEFAULT_COMPRESSION, CharT, Traits> zstream;
			typedef gif_decoder<CharT, Traits> decoder;
			
			gif_resource(istream_type& in) :
				in_(in), index_(0), label_(),
				size_(0), width_(0), height_(0), interlace_(false), colors_() {}
			
			template <class OutStream, class PDFManager>
			bool operator()(OutStream& out, PDFManager& pm) {
				if (in_.bad() || !this->xget()) return false;
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
			
			// internal information
			bool interlace_;
			size_type bits_;
			std::vector<size_type> colors_;
			
			// Note: 現在の実装では，最初の Image Block のみを処理する．
			bool xget_block() {
				// skip extension blocks
				byte_type flag = 0;
				int n = 0;
				while (1) {
					n = 1 / sizeof(char_type);
					in_.read(reinterpret_cast<char_type*>(&flag), n);
					if (in_.gcount() < n) return false;
					else if (flag == 0x2c) break;
					in_.seekg(1 / sizeof(char_type), std::ios_base::cur);
					
					while (1) {
						size_type size = 0;
						n = 1 / sizeof(char_type);
						in_.read(reinterpret_cast<char_type*>(&size), n);
						if (in_.gcount() < n) return false;
						else if (size == 0) break;
						in_.seekg(size / sizeof(char_type), std::ios_base::cur);
					}
				}
				
				// skip the information of left/right position
				in_.seekg(4 / sizeof(char_type), std::ios_base::cur);
				
				int width = 0;
				n = 2 / sizeof(char_type);
				in_.read(reinterpret_cast<char_type*>(&width), n);
				if (in_.gcount() < n) return false;
				width_ = width;
				
				int height = 0;
				in_.read(reinterpret_cast<char_type*>(&height), n);
				if (in_.gcount() < n) return false;
				height_ = height;
				
				flag = 0;
				n = 1 / sizeof(char_type);
				in_.read(reinterpret_cast<char_type*>(&flag), n);
				if (in_.gcount() < n) return false;
				if (flag & 0x40) interlace_ = true;
				
				if (flag & 0x80) {
					bits_ = (flag & 0x07) + 1;
					size_type ncolors = 1 << bits_;
					for (size_type i = 0; i < ncolors; ++i) {
						size_type rgb = 0;
						n = 3 / sizeof(char_type);
						in_.read(reinterpret_cast<char_type*>(&rgb), n);
						if (in_.gcount() < n) return false;
						rgb = ((rgb & 0x0000ff) << 16) | (rgb & 0x00ff00) | ((rgb & 0xff0000) >> 16);
						colors_.push_back(rgb);
					}
				}
				size_ = width_ * height_ * bits_ / 8;
				
				return true;
			}
			
			bool xget() {
				static const size_type soi = 0x464947;
				static const size_type v87 = 0x613738;
				static const size_type v89 = 0x613938;
				
				// check gif format
				size_type buf = 0;
				int n = 3 / sizeof(char_type);
				in_.read(reinterpret_cast<char_type*>(&buf), n);
				if (in_.gcount() < n || buf != soi) return false;
				in_.read(reinterpret_cast<char_type*>(&buf), n);
				if (in_.gcount() < n || (buf != v87 && buf != v89)) return false;
				
				// get width and height
				n = 2 / sizeof(char_type);
				in_.read(reinterpret_cast<char_type*>(&width_), n);
				if (in_.gcount() < n) return false;
				in_.read(reinterpret_cast<char_type*>(&height_), n);
				if (in_.gcount() < n) return false;
				
				size_type ncolors = 0;
				byte_type flag = 0;
				n = 1 / sizeof(char_type);
				in_.read(reinterpret_cast<char_type*>(&flag), n);
				if (in_.gcount() < n || (flag >> 7) == 0) return false;
				bits_ = ((flag >> 4) & 0x07) + 1;
				ncolors = 1 << bits_;
				size_ = width_ * height_ * bits_ / 8;
				
				in_.seekg(2 / sizeof(char_type), std::ios_base::cur);
				n = 3 / sizeof(char_type);
				for (size_type i = 0; i < ncolors; ++i) {
					size_type rgb = 0;
					in_.read(reinterpret_cast<char_type*>(&rgb), n);
					if (in_.gcount() < n) return false;
					rgb = ((rgb & 0x0000ff) << 16) | (rgb & 0x00ff00) | ((rgb & 0xff0000) >> 16);
					colors_.push_back(rgb);
				}
				
				// Note: gif にイメージ (ImageBlock) が複数含まれる場合の出力方法の検討
				return this->xget_block();
			}
			
			template <class OutStream, class PDFManager>
			bool xobject(OutStream& out, PDFManager& pm) {
				index_ = pm.newindex();
				label_ = LITERAL("Im") + clx::lexical_cast<string_type>(index_);
				
				out << fmt(LITERAL("%d 0 obj")) % index_ << std::endl;
				out << LITERAL("<<") << std::endl;
				out << LITERAL("/Type /XObject") << std::endl;
				out << LITERAL("/Subtype /Image") << std::endl;
				out << fmt(LITERAL("/Name /%s")) % label_ << std::endl;
				out << fmt(LITERAL("/Width %d")) % width_ << std::endl;
				out << fmt(LITERAL("/Height %d")) % height_ << std::endl;
				out << LITERAL("/BitsPerComponent 8") << std::endl;
				out << LITERAL("/Filter [ /FlateDecode ]") << std::endl;
				
				out << fmt(LITERAL("/ColorSpace [ /Indexed /DeviceRGB %d < ")) % (colors_.size() - 1);
				for (size_type i = 0; i < colors_.size(); ++i) out << fmt(LITERAL("%06X ")) % colors_.at(i);
				out << LITERAL("> ]") << std::endl;
				
				out << fmt(LITERAL("/Length %d 0 R")) % (pm.index() + 1) << std::endl;
				out << LITERAL(">>") << std::endl;
				out << LITERAL("stream") << std::endl;
				size_type pos = static_cast<size_type>(out.tellp());
				bool status = this->xblock(out);
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
			
			template <class OutStream>
			bool xblock(OutStream& out) {
				// for interlace
				static const int start[] = {0, 4, 2, 1};
				static const int step[] = {8, 8, 4, 2};
				
				try {
					clx::zstream z(out);
					decoder dec(in_);
					if (interlace_) {
						std::vector<string_type> buf(height_, string_type());
						for (size_type i = 0; i < 4; ++i) {
							size_type row = start[i];
							while (row < height_) {
								string_type tmp;
								std::insert_iterator<string_type> pos(tmp, tmp.end());
								dec(pos, width_);
								buf.at(row) = tmp;
								row += step[i];
							}
						}
						
						for (size_type i = 0; i < buf.size(); ++i) {
							z.write(buf.at(i).c_str(), buf.at(i).size());
						}
					}
					else {
						std::ostream_iterator<char_type> pos(z);
						for (size_type row = 0; row < height_; ++row) pos = dec(pos, width_);
					}
				}
				catch (std::runtime_error&) {
					return false;
				}
				
				return true;
			}
		};
		
		typedef basic_image<gif_resource<char>, char> gif;
	}
}

#endif // FAML_PDF_GIF_H
