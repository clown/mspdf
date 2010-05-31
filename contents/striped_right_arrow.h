/* ------------------------------------------------------------------------- */
/*
 *  contents/striped_right_arrow.h
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
#ifndef FAML_PDF_CONTENTS_STRIPED_RIGHT_ARROW_H
#define FAML_PDF_CONTENTS_STRIPED_RIGHT_ARROW_H

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
			//  basic_striped_right_arrow
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_striped_right_arrow : public basic_polygon<CharT, Traits> {
			public:
				typedef basic_polygon<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				typedef std::vector<double> container;
				
				explicit basic_striped_right_arrow(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h), entag_(entag), adjust_() {
					adjust_.push_back(0.5);
					adjust_.push_back(0.5);
				}
				
				virtual ~basic_striped_right_arrow() throw() {}
				
				virtual bool operator()(ostream_type& out) {
					// No need to draw and/or fill the shape.
					if (!this->is_stroke() && !this->is_fill()) return true;
					
					double ox = -this->width() / 2.0;
					double oy = -this->height() / 2.0;
					
					double short_edge = (this->width() < this->height()) ? this->width() : this->height();
					double head = short_edge * adjust_.at(1);
					if(head > this->width()){
						head = this->width();
					}
					
					// Phase 1
					if (entag_) out << LITERAL("q") << std::endl;
					this->setorigin(out);
					this->setborder(out);
					this->setbackground(out);
					out << fmt(LITERAL("%f %f m")) % (ox) % (oy + this->height() * (0.5 - 0.5 * adjust_.at(0))) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox + this->width() / 60.0) % (oy + this->height() * (0.5 - 0.5 * adjust_.at(0))) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox + this->width() / 60.0) % (oy + this->height() * (0.5 + 0.5 * adjust_.at(0))) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox) % (oy + this->height() * (0.5 + 0.5 * adjust_.at(0))) << std::endl;
					if (this->is_stroke() && this->is_fill()) out << LITERAL("b") << std::endl;
					else if (this->is_fill()) out << LITERAL("f") << std::endl;
					else out << LITERAL("s") << std::endl;
					if (entag_) out << LITERAL("Q") << std::endl;
					
					// Phase 2
					if (entag_) out << LITERAL("q") << std::endl;
					this->setorigin(out);
					this->setborder(out);
					this->setbackground(out);
					out << fmt(LITERAL("%f %f m")) % (ox + this->width() / 30.0) % (oy + this->height() * (0.5 - 0.5 * adjust_.at(0))) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox + this->width() * 2.0 / 30.0) % (oy + this->height() * (0.5 - 0.5 * adjust_.at(0))) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox + this->width() * 2.0 / 30.0) % (oy + this->height() * (0.5 + 0.5 * adjust_.at(0))) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox + this->width() / 30.0) % (oy + this->height() * (0.5 + 0.5 * adjust_.at(0))) << std::endl;
					if (this->is_stroke() && this->is_fill()) out << LITERAL("b") << std::endl;
					else if (this->is_fill()) out << LITERAL("f") << std::endl;
					else out << LITERAL("s") << std::endl;
					if (entag_) out << LITERAL("Q") << std::endl;
					
					// Phase 3
					if (entag_) out << LITERAL("q") << std::endl;
					this->setorigin(out);
					this->setborder(out);
					this->setbackground(out);
					out << fmt(LITERAL("%f %f m")) % (ox + this->width() / 12.0) % (oy + this->height() * (0.5 + 0.5 * adjust_.at(0))) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox + this->width() - head) % (oy + this->height() * (0.5 + 0.5 * adjust_.at(0))) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox + this->width() - head) % (oy + this->height()) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox + this->width()) % (oy + this->height() / 2.0) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox + this->width() - head) % (oy) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox + this->width() - head) % (oy + this->height() * (0.5 - 0.5 * adjust_.at(0))) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox + this->width() / 12.0) % (oy + this->height() * (0.5 - 0.5 * adjust_.at(0))) << std::endl;
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
			typedef basic_striped_right_arrow<char> striped_right_arrow;
		}
	}
}

#endif // FAML_PDF_CONTENTS_STRIPED_RIGHT_ARROW_H
