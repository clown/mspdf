/* ------------------------------------------------------------------------- */
/*
 *  contents/curved_up_arrow.h
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
#ifndef FAML_PDF_CONTENTS_CURVED_UP_ARROW_H
#define FAML_PDF_CONTENTS_CURVED_UP_ARROW_H

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
			//  basic_curved_up_arrow
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_curved_up_arrow : public basic_shape<CharT, Traits> {
			public:
				typedef basic_shape<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				typedef std::vector<double> container;
				
				explicit basic_curved_up_arrow(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h), adjust_() {
					adjust_.push_back(0.25);
					adjust_.push_back(0.5);
					adjust_.push_back(0.25);
				}
				
				virtual ~basic_curved_up_arrow() throw() {}
				
				virtual bool operator()(ostream_type& out) {
					// No need to draw and/or fill the shape.
					if (!this->is_stroke() && !this->is_fill()) return true;
					
					double ox = -this->width() / 2.0;
					double oy = -this->height() / 2.0;
					
					double head_w = (this->width() < this->height()) ? this->width() * adjust_.at(1) : this->height() * adjust_.at(1);
					double head_h = (this->width() < this->height()) ? this->width() * adjust_.at(2) : this->height() * adjust_.at(2);
					double eave_w = (this->width() < this->height()) ? (head_w - this->width() * adjust_.at(0)) / 2.0 : (head_w - this->height() * adjust_.at(0)) / 2.0;
					double stick_w = head_w - eave_w*2.0;
					double curve_l_w = (this->width() - head_w + eave_w)/2.0;
					double curve_l_h = this->height();
					double curve_r_w = (this->width() - head_w + eave_w)/2.0;
					double curve_r_h = this->height() - head_h;
					
					// Phase 1
					if (entag_) out << LITERAL("q") << std::endl;
					this->setorigin(out);
					this->setborder(out);
					this->setbackground(out);
					detail::draw_arc(out, ox+curve_r_w, oy+this->height()-head_h, curve_r_w, curve_r_h, 4);
					out << fmt(LITERAL("%f %f l")) % (ox+this->width()-head_w) % (oy+this->height()-head_h) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox+this->width()-head_w/2.0) % (oy+this->height()) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox+this->width()) % (oy+this->height()-head_h) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox+this->width()-eave_w) % (oy+this->height()-head_h) << std::endl;
					detail::draw_arc_rev(out, ox+curve_r_w+stick_w, oy+this->height()-head_h, curve_r_w, curve_r_h, 4, true);
					if (this->is_stroke() && this->is_fill()) out << LITERAL("b") << std::endl;
					else if (this->is_fill()) out << LITERAL("f") << std::endl;
					else out << LITERAL("s") << std::endl;
					if (entag_) out << LITERAL("Q") << std::endl;
					
					// Phase 2
					if (entag_) out << LITERAL("q") << std::endl;
					this->setorigin(out);
					this->setborder(out);
					double red = this->background().red() * this->highlight().red();
					double green = this->background().green() * this->highlight().green();
					double blue = this->background().blue() * this->highlight().blue();
					out << fmt(LITERAL("%f %f %f rg")) % red % green % blue << std::endl;
					detail::draw_arc(out, ox+curve_l_w, oy+this->height(), curve_l_w, curve_l_h, 3);
					out << fmt(LITERAL("%f %f l")) % (ox+curve_l_w+stick_w) % (oy) << std::endl;
					detail::draw_arc_rev(out, ox+curve_l_w+stick_w, oy+this->height(), curve_l_w, curve_l_h, 3, true);
					if (this->is_stroke() && this->is_fill()) out << LITERAL("b") << std::endl;
					else if (this->is_fill()) out << LITERAL("f") << std::endl;
					else out << LITERAL("s") << std::endl;
					if (entag_) out << LITERAL("Q") << std::endl;
					
					return true;
				}
				
				virtual size_type has_adjust() const { return 3; }
				virtual double adjust(size_type pos = 0) const { return adjust_.at(pos); }
				virtual void adjust(double value, size_type pos = 0) { adjust_.at(pos) = value; }
				
			private:
				typedef clx::basic_format<CharT, Traits> fmt;
				
				bool entag_;
				container adjust_;
			};
			typedef basic_curved_up_arrow<char> curved_up_arrow;
		}
	}
}

#endif // FAML_PDF_CONTENTS_CURVED_UP_ARROW_H
