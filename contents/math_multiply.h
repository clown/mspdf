/* ------------------------------------------------------------------------- */
/*
 *  contents/math_multiply.h
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
#ifndef FAML_PDF_CONTENTS_MATH_MULTIPLY_H
#define FAML_PDF_CONTENTS_MATH_MULTIPLY_H

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
			//  basic_math_multiply
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_math_multiply : public basic_polygon<CharT, Traits> {
			public:
				typedef basic_polygon<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				
				explicit basic_math_multiply(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h), entag_(entag), adjust_(0.25) {}
				
				virtual ~basic_math_multiply() throw() {}
				
				virtual bool operator()(ostream_type& out) {
					// No need to draw and/or fill the shape.
					if (!this->is_stroke() && !this->is_fill()) return true;
					
					double ox = 0;
					double oy = 0;
					double delta = (this->width() < this->height()) ? this->width() * adjust_ : this->height() * adjust_;
					
					if (entag_) out << LITERAL("q") << std::endl;
					this->setorigin(out);
					this->setborder(out);
					this->setbackground(out);
					double w  =  this->width() * (3.0/8.0);
					double h  =  this->height() * (3.0/8.0);
					double temp = w*w + h*h;
					double dx = (delta/2.0) * (h/(double)std::sqrt((double)temp));
					double dy = (delta/2.0) * (w/(double)std::sqrt((double)temp));
					double ox1 = -w*(1/std::sqrt(2.0)) + dx;
					double oy1 =  h*(1/std::sqrt(2.0)) + dy;
					double ox2 = -w*(1/std::sqrt(2.0)) - dx;
					double oy2 =  h*(1/std::sqrt(2.0)) - dy;
					double ox3 = ox2;
					double oy3 = -oy2;
					double ox4 = ox1;
					double oy4 = -oy1;
					double ox5 =  w*(1/std::sqrt(2.0)) - dx;
					double oy5 = -h*(1/std::sqrt(2.0)) - dy;
					double ox6 =  w*(1/std::sqrt(2.0)) + dx;
					double oy6 = -h*(1/std::sqrt(2.0)) + dy;
					double ox7 = ox6;
					double oy7 = -oy6;
					double ox8 = ox5;
					double oy8 = -oy5;
					
					double ix1 = ox2 + (w/h) * oy2;
					double iy1 = 0;
					double ix2 = 0;
					double iy2 = -(h/w) * ox4 + oy4;
					double ix3 = -ix1;
					double iy3 = 0;
					double ix4 = 0;
					double iy4 = -iy2;
					
					out << fmt(LITERAL("%f %f m")) % (ox + ox1) % (oy + oy1) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox + ox2) % (oy + oy2) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox + ix1) % (oy + iy1) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox + ox3) % (oy + oy3) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox + ox4) % (oy + oy4) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox + ix2) % (oy + iy2) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox + ox5) % (oy + oy5) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox + ox6) % (oy + oy6) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox + ix3) % (oy + iy3) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox + ox7) % (oy + oy7) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox + ox8) % (oy + oy8) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox + ix4) % (oy + iy4) << std::endl;
					
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
			typedef basic_math_multiply<char> math_multiply;
		}
	}
}

#endif // FAML_PDF_CONTENTS_MATH_MULTIPLY_H
