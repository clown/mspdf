/* ------------------------------------------------------------------------- */
/*
 *  contents/ellipse_ribbon2.h
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
#ifndef FAML_PDF_CONTENTS_ELLIPSE_RIBBON2_H
#define FAML_PDF_CONTENTS_ELLIPSE_RIBBON2_H

#ifndef FAML_PDF_CONTENTS_SHAPE_H
#include "shape.h"
#endif // FAML_PDF_CONTENTS_SHAPE_H

#include <cassert>
#include <vector>
#include "clx/format.h"
#include "clx/literal.h"
#include "circle.h"
#include "../unit.h"

namespace faml {
	namespace pdf {
		namespace contents {
			/* ------------------------------------------------------------- */
			//  basic_ellipse_ribbon2
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_ellipse_ribbon2 : public basic_shape<CharT, Traits> {
			public:
				typedef basic_shape<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				typedef std::vector<double> container;
				
				explicit basic_ellipse_ribbon2(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h), entag_(entag), adjust_() {
					adjust_.push_back(0.25);
					adjust_.push_back(0.5);
					adjust_.push_back(0.125);
				}
				
				virtual ~basic_ellipse_ribbon2() throw() {}
				
				virtual bool operator()(ostream_type& out) {
					// No need to draw and/or fill the shape.
					if (!this->is_stroke() && !this->is_fill() && !this->is_shading()) return true;
					
					if (entag_) out << LITERAL("q") << std::endl;
					
					this->setorigin(out);
					this->setborder(out);
					this->setbackground(out);
					
					double ox = -this->width() / 2.0;
					double oy = -this->height() / 2.0;
					double delta1 = this->height() * adjust_.at(0);
					double delta2 = this->width() * adjust_.at(1);
					double delta3 = this->height() * adjust_.at(2);
					double fold_len = this->width() * 0.125;
					
					double x1 = ox;
					double y1 = oy + this->height() - delta3;
					double x2 = ox + this->width() * (1.0/3.0);
					double y2 = oy + this->height() + delta3 * (1.0/3.0);
					double x3 = ox + this->width() * (2.0/3.0);
					double y3 = oy + this->height() + delta3 * (1.0/3.0);
					double x4 = ox + this->width();
					double y4 = oy + this->height() - delta3;
					double t = 0.5;
					if(oy + delta1 > (1-t)*(1-t)*(1-t)*y1 + 3*(1-t)*(1-t)*t*y2 + 3*(1-t)*t*t*y3 + t*t*t*y4 + delta3 - (this->height() + delta1)/2.0){
						delta3 = this->height() - (this->height() - delta1) * 3.0 / 2.0;
						y1 = oy + this->height() - delta3;
						y2 = oy + this->height() + delta3 * (1.0/8.0);
						y3 = oy + this->height() + delta3 * (1.0/8.0);
						y4 = oy + this->height() - delta3;
					}
					t = 0.125;
					double fold_Y = (1-t)*(1-t)*(1-t)*y1 + 3*(1-t)*(1-t)*t*y2 + 3*(1-t)*t*t*y3 + t*t*t*y4 + delta3 - (this->height() + delta1)/2.0;
					
					// phase1
					t = (this->width()/2.0 - delta2/2.0 + fold_len) / this->width();
					double X1 = (1-t)*(1-t)*(1-t)*x1 + 3*(1-t)*(1-t)*t*x2 + 3*(1-t)*t*t*x3 + t*t*t*x4;
					double Y1 = (1-t)*(1-t)*(1-t)*y1 + 3*(1-t)*(1-t)*t*y2 + 3*(1-t)*t*t*y3 + t*t*t*y4;
					out << fmt(LITERAL("%f %f m")) % (ox) % (oy + this->height() - delta1) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (x2) % (y2 + delta3 - delta1)
							% (X1) % (Y1 + delta3 - delta1)
							% (X1) % (Y1 + delta3 - delta1) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (X1) % (Y1 + delta3 - this->height()) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (X1) % (Y1 + delta3 - this->height())
							% (x2) % (y2 + delta3 - this->height())
							% (x1) % (y1 + delta3 - this->height()) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox + fold_len) % (fold_Y) << std::endl;
					if (this->is_stroke() && this->is_fill()) out << LITERAL("b") << std::endl;
					else if (this->is_fill()) out << LITERAL("f") << std::endl;
					else out << LITERAL("s") << std::endl;
					
					// phase2
					t = (this->width()/2.0 + delta2/2.0 - fold_len) / this->width();
					double X2 = (1-t)*(1-t)*(1-t)*x1 + 3*(1-t)*(1-t)*t*x2 + 3*(1-t)*t*t*x3 + t*t*t*x4;
					double Y2 = (1-t)*(1-t)*(1-t)*y1 + 3*(1-t)*(1-t)*t*y2 + 3*(1-t)*t*t*y3 + t*t*t*y4;
					out << fmt(LITERAL("%f %f m")) % (ox + this->width()) % (oy + this->height() - delta1) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (x3) % (y3 + delta3 - delta1)
							% (X2) % (Y2 + delta3 - delta1)
							% (X2) % (Y2 + delta3 - delta1) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (X2) % (Y2 + delta3 - this->height()) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (X2) % (Y2 + delta3 - this->height())
							% (x3) % (y3 + delta3 - this->height())
							% (x4) % (y4 + delta3 - this->height()) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox + this->width() - fold_len) % (fold_Y) << std::endl;
					if (this->is_stroke() && this->is_fill()) out << LITERAL("b") << std::endl;
					else if (this->is_fill()) out << LITERAL("f") << std::endl;
					else out << LITERAL("s") << std::endl;
					
					double red = this->background().red() * this->shadow().red();
					double green = this->background().green() * this->shadow().green();
					double blue = this->background().blue() * this->shadow().blue();
					
					// phase3
					t = (this->width()/2.0 - delta2/2.0) / this->width();
					double X3 = (1-t)*(1-t)*(1-t)*x1 + 3*(1-t)*(1-t)*t*x2 + 3*(1-t)*t*t*x3 + t*t*t*x4;
					double Y3 = (1-t)*(1-t)*(1-t)*y1 + 3*(1-t)*(1-t)*t*y2 + 3*(1-t)*t*t*y3 + t*t*t*y4;
					out << fmt(LITERAL("%f %f %f rg")) % red % green % blue << std::endl;
					out << fmt(LITERAL("%f %f m")) % (X1) % (Y1 + delta3 - delta1) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (X1) % (Y1 + delta3 - this->height()) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (X3) % (Y3 - this->height() + delta1) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (X3) % (Y3) << std::endl;
					if (this->is_stroke() && this->is_fill()) out << LITERAL("b") << std::endl;
					else if (this->is_fill()) out << LITERAL("f") << std::endl;
					else out << LITERAL("s") << std::endl;
					
					// phase4
					t = (this->width()/2.0 + delta2/2.0) / this->width();
					double X4 = (1-t)*(1-t)*(1-t)*x1 + 3*(1-t)*(1-t)*t*x2 + 3*(1-t)*t*t*x3 + t*t*t*x4;
					double Y4 = (1-t)*(1-t)*(1-t)*y1 + 3*(1-t)*(1-t)*t*y2 + 3*(1-t)*t*t*y3 + t*t*t*y4;
					out << fmt(LITERAL("%f %f %f rg")) % red % green % blue << std::endl;
					out << fmt(LITERAL("%f %f m")) % (X2) % (Y2 + delta3 - delta1) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (X2) % (Y2 + delta3 - this->height()) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (X4) % (Y4 - this->height() + delta1) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (X4) % (Y4) << std::endl;
					if (this->is_stroke() && this->is_fill()) out << LITERAL("b") << std::endl;
					else if (this->is_fill()) out << LITERAL("f") << std::endl;
					else out << LITERAL("s") << std::endl;
					
					// phase5
					out << fmt(LITERAL("%f %f %f rg")) % this->background().red() % this->background().green() % this->background().blue() << std::endl;
					out << fmt(LITERAL("%f %f m")) % (X3) % (Y3) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (x2) % (y2)
							% (x3) % (y3)
							% (X4) % (Y4) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (X4) % (Y4 - this->height() + delta1) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (x3) % (y2 - this->height() + delta1)
							% (x2) % (y3 - this->height() + delta1)
							% (X3) % (Y3 - this->height() + delta1) << std::endl;
					if (this->is_stroke() && this->is_fill()) out << LITERAL("b") << std::endl;
					else if (this->is_fill()) out << LITERAL("f") << std::endl;
					else out << LITERAL("s") << std::endl;
					
					if (entag_) out << LITERAL("Q") << std::endl;
					
					return true;
				}
				
				virtual size_type has_adjust() const { return 2; }
				virtual double adjust(size_type pos = 0) const { return adjust_.at(pos); }
				virtual void adjust(double value, size_type pos = 0) { adjust_.at(pos) = value; }
				
			private:
				typedef clx::basic_format<CharT, Traits> fmt;
				
				bool entag_;
				container adjust_;
			};
			typedef basic_ellipse_ribbon2<char> ellipse_ribbon2;
		}
	}
}

#endif // FAML_PDF_CONTENTS_ELLIPSE_RIBBON2_H
