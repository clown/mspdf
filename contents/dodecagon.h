/* ------------------------------------------------------------------------- */
/*
 *  contents/dodecagon.h
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
#ifndef FAML_PDF_CONTENTS_DODECAGON_H
#define FAML_PDF_CONTENTS_DODECAGON_H

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
			//  basic_dodecagon
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_dodecagon : public basic_polygon<CharT, Traits> {
			public:
				typedef basic_polygon<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				
				explicit basic_dodecagon(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h), entag_(entag) {}
				
				virtual ~basic_dodecagon() throw() {}
				
				virtual bool operator()(ostream_type& out) {
					// No need to draw and/or fill the shape.
					if (!this->is_stroke() && !this->is_fill()) return true;
					
					double ox = 0;
					double oy = 0;
					
					const double PI = 3.1415;
					double rad = 360 * PI /  (12 * 180.0);
					
					// 21600 * 21600
					double x1 = - 5400*(std::sqrt(2.0-std::sqrt(3.0)));
					double y1 = 10800;
					double x2 = x1 * cos(rad) - y1 * sin(rad);
					double y2 = x1 * sin(rad) + y1 * cos(rad);
					double x3 = x2 * cos(rad) - y2 * sin(rad);
					double y3 = x2 * sin(rad) + y2 * cos(rad);
					double x4 = x3 * cos(rad) - y3 * sin(rad);
					double y4 = x3 * sin(rad) + y3 * cos(rad);
					double x5 = x4 * cos(rad) - y4 * sin(rad);
					double y5 = x4 * sin(rad) + y4 * cos(rad);
					double x6 = x5 * cos(rad) - y5 * sin(rad);
					double y6 = x5 * sin(rad) + y5 * cos(rad);
					double x7 = 5400*(std::sqrt(2.0-std::sqrt(3.0)));
					double y7 = -10800;
					double x8 = x7 * cos(rad) - y7 * sin(rad);
					double y8 = x7 * sin(rad) + y7 * cos(rad);
					double x9 = x8 * cos(rad) - y8 * sin(rad);
					double y9 = x8 * sin(rad) + y8 * cos(rad);
					double x10 = x9 * cos(rad) - y9 * sin(rad);
					double y10 = x9 * sin(rad) + y9 * cos(rad);
					double x11 = x10 * cos(rad) - y10 * sin(rad);
					double y11 = x10 * sin(rad) + y10 * cos(rad);
					double x12 = x11 * cos(rad) - y11 * sin(rad);
					double y12 = x11 * sin(rad) + y11 * cos(rad);
					
					if (entag_) out << LITERAL("q") << std::endl;
					this->setorigin(out);
					this->setborder(out);
					this->setbackground(out);
					out << fmt(LITERAL("%f %f m")) % (ox+x1*this->width()/21600.0) % (oy+y1*this->height()/21600.0) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox+x2*this->width()/21600.0) % (oy+y2*this->height()/21600.0) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox+x3*this->width()/21600.0) % (oy+y3*this->height()/21600.0) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox+x4*this->width()/21600.0) % (oy+y4*this->height()/21600.0) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox+x5*this->width()/21600.0) % (oy+y5*this->height()/21600.0) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox+x6*this->width()/21600.0) % (oy+y6*this->height()/21600.0) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox+x7*this->width()/21600.0) % (oy+y7*this->height()/21600.0) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox+x8*this->width()/21600.0) % (oy+y8*this->height()/21600.0) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox+x9*this->width()/21600.0) % (oy+y9*this->height()/21600.0) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox+x10*this->width()/21600.0) % (oy+y10*this->height()/21600.0) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox+x11*this->width()/21600.0) % (oy+y11*this->height()/21600.0) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox+x12*this->width()/21600.0) % (oy+y12*this->height()/21600.0) << std::endl;
					if (this->is_stroke() && this->is_fill()) out << LITERAL("b") << std::endl;
					else if (this->is_fill()) out << LITERAL("f") << std::endl;
					else out << LITERAL("s") << std::endl;
					if (entag_) out << LITERAL("Q") << std::endl;
					
					return true;
				}
				
			private:
				typedef clx::basic_format<CharT, Traits> fmt;
				
				bool entag_;
			};
			typedef basic_dodecagon<char> dodecagon;
		}
	}
}

#endif // FAML_PDF_CONTENTS_DODECAGON_H
