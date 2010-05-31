/* ------------------------------------------------------------------------- */
/*
 *  contents/irregular_seal1.h
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
#ifndef FAML_PDF_CONTENTS_IRREGULAR_SEAL1_H
#define FAML_PDF_CONTENTS_IRREGULAR_SEAL1_H

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
			//  basic_irregular_seal1
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_irregular_seal1 : public basic_polygon<CharT, Traits> {
			public:
				typedef basic_polygon<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				
				explicit basic_irregular_seal1(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h, entag) {}
				
				virtual ~basic_irregular_seal1() throw() {}
				
				virtual bool operator()(ostream_type& out) {
					double ox = -this->width() / 2.0;
					double oy = -this->height() / 2.0;
					
					double x1  = 10800;	double y1 = 5800;
					double x2  = 8352;	double y2 = 2295;
					double x3  = 7312;	double y3 = 6320;
					double x4  = 370;	double y4 = 2295;
					double x5  = 4627;	double y5 = 7617;
					double x6  = 0;		double y6 = 8615;
					double x7  = 3722;	double y7 = 11775;
					double x8  = 135;	double y8 = 14587;
					double x9  = 5667;	double y9 = 13937;
					double x10 = 4762;	double y10 = 17617;
					double x11 = 7715;	double y11 = 15627;
					double x12 = 8485;	double y12 = 21600;
					double x13 = 10532;	double y13 = 14935;
					double x14 = 13247;	double y14 = 19737;
					double x15 = 14020;	double y15 = 14457;
					double x16 = 18145;	double y16 = 18095;
					double x17 = 16837;	double y17 = 12942;
					double x18 = 21600;	double y18 = 13290;
					double x19 = 17607;	double y19 = 10475;
					double x20 = 21097;	double y20 = 8137;
					double x21 = 16702;	double y21 = 7315;
					double x22 = 18380;	double y22 = 4457;
					double x23 = 14155;	double y23 = 5325;
					double x24 = 14522;	double y24 = 0;
					
					this->add(coordinate(ox+sh(x1)*this->width(), oy+(1-sh(y1))*this->height()));
					this->add(coordinate(ox+sh(x2)*this->width(), oy+(1-sh(y2))*this->height()));
					this->add(coordinate(ox+sh(x3)*this->width(), oy+(1-sh(y3))*this->height()));
					this->add(coordinate(ox+sh(x4)*this->width(), oy+(1-sh(y4))*this->height()));
					this->add(coordinate(ox+sh(x5)*this->width(), oy+(1-sh(y5))*this->height()));
					this->add(coordinate(ox+sh(x6)*this->width(), oy+(1-sh(y6))*this->height()));
					this->add(coordinate(ox+sh(x7)*this->width(), oy+(1-sh(y7))*this->height()));
					this->add(coordinate(ox+sh(x8)*this->width(), oy+(1-sh(y8))*this->height()));
					this->add(coordinate(ox+sh(x9)*this->width(), oy+(1-sh(y9))*this->height()));
					this->add(coordinate(ox+sh(x10)*this->width(), oy+(1-sh(y10))*this->height()));
					this->add(coordinate(ox+sh(x11)*this->width(), oy+(1-sh(y11))*this->height()));
					this->add(coordinate(ox+sh(x12)*this->width(), oy+(1-sh(y12))*this->height()));
					this->add(coordinate(ox+sh(x13)*this->width(), oy+(1-sh(y13))*this->height()));
					this->add(coordinate(ox+sh(x14)*this->width(), oy+(1-sh(y14))*this->height()));
					this->add(coordinate(ox+sh(x15)*this->width(), oy+(1-sh(y15))*this->height()));
					this->add(coordinate(ox+sh(x16)*this->width(), oy+(1-sh(y16))*this->height()));
					this->add(coordinate(ox+sh(x17)*this->width(), oy+(1-sh(y17))*this->height()));
					this->add(coordinate(ox+sh(x18)*this->width(), oy+(1-sh(y18))*this->height()));
					this->add(coordinate(ox+sh(x19)*this->width(), oy+(1-sh(y19))*this->height()));
					this->add(coordinate(ox+sh(x20)*this->width(), oy+(1-sh(y20))*this->height()));
					this->add(coordinate(ox+sh(x21)*this->width(), oy+(1-sh(y21))*this->height()));
					this->add(coordinate(ox+sh(x22)*this->width(), oy+(1-sh(y22))*this->height()));
					this->add(coordinate(ox+sh(x23)*this->width(), oy+(1-sh(y23))*this->height()));
					this->add(coordinate(ox+sh(x24)*this->width(), oy+(1-sh(y24))*this->height()));
					
					return super::operator()(out);
				}
			};
			typedef basic_irregular_seal1<char> irregular_seal1;
		}
	}
}

#endif // FAML_PDF_CONTENTS_IRREGULAR_SEAL1_H
