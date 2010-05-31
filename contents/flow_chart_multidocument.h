/* ------------------------------------------------------------------------- */
/*
 *  contents/flow_chart_multidocument.h
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
#ifndef FAML_PDF_CONTENTS_FLOW_CHART_MULTIDOCUMENT_H
#define FAML_PDF_CONTENTS_FLOW_CHART_MULTIDOCUMENT_H

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
			//  basic_flow_chart_multidocument
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_flow_chart_multidocument : public basic_polygon<CharT, Traits> {
			public:
				typedef basic_polygon<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				
				explicit basic_flow_chart_multidocument(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h), entag_(entag) {}
				
				virtual ~basic_flow_chart_multidocument() throw() {}
				
				virtual bool operator()(ostream_type& out) {
					// No need to draw and/or fill the shape.
					if (!this->is_stroke() && !this->is_fill()) return true;
					
					double w  =  this->width() * (1 - sh(3064));
					double h  =  this->height() * (1 - sh(3064));
					
					// Phase 1
					if (entag_) out << LITERAL("q") << std::endl;
					this->setorigin(out);
					this->setborder(out);
					this->setbackground(out);
					double ox = this->width() * (sh(3064) - 0.5);
					double oy = this->height() * (sh(3064) - 0.5);
					out << fmt(LITERAL("%f %f m")) % (ox) % (oy + h) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox) % (oy + h * sh(2139)) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox + w * 0.5) % (oy - h * 0.1)
							% (ox + w * 0.5) % (oy + h * sh(4278))
							% (ox + w) % (oy + h * sh(4278)) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox + w) % (oy + h) << std::endl;
					if (this->is_stroke() && this->is_fill()) out << LITERAL("b") << std::endl;
					else if (this->is_fill()) out << LITERAL("f") << std::endl;
					else out << LITERAL("s") << std::endl;
					if (entag_) out << LITERAL("Q") << std::endl;
					
					// Phase 2
					if (entag_) out << LITERAL("q") << std::endl;
					this->setorigin(out);
					this->setborder(out);
					this->setbackground(out);
					ox = this->width() * (sh(1532) - 0.5);
					oy = this->height() * (sh(1532) - 0.5);
					out << fmt(LITERAL("%f %f m")) % (ox) % (oy + h) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox) % (oy + h * sh(2139)) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox + w * 0.5) % (oy - h * 0.1)
							% (ox + w * 0.5) % (oy + h * sh(4278))
							% (ox + w) % (oy + h * sh(4278)) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox + w) % (oy + h) << std::endl;
					if (this->is_stroke() && this->is_fill()) out << LITERAL("b") << std::endl;
					else if (this->is_fill()) out << LITERAL("f") << std::endl;
					else out << LITERAL("s") << std::endl;
					if (entag_) out << LITERAL("Q") << std::endl;
					
					// Phase 23
					if (entag_) out << LITERAL("q") << std::endl;
					this->setorigin(out);
					this->setborder(out);
					this->setbackground(out);
					ox = this->width() * (- 0.5);
					oy = this->height() * (- 0.5);
					out << fmt(LITERAL("%f %f m")) % (ox) % (oy + h) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox) % (oy + h * sh(2139)) << std::endl;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (ox + w * 0.5) % (oy - h * 0.1)
							% (ox + w * 0.5) % (oy + h * sh(4278))
							% (ox + w) % (oy + h * sh(4278)) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox + w) % (oy + h) << std::endl;
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
			typedef basic_flow_chart_multidocument<char> flow_chart_multidocument;
		}
	}
}

#endif // FAML_PDF_CONTENTS_FLOW_CHART_MULTIDOCUMENT_H
