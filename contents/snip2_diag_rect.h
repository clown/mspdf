/* ------------------------------------------------------------------------- */
/*
 *  contents/snip2_diag_rect.h
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
#ifndef FAML_PDF_CONTENTS_SNIP2_DIAG_RECT_H
#define FAML_PDF_CONTENTS_SNIP2_DIAG_RECT_H

#ifndef FAML_PDF_CONTENTS_SHAPE_H
#include "shape.h"
#endif // FAML_PDF_CONTENTS_SHAPE_H

#include <cassert>
#include <vector>
#include "clx/format.h"
#include "clx/literal.h"
#include "../unit.h"

namespace faml {
	namespace pdf {
		namespace contents {
			/* ------------------------------------------------------------- */
			//  basic_snip2_diag_rect
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_snip2_diag_rect : public basic_polygon<CharT, Traits> {
			public:
				typedef basic_polygon<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				typedef std::vector<double> container;
				
				explicit basic_snip2_diag_rect(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h, entag), adjust_() {
					adjust_.push_back(0.0);
					adjust_.push_back(0.16667);
				}
				
				virtual ~basic_snip2_diag_rect() throw() {}
				
				virtual bool operator()(ostream_type& out) {
					double ox = -this->width() / 2.0;
					double oy = -this->height() / 2.0;
					double delta1 = (this->width() < this->height()) ? this->width() * adjust_.at(0) : this->height() * adjust_.at(0);
					double delta2 = (this->width() < this->height()) ? this->width() * adjust_.at(1) : this->height() * adjust_.at(1);
					
					this->add(coordinate(ox, oy + this->height() - delta1));
					this->add(coordinate(ox + delta1, oy + this->height()));
					this->add(coordinate(ox + this->width() - delta2, oy + this->height()));
					this->add(coordinate(ox + this->width(), oy + this->height() - delta2));
					this->add(coordinate(ox + this->width(), oy + delta1));
					this->add(coordinate(ox + this->width() - delta1, oy));
					this->add(coordinate(ox + delta2, oy));
					this->add(coordinate(ox, oy + delta2));
					return super::operator()(out);
				}
				
				virtual size_type has_adjust() const { return 2; }
				virtual double adjust(size_type pos = 0) const { return adjust_.at(pos); }
				virtual void adjust(double value, size_type pos = 0) { adjust_.at(pos) = value; }
				
			private:
				container adjust_;
			};
			typedef basic_snip2_diag_rect<char> snip2_diag_rect;
		}
	}
}

#endif // FAML_PDF_CONTENTS_SNIP2_DIAG_RECT_H