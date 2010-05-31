/* ------------------------------------------------------------------------- */
/*
 *  text.h
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
 *  Last-modified: Sun 19 Apr 2009 17:27:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PDF_TEXT_H
#define FAML_PDF_TEXT_H

#include <deque>
#include <string>
#include <iterator>
#include <istream>
#include <sstream>
#include "page.h"
#include "font.h"
#include "color.h"
#include "coordinate.h"
#include "text_contents.h"
#include "text_helper.h"
#include "clx/format.h"
#include "clx/hexdump.h"
#include "clx/strip.h"
#ifdef FAML_USE_ZLIB
#include "clx/zstream.h"
#endif

namespace faml {
	namespace pdf {
		/* ----------------------------------------------------------------- */
		//  basic_text
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_text {
		public:
			typedef size_t size_type;
			typedef unsigned char byte_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef std::basic_istream<CharT, Traits> istream_type;
			typedef basic_font<CharT, Traits> font_type;
			
			basic_text(istream_type& in, font_type& f) :
				in_(in), font_(&f), fontj_(NULL), size_(12.0),
				space_(12.0 * 0.25), color_(), page_() {}
				
			basic_text(istream_type& in, font_type& f, font_type& fj) :
				in_(in), font_(&f), fontj_(&fj), size_(12.0),
				space_(12.0 * 0.25), color_(), page_() {}
			
			template <class OutStream, class PDFManager>
			bool operator()(OutStream& out, PDFManager& pm) {
				if (in_.bad()) return false;
				if (!(*font_)(out, pm)) return false;
				if (fontj_) {
					if (!(*fontj_)(out, pm)) return false;
				}
				
#if 0 // Note: streambuf_iterator ÇæÇ∆ iterator Ç™ñﬂÇπÇ»Ç¢ÇÃÇ≈ÅCéÊÇËÇ†Ç¶Ç∏ì«Ç›çûÇﬁÅD
				std::istreambuf_iterator<char_type> first(in_);
				std::istreambuf_iterator<char_type> last;
#else
				
				std::vector<char_type> v;
				char_type data[65536];
				do {
					in_.read(data, 65536);
					v.insert(v.end(), data, data + in_.gcount());
				} while (in_.gcount() > 0);
				typename std::vector<char_type>::iterator first = v.begin();
				typename std::vector<char_type>::iterator last = v.end();
#endif
				
				while (first != last) {
					page_type newpage(page_);
					cur_.x = page_.margin().left();
					cur_.y = page_.height() - page_.margin().top();
					if (!this->xresources(out, pm)) return false;
					newpage.resources(pm.index());
					if (!this->xcontents(out, pm, first, last)) return false;
					newpage.contents(pm.index());
					if (!newpage(out, pm)) return false;
				}
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  Access methods (set).
			/* ------------------------------------------------------------- */
			void font1st(font_type& f) { font_ = &f; }
			void font2nd(font_type& f) { fontj_ = &f; }
			void font_size(double size) { size_ = size; }
			void font_color(const color& value) { color_ = value; }
			void line_space(double space) { space_ = space; }
			void page(const page_property& value) { page_ = value; }
			
		private:
			typedef clx::basic_format<CharT, Traits> fmt;
			typedef basic_text_contents<CharT, Traits> text_contents;
			typedef basic_page<CharT, Traits> page_type;
			typedef basic_font_property<CharT, Traits> font_property;
			
			istream_type& in_;
			font_type* font_;
			font_type* fontj_;
			double size_;
			double space_;
			color color_;
			page_property page_;
			
			/* ------------------------------------------------------------- */
			/*
			 *  current_status
			 *
			 *  The current_status structure keeps some information needed
			 *  for sub-records. When the parent class finished parsing,
			 *  values of the structure member were nonsense. Members of
			 *  the private structure depend on its parent class.
			 */
			/* ------------------------------------------------------------- */
			struct current_status {
				double x;
				double y;
				
				current_status() : x(0), y(0) {}
			};
			current_status cur_;
			
			/* ------------------------------------------------------------- */
			//  xresources
			/* ------------------------------------------------------------- */
			template <class OutStream, class PDFManager>
			bool xresources(OutStream& out, PDFManager& pm) {
				out << fmt(LITERAL("%d 0 obj")) % pm.newindex() << std::endl;
				out << LITERAL("<<") << std::endl;
				out << LITERAL("/ProcSet [ /PDF /Text ]") << std::endl;
				out << LITERAL("/Font") << std::endl;
				out << LITERAL("<<") << std::endl;
				out << fmt(LITERAL("/%s %d 0 R")) % font_->label() % font_->index() << std::endl;
				if (fontj_) out << fmt(LITERAL("/%s %d 0 R")) % fontj_->label() % fontj_->index() << std::endl;
				out << LITERAL(">>") << std::endl;
				out << LITERAL(">>") << std::endl;
				out << LITERAL("endobj") << std::endl;
				out << std::endl;
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xcontents
			/* ------------------------------------------------------------- */
			template <class OutStream, class PDFManager, class InIter>
			bool xcontents(OutStream& out, PDFManager& pm, InIter& first, InIter last) {
				std::basic_stringstream<char> ss;
				string_type filter;
#ifdef FAML_USE_ZLIB
				filter = LITERAL("/FlateDecode");
				clx::basic_zstream<Z_DEFAULT_COMPRESSION, char> z(ss);
				this->xmakestream(z, pm, first, last);
				z.finish();
#else
				this->xmakestream(ss, pm, first, last);
#endif
				size_type index = pm.newindex();
				out << fmt(LITERAL("%d 0 obj")) % index << std::endl;
				out << LITERAL("<< ") << std::endl;
				out << LITERAL("/Length ") << ss.str().size() << std::endl;
				out << fmt(LITERAL("/Filter [ %s ]")) % filter << std::endl;
				out << LITERAL(">>") << std::endl;
				out << LITERAL("stream") << std::endl;
				out << ss.str();
				out << std::endl;
				out << LITERAL("endstream") << std::endl;
				out << LITERAL("endobj") << std::endl;
				out << std::endl;
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xmakestream
			/* ------------------------------------------------------------- */
			template <class OutStream, class PDFManager, class InIter>
			bool xmakestream(OutStream& out, PDFManager& pm, InIter& first, InIter last) {
				out << LITERAL("BT") << std::endl;
				text_helper helper(font_->property(), font_->charset());
				while (cur_.y - space_ > page_.margin().bottom()) {
					double width = page_.width() - cur_.x - page_.margin().right();
					int limit = static_cast<int>(width / size_ * 1000);
					string_type line;
					//int used = helper.getline(first, last, std::inserter(line, line.end()), limit);
					helper.getline(first, last, std::inserter(line, line.end()), limit);
					if (line.empty()) break;
					char_type c = line.at(line.size() - 1);
					bool newline = false;
					if (first == last) newline = true;
					else if (line.size() == 1 || (static_cast<int>(line.at(line.size() - 2)) & 0xff) < 0x81) {
						if (line.at(line.size() - 1) == 0x0a || line.at(line.size() - 1) == 0x0d) {
							newline = true;
							chomp(line, font_->charset());
						}
					}
					
					if (!line.empty()) {
						text_contents txt(line, coordinate(cur_.x, cur_.y), *font_, false);
						if (fontj_) txt.font2nd(*fontj_);
						txt.font_size(size_);
						//double sp = (limit - used) / 1000.0 * size_;
						//if (!newline) txt.space(sp);
						if (!txt(out, pm)) break;
					}
					cur_.y -= (size_ + space_);
					if (c == 0x0c) break;
				}
				out << LITERAL("ET");
				
				return true;
			}
		};
		
		typedef basic_text<char> text;
	}
}

#endif // FAML_PDF_TEXT_H
