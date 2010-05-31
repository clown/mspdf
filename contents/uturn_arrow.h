/* ------------------------------------------------------------------------- */
/*
 *  contents/uturn_arrow.h
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
#ifndef FAML_PDF_CONTENTS_UTURN_ARROW_H
#define FAML_PDF_CONTENTS_UTURN_ARROW_H

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
			//  basic_uturn_arrow
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_uturn_arrow : public basic_shape<CharT, Traits> {
			public:
				typedef basic_shape<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				typedef std::vector<double> container;
				
				explicit basic_uturn_arrow(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h), adjust_() {
					adjust_.push_back(0.25);
					adjust_.push_back(0.25);
					adjust_.push_back(0.25);
					adjust_.push_back(0.4375);
					adjust_.push_back(0.75);
				}
				
				virtual ~basic_uturn_arrow() throw() {}
				
				virtual bool operator()(ostream_type& out) {
					// No need to draw and/or fill the shape.
					if (!this->is_stroke() && !this->is_fill()) return true;
					
					double ox = -this->width() / 2.0;
					double oy = -this->height() / 2.0;
					
					double short_edge = (this->width() < this->height()) ? this->width() : this->height();
					double head_w = short_edge * adjust_.at(1) * 2.0;
					double head_h = short_edge * adjust_.at(2);
					double stick_w = short_edge * adjust_.at(0);
					if(head_w < stick_w){
						stick_w = head_w;
					}
					double top_to_eave = short_edge * adjust_.at(4) - head_h;
					if(short_edge * adjust_.at(4) < head_h + stick_w){
						top_to_eave = stick_w;
					}
					double eave_w = (head_w - stick_w)/2.0;
					double curve_r1 = short_edge * adjust_.at(3);
					if(curve_r1 > top_to_eave){
						curve_r1 = top_to_eave;
					}
					double curve_r2 = curve_r1 - stick_w;
					if(curve_r2<0) curve_r2 = 0;
					
					if (entag_) out << LITERAL("q") << std::endl;
					this->setorigin(out);
					this->setborder(out);
					this->setbackground(out);
					
					out << fmt(LITERAL("%f %f m")) % (ox) % (oy) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox) % (oy + this->height() - curve_r1) << std::endl;
					detail::draw_arc_rev(out, ox+curve_r1, oy+this->height()-curve_r1, curve_r1, curve_r1, 2, true);
					out << fmt(LITERAL("%f %f l")) % (ox+this->width()-eave_w-curve_r1) % (oy + this->height()) << std::endl;
					detail::draw_arc_rev(out, ox+this->width()-eave_w-curve_r1, oy+this->height()-curve_r1, curve_r1, curve_r1, 1, true);
					out << fmt(LITERAL("%f %f l")) % (ox+this->width()-eave_w) % (oy + this->height() - top_to_eave) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox+this->width()) % (oy + this->height() - top_to_eave) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox+this->width()-head_w/2.0) % (oy + this->height() - top_to_eave - head_h) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox+this->width()-head_w) % (oy + this->height() - top_to_eave) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox+this->width()-head_w+eave_w) % (oy + this->height() - top_to_eave) << std::endl;
					if(curve_r1 > stick_w){
						out << fmt(LITERAL("%f %f l")) % (ox+this->width()-head_w+eave_w) % (oy + this->height() - curve_r1) << std::endl;
						detail::draw_arc(out, ox+this->width()-eave_w-curve_r1, oy+this->height()-curve_r1, curve_r2, curve_r2, 1, true);
						out << fmt(LITERAL("%f %f l")) % (ox+curve_r1) % (oy + this->height() - curve_r1 + curve_r2) << std::endl;
						detail::draw_arc(out, ox+curve_r1, oy+this->height()-curve_r1, curve_r2, curve_r2, 2, true);
					}else{
						out << fmt(LITERAL("%f %f l")) % (ox+this->width()-head_w+eave_w) % (oy + this->height() - stick_w) << std::endl;
						detail::draw_arc(out, ox+this->width()-eave_w-stick_w, oy+this->height()-stick_w, curve_r2, curve_r2, 1, true);
						out << fmt(LITERAL("%f %f l")) % (ox+stick_w) % (oy + this->height() - stick_w + curve_r2) << std::endl;
						detail::draw_arc(out, ox+stick_w, oy+this->height()-stick_w, curve_r2, curve_r2, 2, true);
					}
					out << fmt(LITERAL("%f %f l")) % (ox+stick_w) % (oy) << std::endl;
					
					if (this->is_stroke() && this->is_fill()) out << LITERAL("b") << std::endl;
					else if (this->is_fill()) out << LITERAL("f") << std::endl;
					else out << LITERAL("s") << std::endl;
					
					if (entag_) out << LITERAL("Q") << std::endl;
					
					return true;
				}
				
				virtual size_type has_adjust() const { return 4; }
				virtual double adjust(size_type pos = 0) const { return adjust_.at(pos); }
				virtual void adjust(double value, size_type pos = 0) { adjust_.at(pos) = value; }
				
			private:
				typedef clx::basic_format<CharT, Traits> fmt;
				
				bool entag_;
				container adjust_;
			};
			typedef basic_uturn_arrow<char> uturn_arrow;
		}
	}
}

#endif // FAML_PDF_CONTENTS_UTURN_ARROW_H
