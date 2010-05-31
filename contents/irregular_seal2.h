/* ------------------------------------------------------------------------- */
/*
 *  contents/irregular_seal2.h
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
#ifndef FAML_PDF_CONTENTS_IRREGULAR_SEAL2_H
#define FAML_PDF_CONTENTS_IRREGULAR_SEAL2_H

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
			//  basic_irregular_seal2
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_irregular_seal2 : public basic_polygon<CharT, Traits> {
			public:
				typedef basic_polygon<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				
				explicit basic_irregular_seal2(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h, entag) {}
				
				virtual ~basic_irregular_seal2() throw() {}
				
				virtual bool operator()(ostream_type& out) {
					double ox = -this->width() / 2.0;
					double oy = -this->height() / 2.0;
					
					double x1  = 11462;	double y1  = 4342;
					double x2  = 9722;	double y2  = 1887;
					double x3  = 8550;	double y3  = 6382;
					double x4  = 4502;	double y4  = 3625;
					double x5  = 5372;	double y5  = 7817;
					double x6  = 1172;	double y6  = 8270;
					double x7  = 3935;	double y7  = 11592;
					double x8  = 0;		double y8  = 12877;
					double x9  = 3330;	double y9  = 15370;
					double x10 = 1285;	double y10 = 17825;
					double x11 = 4805;	double y11 = 18240;
					double x12 = 4917;	double y12 = 21600;
					double x13 = 7527;	double y13 = 18125;
					double x14 = 8700;	double y14 = 19712;
					double x15 = 9872;	double y15 = 17370;
					double x16 = 11612;	double y16 = 18842;
					double x17 = 12180;	double y17 = 15935;
					double x18 = 14942;	double y18 = 17370;
					double x19 = 14640;	double y19 = 14350;
					double x20 = 18877;	double y20 = 15632;
					double x21 = 16380;	double y21 = 12310;
					double x22 = 18270;	double y22 = 11290;
					double x23 = 16985;	double y23 = 9402;
					double x24 = 21600;	double y24 = 6645;
					double x25 = 16380;	double y25 = 6532;
					double x26 = 18007;	double y26 = 3172;
					double x27 = 14525;	double y27 = 5777;
					double x28 = 14790;	double y28 = 0;
					
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
					this->add(coordinate(ox+sh(x25)*this->width(), oy+(1-sh(y25))*this->height()));
					this->add(coordinate(ox+sh(x26)*this->width(), oy+(1-sh(y26))*this->height()));
					this->add(coordinate(ox+sh(x27)*this->width(), oy+(1-sh(y27))*this->height()));
					this->add(coordinate(ox+sh(x28)*this->width(), oy+(1-sh(y28))*this->height()));
					
					return super::operator()(out);
				}
			};
			typedef basic_irregular_seal2<char> irregular_seal2;
		}
	}
}

#endif // FAML_PDF_CONTENTS_IRREGULAR_SEAL2_H
