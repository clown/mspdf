/* ------------------------------------------------------------------------- */
/*
 *  contents/cloud.h
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
#ifndef FAML_PDF_CONTENTS_CLOUD_H
#define FAML_PDF_CONTENTS_CLOUD_H

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
			//  basic_cloud
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_cloud : public basic_shape<CharT, Traits> {
			public:
				typedef basic_shape<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				
				explicit basic_cloud(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h), entag_(entag), adjust_(sh(10800)) {}
				
				virtual ~basic_cloud() throw() {}
				
				virtual bool operator()(ostream_type& out) {
					// No need to draw and/or fill the shape.
					if (!this->is_stroke() && !this->is_fill()) return true;
					
					double ox = -this->width() / 2.0;
					double oy = -this->height() / 2.0;
					
					// Phase 1
					if (entag_) out << LITERAL("q") << std::endl;
					this->setorigin(out);
					this->setborder(out);
					this->setbackground(out);
					
					out << fmt(LITERAL("%f %f m")) % (ox+this->width()*sh(1949)) % (oy+this->height()*(1-sh(7180))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(841)) % (oy+this->height()*(1-sh(7336)))
							% (ox+this->width()*sh(0)) % (oy+this->height()*(1-sh(8613)))
							% (ox+this->width()*sh(0)) % (oy+this->height()*(1-sh(10137))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(-1)) % (oy+this->height()*(1-sh(11192)))
							% (ox+this->width()*sh(409)) % (oy+this->height()*(1-sh(12169)))
							% (ox+this->width()*sh(1074)) % (oy+this->height()*(1-sh(12702))) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox+this->width()*sh(1063)) % (oy+this->height()*(1-sh(12668))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(685)) % (oy+this->height()*(1-sh(13217)))
							% (ox+this->width()*sh(475)) % (oy+this->height()*(1-sh(13940)))
							% (ox+this->width()*sh(475)) % (oy+this->height()*(1-sh(14690))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(475)) % (oy+this->height()*(1-sh(16325)))
							% (ox+this->width()*sh(1451)) % (oy+this->height()*(1-sh(17650)))
							% (ox+this->width()*sh(2655)) % (oy+this->height()*(1-sh(17650))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(2739)) % (oy+this->height()*(1-sh(17650)))
							% (ox+this->width()*sh(2824)) % (oy+this->height()*(1-sh(17643)))
							% (ox+this->width()*sh(2909)) % (oy+this->height()*(1-sh(17629))) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox+this->width()*sh(2897)) % (oy+this->height()*(1-sh(17649))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(3585)) % (oy+this->height()*(1-sh(19288)))
							% (ox+this->width()*sh(4863)) % (oy+this->height()*(1-sh(20300)))
							% (ox+this->width()*sh(6247)) % (oy+this->height()*(1-sh(20300))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(6947)) % (oy+this->height()*(1-sh(20299)))
							% (ox+this->width()*sh(7635)) % (oy+this->height()*(1-sh(20039)))
							% (ox+this->width()*sh(8235)) % (oy+this->height()*(1-sh(19546))) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox+this->width()*sh(8229)) % (oy+this->height()*(1-sh(19550))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(8855)) % (oy+this->height()*(1-sh(20829)))
							% (ox+this->width()*sh(9908)) % (oy+this->height()*(1-sh(21597)))
							% (ox+this->width()*sh(11036)) % (oy+this->height()*(1-sh(21597))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(12523)) % (oy+this->height()*(1-sh(21596)))
							% (ox+this->width()*sh(13836)) % (oy+this->height()*(1-sh(20267)))
							% (ox+this->width()*sh(14267)) % (oy+this->height()*(1-sh(18324))) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox+this->width()*sh(14270)) % (oy+this->height()*(1-sh(18350))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(14730)) % (oy+this->height()*(1-sh(18740)))
							% (ox+this->width()*sh(15260)) % (oy+this->height()*(1-sh(18947)))
							% (ox+this->width()*sh(15802)) % (oy+this->height()*(1-sh(18947))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(17390)) % (oy+this->height()*(1-sh(18946)))
							% (ox+this->width()*sh(18682)) % (oy+this->height()*(1-sh(17205)))
							% (ox+this->width()*sh(18694)) % (oy+this->height()*(1-sh(15045))) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox+this->width()*sh(18689)) % (oy+this->height()*(1-sh(15035))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(20357)) % (oy+this->height()*(1-sh(14710)))
							% (ox+this->width()*sh(21597)) % (oy+this->height()*(1-sh(12765)))
							% (ox+this->width()*sh(21597)) % (oy+this->height()*(1-sh(10472))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(21597)) % (oy+this->height()*(1-sh(9456)))
							% (ox+this->width()*sh(21350)) % (oy+this->height()*(1-sh(8469)))
							% (ox+this->width()*sh(20896)) % (oy+this->height()*(1-sh(7663))) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox+this->width()*sh(20889)) % (oy+this->height()*(1-sh(7661))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(21031)) % (oy+this->height()*(1-sh(7208)))
							% (ox+this->width()*sh(21105)) % (oy+this->height()*(1-sh(6721)))
							% (ox+this->width()*sh(21105)) % (oy+this->height()*(1-sh(6228))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(21105)) % (oy+this->height()*(1-sh(4588)))
							% (ox+this->width()*sh(20299)) % (oy+this->height()*(1-sh(3150)))
							% (ox+this->width()*sh(19139)) % (oy+this->height()*(1-sh(2719))) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox+this->width()*sh(19148)) % (oy+this->height()*(1-sh(2712))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(18940)) % (oy+this->height()*(1-sh(1142)))
							% (ox+this->width()*sh(17933)) % (oy+this->height()*(1-sh(0)))
							% (ox+this->width()*sh(16758)) % (oy+this->height()*(1-sh(0))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(16044)) % (oy+this->height()*(1-sh(-1)))
							% (ox+this->width()*sh(15367)) % (oy+this->height()*(1-sh(426)))
							% (ox+this->width()*sh(14905)) % (oy+this->height()*(1-sh(1165))) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox+this->width()*sh(14909)) % (oy+this->height()*(1-sh(1170))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(14497)) % (oy+this->height()*(1-sh(432)))
							% (ox+this->width()*sh(13855)) % (oy+this->height()*(1-sh(0)))
							% (ox+this->width()*sh(13174)) % (oy+this->height()*(1-sh(0))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(12347)) % (oy+this->height()*(1-sh(-1)))
							% (ox+this->width()*sh(11590)) % (oy+this->height()*(1-sh(637)))
							% (ox+this->width()*sh(11221)) % (oy+this->height()*(1-sh(1645))) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox+this->width()*sh(11229)) % (oy+this->height()*(1-sh(1694))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(10730)) % (oy+this->height()*(1-sh(1024)))
							% (ox+this->width()*sh(10058)) % (oy+this->height()*(1-sh(650)))
							% (ox+this->width()*sh(9358)) % (oy+this->height()*(1-sh(650))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(8372)) % (oy+this->height()*(1-sh(649)))
							% (ox+this->width()*sh(7466)) % (oy+this->height()*(1-sh(1391)))
							% (ox+this->width()*sh(7003)) % (oy+this->height()*(1-sh(2578))) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox+this->width()*sh(6995)) % (oy+this->height()*(1-sh(2602))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(6477)) % (oy+this->height()*(1-sh(2189)))
							% (ox+this->width()*sh(5888)) % (oy+this->height()*(1-sh(1972)))
							% (ox+this->width()*sh(5288)) % (oy+this->height()*(1-sh(1972))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(3423)) % (oy+this->height()*(1-sh(1972)))
							% (ox+this->width()*sh(1912)) % (oy+this->height()*(1-sh(4029)))
							% (ox+this->width()*sh(1912)) % (oy+this->height()*(1-sh(6567))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(1911)) % (oy+this->height()*(1-sh(6774)))
							% (ox+this->width()*sh(1922)) % (oy+this->height()*(1-sh(6981)))
							% (ox+this->width()*sh(1942)) % (oy+this->height()*(1-sh(7186))) << std::endl;
					
					out << fmt(LITERAL("%f %f m")) % (ox+this->width()*sh(1074)) % (oy+this->height()*(1-sh(12702))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(1407)) % (oy+this->height()*(1-sh(12969)))
							% (ox+this->width()*sh(1786)) % (oy+this->height()*(1-sh(13110)))
							% (ox+this->width()*sh(2172)) % (oy+this->height()*(1-sh(13110))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(2228)) % (oy+this->height()*(1-sh(13109)))
							% (ox+this->width()*sh(2285)) % (oy+this->height()*(1-sh(13107)))
							% (ox+this->width()*sh(2341)) % (oy+this->height()*(1-sh(13101))) << std::endl;
					
					out << fmt(LITERAL("%f %f m")) % (ox+this->width()*sh(2909)) % (oy+this->height()*(1-sh(17629))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(3099)) % (oy+this->height()*(1-sh(17599)))
							% (ox+this->width()*sh(3285)) % (oy+this->height()*(1-sh(17535)))
							% (ox+this->width()*sh(3463)) % (oy+this->height()*(1-sh(17439))) << std::endl;
					
					out << fmt(LITERAL("%f %f m")) % (ox+this->width()*sh(7895)) % (oy+this->height()*(1-sh(18680))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(7983)) % (oy+this->height()*(1-sh(18985)))
							% (ox+this->width()*sh(8095)) % (oy+this->height()*(1-sh(19277)))
							% (ox+this->width()*sh(8229)) % (oy+this->height()*(1-sh(19550))) << std::endl;
					
					out << fmt(LITERAL("%f %f m")) % (ox+this->width()*sh(14267)) % (oy+this->height()*(1-sh(18324))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(14336)) % (oy+this->height()*(1-sh(18013)))
							% (ox+this->width()*sh(14380)) % (oy+this->height()*(1-sh(17693)))
							% (ox+this->width()*sh(14400)) % (oy+this->height()*(1-sh(17370))) << std::endl;
					
					out << fmt(LITERAL("%f %f m")) % (ox+this->width()*sh(18694)) % (oy+this->height()*(1-sh(15045))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(18694)) % (oy+this->height()*(1-sh(15034)))
							% (ox+this->width()*sh(18695)) % (oy+this->height()*(1-sh(15024)))
							% (ox+this->width()*sh(18695)) % (oy+this->height()*(1-sh(15013))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(18695)) % (oy+this->height()*(1-sh(13508)))
							% (ox+this->width()*sh(18063)) % (oy+this->height()*(1-sh(12136)))
							% (ox+this->width()*sh(17069)) % (oy+this->height()*(1-sh(11477))) << std::endl;
					
					out << fmt(LITERAL("%f %f m")) % (ox+this->width()*sh(20165)) % (oy+this->height()*(1-sh(8999))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(20479)) % (oy+this->height()*(1-sh(8635)))
							% (ox+this->width()*sh(20726)) % (oy+this->height()*(1-sh(8177)))
							% (ox+this->width()*sh(20889)) % (oy+this->height()*(1-sh(7661))) << std::endl;
					
					out << fmt(LITERAL("%f %f m")) % (ox+this->width()*sh(19186)) % (oy+this->height()*(1-sh(3344))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(19186)) % (oy+this->height()*(1-sh(3328)))
							% (ox+this->width()*sh(19187)) % (oy+this->height()*(1-sh(3313)))
							% (ox+this->width()*sh(19187)) % (oy+this->height()*(1-sh(3297))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(19187)) % (oy+this->height()*(1-sh(3101)))
							% (ox+this->width()*sh(19174)) % (oy+this->height()*(1-sh(2905)))
							% (ox+this->width()*sh(19148)) % (oy+this->height()*(1-sh(2712))) << std::endl;
					
					out << fmt(LITERAL("%f %f m")) % (ox+this->width()*sh(14905)) % (oy+this->height()*(1-sh(1165))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(14754)) % (oy+this->height()*(1-sh(1408)))
							% (ox+this->width()*sh(14629)) % (oy+this->height()*(1-sh(1679)))
							% (ox+this->width()*sh(14535)) % (oy+this->height()*(1-sh(1971))) << std::endl;
					
					out << fmt(LITERAL("%f %f m")) % (ox+this->width()*sh(11221)) % (oy+this->height()*(1-sh(1645))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(11140)) % (oy+this->height()*(1-sh(1866)))
							% (ox+this->width()*sh(11080)) % (oy+this->height()*(1-sh(2099)))
							% (ox+this->width()*sh(11041)) % (oy+this->height()*(1-sh(2340))) << std::endl;
					
					out << fmt(LITERAL("%f %f m")) % (ox+this->width()*sh(7645)) % (oy+this->height()*(1-sh(3276))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(7449)) % (oy+this->height()*(1-sh(3016)))
							% (ox+this->width()*sh(7231)) % (oy+this->height()*(1-sh(2790)))
							% (ox+this->width()*sh(6995)) % (oy+this->height()*(1-sh(2602))) << std::endl;
					
					out << fmt(LITERAL("%f %f m")) % (ox+this->width()*sh(1942)) % (oy+this->height()*(1-sh(7186))) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox+this->width()*sh(1966)) % (oy+this->height()*(1-sh(7426)))
							% (ox+this->width()*sh(2004)) % (oy+this->height()*(1-sh(7663)))
							% (ox+this->width()*sh(2056)) % (oy+this->height()*(1-sh(7895))) << std::endl;
					
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
			typedef basic_cloud<char> cloud;
		}
	}
}

#endif // FAML_PDF_CONTENTS_CLOUD_H
