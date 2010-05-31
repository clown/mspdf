/* ------------------------------------------------------------------------- */
/*
 *  contents/line.h
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
 *  Last-modified: Fri 05 Jun 2009 00:09:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PDF_CONTENTS_LINE_H
#define FAML_PDF_CONTENTS_LINE_H

#ifndef FAML_PDF_CONTENTS_SHAPE_H
#include "shape.h"
#endif // FAML_PDF_CONTENTS_SHAPE_H

#include <cassert>
#include "clx/format.h"
#include "clx/literal.h"
#include "../unit.h"

namespace faml {
	namespace pdf {
		namespace contents {
			/* ------------------------------------------------------------- */
			//  basic_line
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_line : public basic_shape<CharT, Traits> {
			public:
				typedef basic_shape<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				
				explicit basic_line(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h), entag_(entag), opt_(0) {}
				
				virtual ~basic_line() throw() {}
				
				/* --------------------------------------------------------- */
				//  main operation.
				/* --------------------------------------------------------- */
				bool operator()(ostream_type& out) {
					// No need to draw and/or fill the shape.
					if (!this->is_stroke()) return true;
					
					if (entag_) out << LITERAL("q") << std::endl;
					
					this->setorigin(out);
					
					// effects.
					typename super::effect_map::const_iterator pos;
					pos = this->effects().find(LITERAL("shadow"));
					if (pos != this->effects().end()) {
						this->xput_effect(out, pos->second);
					}
					
					double ox = -this->width() / 2.0;
					double oy = this->height() / 2.0;
					this->setborder(out);
					this->background(this->border());
					this->setbackground(out);
					this->put_shape(out, ox, oy);
					
					if (entag_) out << LITERAL("Q") << std::endl;
					
					return true;
				}
				
				void option(size_type cp) { opt_ = cp; }
				
			private:
				typedef clx::basic_format<CharT, Traits> fmt;
				
				bool entag_;
				size_type opt_;
				
				/* --------------------------------------------------------- */
				//  put_shape
				/* --------------------------------------------------------- */
				bool put_shape(ostream_type& out, double ox, double oy) {
					double w = this->width();
					double h = -this->height();
					double arrow = std::max(this->weight() * 2.0, 2.0);
					
					double l = ((opt_ & 0x01) && (static_cast<int>(this->height()) <= 0)) ? arrow : 0;
					if (w < 0.0) l = -l;
					double r = ((opt_ & 0x02) && (static_cast<int>(this->height()) <= 0)) ? arrow : 0;
					if (w < 0.0) r = -r;
					double t = ((opt_ & 0x01) && (static_cast<int>(this->width()) <= 0)) ? arrow : 0;
					if (h < 0.0) t = -t;
					double b = ((opt_ & 0x02) && (static_cast<int>(this->width()) <= 0)) ? arrow : 0;
					if (h < 0.0) b = -b;
					
					out << LITERAL("0 J") << std::endl;
					out << fmt(LITERAL("%f %f m")) % (ox + l) % (oy + t) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox + w - r) % (oy + h - b) << std::endl;
					out << LITERAL("S") << std::endl;
					
					if ((opt_ & 0x01)) this->put_arrow(out, ox + l, oy + t, arrow);
					if ((opt_ & 0x02)) this->put_arrow(out, ox + w - r, oy + h - b, arrow);
					
					return true;
				}
				
				/* --------------------------------------------------------- */
				//  xput_effect
				/* --------------------------------------------------------- */
				template <class Type>
				bool xput_effect(ostream_type& out, const Type& src) {
					if (src.fills().empty()) return true;
					
					double angle = src.direction() + clx::degree(this->angle());
					if (angle < 0.0) angle += 360.0;
					else if (angle > 360.0) angle -= 360.0;
					int h = (this->flip() & 0x01) ? -1 : 1;
					int v = (this->flip() & 0x02) ? -1 : 1;
					double ox = -this->width() / 2.0 + (std::cos(clx::radian(angle)) * src.distance() * h);
					double oy = this->height() / 2.0 - (std::sin(clx::radian(angle)) * src.distance() * v);
					
					out << fmt(LITERAL("%f w")) % this->weight() << std::endl;
					out << fmt(LITERAL("%f %f %f rg"))
						% src.fill().red() % src.fill().green() % src.fill().blue() << std::endl;
					out << fmt(LITERAL("%f %f %f RG"))
						% src.fill().red() % src.fill().green() % src.fill().blue() << std::endl;
					this->put_shape(out, ox, oy);
					
					return true;
				}
				
				/* --------------------------------------------------------- */
				//  put_arrow
				/* --------------------------------------------------------- */
				bool put_arrow(ostream_type& out, double x, double y, double r) {
					double x0 = x;
					double y0 = y;
					
					double a = (x0 == 0) ? 0 : y0 / x0;
					double dist = std::sqrt(r * r / (a * a + 1));
					
					double x1 = (x0 == 0) ? 0 : ((x0 > 0) ? x0 + dist : x0 - dist);
					double y1 = (x1 != 0) ? a * x1 : ((y0 > 0) ? y0 + dist : y0 - dist);
					
					double x2 = -((x1 - x0) + std::sqrt(3) * (y1 - y0)) / 2.0 + x0;
					double y2 =  (std::sqrt(3) * (x1 - x0) - (y1 - y0)) / 2.0 + y0;
					
					double x3 = -((x1 - x0) - std::sqrt(3) * (y1 - y0)) / 2.0 + x0;
					double y3 = -(std::sqrt(3) * (x1 - x0) + (y1 - y0)) / 2.0 + y0;
					
					out << fmt(LITERAL("%f %f m")) % x1 % y1 << std::endl;
					out << fmt(LITERAL("%f %f l")) % x2 % y2 << std::endl;
					out << fmt(LITERAL("%f %f l")) % x3 % y3 << std::endl;
					out << LITERAL("f") << std::endl;
					
					return true;
				}
			};
		}
	}
}

#endif // FAML_PDF_CONTENTS_LINE_H
