/* ------------------------------------------------------------------------- */
/*
 *  contents/up_arrow_callout.h
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
#ifndef FAML_PDF_CONTENTS_UP_ARROW_CALLOUT_H
#define FAML_PDF_CONTENTS_UP_ARROW_CALLOUT_H

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
			//  basic_up_arrow_callout
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_up_arrow_callout : public basic_polygon<CharT, Traits> {
			public:
				typedef basic_polygon<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				typedef std::vector<double> container;
				
				explicit basic_up_arrow_callout(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h, entag), adjust_() {
					adjust_.push_back(0.25);
					adjust_.push_back(0.25);
					adjust_.push_back(0.25);
					adjust_.push_back(0.64977);
				}
				
				virtual ~basic_up_arrow_callout() throw() {}
				
				virtual bool operator()(ostream_type& out) {
					// No need to draw and/or fill the shape.
					if (!this->is_stroke() && !this->is_fill()) return true;
					
					double ox = -this->width() / 2.0;
					double oy = -this->height() / 2.0;
					
					double head_w = (this->width() < this->height()) ? 2.0 * this->width() * adjust_.at(1) : 2.0 * this->height() * adjust_.at(1);
					double head_h = (this->width() < this->height()) ? this->width() * adjust_.at(2) : this->height() * adjust_.at(2);
					double eave_w = (this->width() < this->height()) ? (head_w - this->width() * adjust_.at(0)) / 2.0 : (head_w - this->height() * adjust_.at(0)) / 2.0;
					if(eave_w < 0) eave_w = 0;
					double reg_h = this->height() * (1 - adjust_.at(3)) - head_h;
					if(reg_h < 0) reg_h = 0;
					double reg_w = head_w - 2.0*eave_w;
					
					this->add(coordinate(ox + this->width(), oy));
					this->add(coordinate(ox + this->width(), oy + this->height() - reg_h - head_h));
					this->add(coordinate(ox + this->width()/2.0 + reg_w/2.0, oy + this->height() - reg_h - head_h));
					this->add(coordinate(ox + this->width()/2.0 + reg_w/2.0, oy + this->height() -  head_h));
					this->add(coordinate(ox + this->width()/2.0 + head_w/2.0, oy + this->height() - head_h));
					this->add(coordinate(ox + this->width()/2.0, oy + this->height()));
					this->add(coordinate(ox + this->width()/2.0 - head_w/2.0, oy + this->height() - head_h));
					this->add(coordinate(ox + this->width()/2.0 - reg_w/2.0, oy + this->height() - head_h));
					this->add(coordinate(ox + this->width()/2.0 - reg_w/2.0, oy + this->height() - reg_h - head_h));
					this->add(coordinate(ox, oy + this->height() - reg_h - head_h));
					this->add(coordinate(ox, oy));
					
					return super::operator()(out);
				}
				
				virtual size_type has_adjust() const { return 4; }
				virtual double adjust(size_type pos = 0) const { return adjust_.at(pos); }
				virtual void adjust(double value, size_type pos = 0) { adjust_.at(pos) = value; }
				
			private:
				typedef clx::basic_format<CharT, Traits> fmt;
				
				bool entag_;
				container adjust_;
			};
			typedef basic_up_arrow_callout<char> up_arrow_callout;
		}
	}
}

#endif // FAML_PDF_CONTENTS_UP_ARROW_CALLOUT_H
