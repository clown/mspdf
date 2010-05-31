/* ------------------------------------------------------------------------- */
/*
 *  contents/sun.h
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
#ifndef FAML_PDF_CONTENTS_SUN_H
#define FAML_PDF_CONTENTS_SUN_H

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
			//  basic_sun
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_sun : public basic_polygon<CharT, Traits> {
			public:
				typedef basic_polygon<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				
				explicit basic_sun(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h), entag_(entag), adjust_(sh(5400)) {}
				
				virtual ~basic_sun() throw() {}
				
				virtual bool operator()(ostream_type& out) {
					// No need to draw and/or fill the shape.
					if (!this->is_stroke() && !this->is_fill()) return true;
					
					double ox = -this->width() / 2.0;
					double oy = -this->height() / 2.0;
					double x  =  this->width() / 2.0;
					double y  =  this->height() / 2.0;
					
					// Phase 1
					if (entag_) out << LITERAL("q") << std::endl;
					this->setorigin(out);
					this->setborder(out);
					this->setbackground(out);
					double w  =  this->width() * (0.5 - adjust_);
					double h  =  this->height() * (0.5 - adjust_);
					detail::draw_arc(out, ox+x, oy+y, w, h, 1);
					detail::draw_arc(out, ox+x, oy+y, w, h, 2, true);
					detail::draw_arc(out, ox+x, oy+y, w, h, 3, true);
					detail::draw_arc(out, ox+x, oy+y, w, h, 4, true);
					if (this->is_stroke() && this->is_fill()) out << LITERAL("b") << std::endl;
					else if (this->is_fill()) out << LITERAL("f") << std::endl;
					else out << LITERAL("s") << std::endl;
					if (entag_) out << LITERAL("Q") << std::endl;
					
					// 1000 * 1000
					double rate = (1000*adjust_*0.78)/500.0;
					double xn0 = 0;
					double yn0 = 500;
					double xn1 = -(500 - 1000*adjust_)*(2.0/3.0)*rate;
					double yn1 = 500 - 1000*adjust_*0.78;
					double xn2 = -xn1;
					double yn2 = yn1;
					
					double root2 = 1/std::sqrt(2.0);
					double xnw0 = xn0*root2 - yn0*root2;
					double ynw0 = xn0*root2 + yn0*root2;
					double xnw1 = xn1*root2 - yn1*root2;
					double ynw1 = xn1*root2 + yn1*root2;
					double xnw2 = xn2*root2 - yn2*root2;
					double ynw2 = xn2*root2 + yn2*root2;
					
					double xw0 = xnw0*root2 - ynw0*root2;
					double yw0 = xnw0*root2 + ynw0*root2;
					double xw1 = xnw1*root2 - ynw1*root2;
					double yw1 = xnw1*root2 + ynw1*root2;
					double xw2 = xnw2*root2 - ynw2*root2;
					double yw2 = xnw2*root2 + ynw2*root2;
					
					double xsw0 = xw0*root2 - yw0*root2;
					double ysw0 = xw0*root2 + yw0*root2;
					double xsw1 = xw1*root2 - yw1*root2;
					double ysw1 = xw1*root2 + yw1*root2;
					double xsw2 = xw2*root2 - yw2*root2;
					double ysw2 = xw2*root2 + yw2*root2;
					
					double xs0 = xsw0*root2 - ysw0*root2;
					double ys0 = xsw0*root2 + ysw0*root2;
					double xs1 = xsw1*root2 - ysw1*root2;
					double ys1 = xsw1*root2 + ysw1*root2;
					double xs2 = xsw2*root2 - ysw2*root2;
					double ys2 = xsw2*root2 + ysw2*root2;
					
					double xne0 =  xn0*root2 + yn0*root2;
					double yne0 = -xn0*root2 + yn0*root2;
					double xne1 =  xn1*root2 + yn1*root2;
					double yne1 = -xn1*root2 + yn1*root2;
					double xne2 =  xn2*root2 + yn2*root2;
					double yne2 = -xn2*root2 + yn2*root2;
					
					double xe0 =  xne0*root2 + yne0*root2;
					double ye0 = -xne0*root2 + yne0*root2;
					double xe1 =  xne1*root2 + yne1*root2;
					double ye1 = -xne1*root2 + yne1*root2;
					double xe2 =  xne2*root2 + yne2*root2;
					double ye2 = -xne2*root2 + yne2*root2;
					
					double xse0 =  xe0*root2 + ye0*root2;
					double yse0 = -xe0*root2 + ye0*root2;
					double xse1 =  xe1*root2 + ye1*root2;
					double yse1 = -xe1*root2 + ye1*root2;
					double xse2 =  xe2*root2 + ye2*root2;
					double yse2 = -xe2*root2 + ye2*root2;
					
					// Phase 2 Triangle North
					if (entag_) out << LITERAL("q") << std::endl;
					this->setorigin(out);
					this->setborder(out);
					this->setbackground(out);
					out << fmt(LITERAL("%f %f m")) % (xn0*this->width()/1000.0) % (yn0*this->height()/1000.0) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (xn1*this->width()/1000.0) % (yn1*this->height()/1000.0) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (xn2*this->width()/1000.0) % (yn2*this->height()/1000.0) << std::endl;
					if (this->is_stroke() && this->is_fill()) out << LITERAL("b") << std::endl;
					else if (this->is_fill()) out << LITERAL("f") << std::endl;
					else out << LITERAL("s") << std::endl;
					if (entag_) out << LITERAL("Q") << std::endl;
					
					// Phase 3 Triangle NorthWest
					if (entag_) out << LITERAL("q") << std::endl;
					this->setorigin(out);
					this->setborder(out);
					this->setbackground(out);
					out << fmt(LITERAL("%f %f m")) % (xnw0*this->width()/1000.0) % (ynw0*this->height()/1000.0) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (xnw1*this->width()/1000.0) % (ynw1*this->height()/1000.0) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (xnw2*this->width()/1000.0) % (ynw2*this->height()/1000.0) << std::endl;
					if (this->is_stroke() && this->is_fill()) out << LITERAL("b") << std::endl;
					else if (this->is_fill()) out << LITERAL("f") << std::endl;
					else out << LITERAL("s") << std::endl;
					if (entag_) out << LITERAL("Q") << std::endl;
					
					// Phase 4 Triangle West
					if (entag_) out << LITERAL("q") << std::endl;
					this->setorigin(out);
					this->setborder(out);
					this->setbackground(out);
					out << fmt(LITERAL("%f %f m")) % (xw0*this->width()/1000.0) % (yw0*this->height()/1000.0) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (xw1*this->width()/1000.0) % (yw1*this->height()/1000.0) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (xw2*this->width()/1000.0) % (yw2*this->height()/1000.0) << std::endl;
					if (this->is_stroke() && this->is_fill()) out << LITERAL("b") << std::endl;
					else if (this->is_fill()) out << LITERAL("f") << std::endl;
					else out << LITERAL("s") << std::endl;
					if (entag_) out << LITERAL("Q") << std::endl;
					
					// Phase 5 Triangle SouthWest
					if (entag_) out << LITERAL("q") << std::endl;
					this->setorigin(out);
					this->setborder(out);
					this->setbackground(out);
					out << fmt(LITERAL("%f %f m")) % (xsw0*this->width()/1000.0) % (ysw0*this->height()/1000.0) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (xsw1*this->width()/1000.0) % (ysw1*this->height()/1000.0) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (xsw2*this->width()/1000.0) % (ysw2*this->height()/1000.0) << std::endl;
					if (this->is_stroke() && this->is_fill()) out << LITERAL("b") << std::endl;
					else if (this->is_fill()) out << LITERAL("f") << std::endl;
					else out << LITERAL("s") << std::endl;
					if (entag_) out << LITERAL("Q") << std::endl;
					
					// Phase 6 Triangle South
					if (entag_) out << LITERAL("q") << std::endl;
					this->setorigin(out);
					this->setborder(out);
					this->setbackground(out);
					out << fmt(LITERAL("%f %f m")) % (xs0*this->width()/1000.0) % (ys0*this->height()/1000.0) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (xs1*this->width()/1000.0) % (ys1*this->height()/1000.0) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (xs2*this->width()/1000.0) % (ys2*this->height()/1000.0) << std::endl;
					if (this->is_stroke() && this->is_fill()) out << LITERAL("b") << std::endl;
					else if (this->is_fill()) out << LITERAL("f") << std::endl;
					else out << LITERAL("s") << std::endl;
					if (entag_) out << LITERAL("Q") << std::endl;
					
					// Phase 7 Triangle SouthEast
					if (entag_) out << LITERAL("q") << std::endl;
					this->setorigin(out);
					this->setborder(out);
					this->setbackground(out);
					out << fmt(LITERAL("%f %f m")) % (xse0*this->width()/1000.0) % (yse0*this->height()/1000.0) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (xse1*this->width()/1000.0) % (yse1*this->height()/1000.0) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (xse2*this->width()/1000.0) % (yse2*this->height()/1000.0) << std::endl;
					if (this->is_stroke() && this->is_fill()) out << LITERAL("b") << std::endl;
					else if (this->is_fill()) out << LITERAL("f") << std::endl;
					else out << LITERAL("s") << std::endl;
					if (entag_) out << LITERAL("Q") << std::endl;
					
					// Phase 8 Triangle East
					if (entag_) out << LITERAL("q") << std::endl;
					this->setorigin(out);
					this->setborder(out);
					this->setbackground(out);
					out << fmt(LITERAL("%f %f m")) % (xe0*this->width()/1000.0) % (ye0*this->height()/1000.0) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (xe1*this->width()/1000.0) % (ye1*this->height()/1000.0) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (xe2*this->width()/1000.0) % (ye2*this->height()/1000.0) << std::endl;
					if (this->is_stroke() && this->is_fill()) out << LITERAL("b") << std::endl;
					else if (this->is_fill()) out << LITERAL("f") << std::endl;
					else out << LITERAL("s") << std::endl;
					if (entag_) out << LITERAL("Q") << std::endl;
					
					// Phase 9 Triangle NouthEast
					if (entag_) out << LITERAL("q") << std::endl;
					this->setorigin(out);
					this->setborder(out);
					this->setbackground(out);
					out << fmt(LITERAL("%f %f m")) % (xne0*this->width()/1000.0) % (yne0*this->height()/1000.0) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (xne1*this->width()/1000.0) % (yne1*this->height()/1000.0) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (xne2*this->width()/1000.0) % (yne2*this->height()/1000.0) << std::endl;
					if (this->is_stroke() && this->is_fill()) out << LITERAL("b") << std::endl;
					else if (this->is_fill()) out << LITERAL("f") << std::endl;
					else out << LITERAL("s") << std::endl;
					if (entag_) out << LITERAL("Q") << std::endl;
					
					return true;
				}
				
				virtual size_type has_adjust() const { return 1; }
				virtual double adjust(size_type pos = 0) const { return adjust_; }
				virtual void adjust(double value, size_type pos = 0) { adjust_ = value; }
				
			private:
				typedef clx::basic_format<CharT, Traits> fmt;
				
				bool entag_;
				double adjust_;
			};
			typedef basic_sun<char> sun;
		}
	}
}

#endif // FAML_PDF_CONTENTS_SUN_H
