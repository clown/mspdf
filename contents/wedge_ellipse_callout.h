/* ------------------------------------------------------------------------- */
/*
 *  contents/wedge_ellipse_callout.h
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
#ifndef FAML_PDF_CONTENTS_WEDGE_ELLIPSE_CALLOUT_H
#define FAML_PDF_CONTENTS_WEDGE_ELLIPSE_CALLOUT_H

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
			//  basic_wedge_ellipse_callout
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_wedge_ellipse_callout : public basic_shape<CharT, Traits> {
			public:
				typedef basic_shape<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				typedef std::vector<double> container;
				
				explicit basic_wedge_ellipse_callout(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h), entag_(entag), adjust_() {
					adjust_.push_back(-0.4375);
					adjust_.push_back(0.7);
				}
				
				virtual ~basic_wedge_ellipse_callout() throw() {}
				
				virtual bool operator()(ostream_type& out) {
					// No need to draw and/or fill the shape.
					if (!this->is_stroke() && !this->is_fill()) return true;
					
					adjust_.at(0) += 0.5;
					adjust_.at(1) += 0.5;
					double ox = -this->width() / 2.0;
					double oy = -this->height() / 2.0;
					double x = adjust_.at(0);
					double y = 1-adjust_.at(1);
					
					static const double PI = 3.1415926535;
					double center_rad = atan2(oy+this->height()*y, ox+this->width()*x)*180/PI;
					double start_rad;
					double end_rad;
					if((y<0.5 && x<0.5) || (y>0.5 && x>0.5)){
						start_rad = center_rad + 20;
						end_rad = center_rad;
					}else{
						start_rad = center_rad;
						end_rad = center_rad - 20;
					}
					
					if (entag_) out << LITERAL("q") << std::endl;
					this->setorigin(out);
					this->setborder(out);
					this->setbackground(out);
					
					detail::draw_val_arc(out, ox + this->width() / 2.0, oy + this->height() / 2.0, this->width() / 2.0, this->height() / 2.0, start_rad, end_rad);
					out << fmt(LITERAL("%f %f l")) % (ox + this->width() * x) % (oy + this->height() * y) << std::endl;
					
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
			typedef basic_wedge_ellipse_callout<char> wedge_ellipse_callout;
		}
	}
}

#endif // FAML_PDF_CONTENTS_WEDGE_ELLIPSE_CALLOUT_H
