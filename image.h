/* ------------------------------------------------------------------------- */
/*
 *  image.h
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
 *  Last-modified: Wed 15 Apr 2009 21:04:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PDF_IMAGE_H
#define FAML_PDF_IMAGE_H

#include <algorithm>
#include <string>
#include <iterator>
#include <istream>
#include <sstream>
#include "clx/literal.h"
#include "clx/format.h"
#include "page.h"

namespace faml {
	namespace pdf {
		namespace scale {
			enum { shrink = -2, expand = -1, full = 0, original = 100 };
		}
		
		namespace align {
			enum { left = 0x01, center = 0x02, right = 0x03,
				top = 0x10, middle = 0x20, bottom = 0x30 };
		}
		
		/* ----------------------------------------------------------------- */
		//  basic_image
		/* ----------------------------------------------------------------- */
		template <
			class Resource,
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_image {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef std::basic_istream<CharT, Traits> istream_type;
			typedef Resource resource_type;
			
			basic_image(istream_type& in = std::cin) :
				page_(0, 0), f_(in),
				scale_(scale::original), align_(align::top | align::left) {}
			
			basic_image(istream_type& in, const page_property& page,
				int x = scale::expand, int pos = align::center | align::middle) :
				page_(page), f_(in), scale_(x), align_(pos) {}
			
			template <class OutStream, class PDFManager>
			bool operator()(OutStream& out, PDFManager& pm) {
				if (!f_(out, pm)) return false;
				if (page_.width() == 0 || page_.height() == 0) {
					page_.width(f_.width() + page_.margin().left() + page_.margin().right());
					page_.height(f_.height() + page_.margin().top() + page_.margin().bottom());
				}
				
				page_type newpage(page_);
				this->xresources(out, pm);
				newpage.resources(pm.index());
				this->xcontents(out, pm);
				newpage.contents(pm.index());
				return newpage(out, pm);
			}
			
			/* ------------------------------------------------------------- */
			//  Access methods (set).
			/* ------------------------------------------------------------- */
			void scale(int x) { scale_ = x; }
			void align(int pos) { align_ = pos; }
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			int scale() const { return scale_; }
			int align() const { return align_; }
			
		private:
			typedef clx::basic_format<CharT, Traits> fmt;
			typedef basic_page<CharT, Traits> page_type;
			
			page_property page_;
			resource_type f_;
			int scale_;
			int align_;
			
			/* ------------------------------------------------------------- */
			//  xresources
			/* ------------------------------------------------------------- */
			template <class OutStream, class PDFManager>
			bool xresources(OutStream& out, PDFManager& pm) {
				int index = pm.newindex();
				out << fmt(LITERAL("%d 0 obj")) % index << std::endl;
				out << LITERAL("<<") << std::endl;
				out << fmt(LITERAL("/XObject << /%s %d 0 R >>")) % f_.label() % f_.index() << std::endl;
				out << LITERAL("/ProcSet [ /PDF /ImageC ]") << std::endl;
				out << LITERAL(">>") << std::endl;
				out << LITERAL("endobj") << std::endl;
				out << std::endl;
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xcontents
			/* ------------------------------------------------------------- */
			template <class OutStream, class PDFManager>
			bool xcontents(OutStream& out, PDFManager& pm) {
				size_type index = pm.newindex();
				std::pair<size_type, size_type> size = this->xsize();
				std::pair<size_type, size_type> pos = this->xposition(size.first, size.second);
				
				std::basic_stringstream<CharT, Traits> ss;
				ss << LITERAL("q") << std::endl;
				ss << fmt(LITERAL("%d 0 0 %d %d %d cm"))
					% size.first % size.second % pos.first % pos.second << std::endl;
				ss << fmt(LITERAL("/%s Do")) % f_.label() << std::endl;
				ss << LITERAL("Q");
				
				out << fmt(LITERAL("%d 0 obj")) % index << std::endl;
				out << fmt(LITERAL("<< /Length %d >>")) % ss.str().size() << std::endl;
				out << LITERAL("stream") << std::endl;
				out << ss.str();
				out << std::endl;
				out << LITERAL("endstream") << std::endl;
				out << LITERAL("endobj") << std::endl;
				out << std::endl;
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xsize
			/* ------------------------------------------------------------- */
			std::pair<size_type, size_type> xsize() {
				std::pair<size_type, size_type> dest;
				
				if (scale_ > 0) {
					int width = static_cast<int>(f_.width() * scale_ / 100.0);
					int height = static_cast<int>(f_.height() * scale_ / 100.0);
					dest.first = std::max(width, page_.width());
					dest.second = std::max(height, page_.height());
				}
				else if (scale_ == scale::full) {
					dest.first = page_.width() - page_.margin().left() - page_.margin().right();
					dest.second = page_.height() - page_.margin().top() - page_.margin().bottom();
				}
				else {
					double x = (page_.width() - page_.margin().left() - page_.margin().right()) / static_cast<double>(f_.width());
					double y = (page_.height() - page_.margin().top() - page_.margin().bottom()) / static_cast<double>(f_.height());
					
					if (scale_ == scale::shrink && x >= 1.0 && y >= 1.0) {
						dest.first = f_.width();
						dest.second = f_.height();
					}
					else if (x <= y) {
						dest.first = static_cast<size_type>(f_.width() * x);
						dest.second = static_cast<size_type>(f_.height() * x);
					}
					else {
						dest.first = static_cast<size_type>(f_.width() * y);
						dest.second = static_cast<size_type>(f_.height() * y);
					}
				}
				
				return dest;
			}
			
			/* ------------------------------------------------------------- */
			//  xposition
			/* ------------------------------------------------------------- */
			std::pair<size_type, size_type> xposition(size_type w, size_type h) {
				std::pair<size_type, size_type> dest;
				
				int pos = 0;
				switch (align_ & 0x0f) {
				case align::center:
					pos = std::max(static_cast<int>(page_.width() - w) / 2, 0);
					dest.first = static_cast<size_type>(pos);
					break;
				case align::right:
					pos = std::max(static_cast<int>(page_.width() - (w + page_.margin().right())), 0);
					dest.first = static_cast<size_type>(pos);
					break;
				case align::left:
				default:
					dest.first = page_.margin().left();
					break;
				}
				
				switch (align_ & 0xf0) {
				case align::top:
					pos = std::max(static_cast<int>(page_.height() - (h + page_.margin().top())), 0);
					dest.second = static_cast<size_type>(pos);
					break;
				case align::middle:
					pos = std::max(static_cast<int>(page_.height() - h) / 2, 0);
					dest.second = static_cast<size_type>(pos);
					break;
				case align::bottom:
				default:
					dest.second = page_.margin().bottom();
					break;
				}
				
				return dest;
			}
		};
	}
}

#endif // FAML_PDF_IMAGE_H
