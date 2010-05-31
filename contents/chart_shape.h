/* ------------------------------------------------------------------------- */
/*
 *  contents/oct_multi.h
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
#ifndef FAML_PDF_CONTENTS_CHART_SHAPE_H
#define FAML_PDF_CONTENTS_CHART_SHAPE_H

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
			//  basic_chart_plus
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_chart_plus : public basic_shape<CharT, Traits> {
			public:
				typedef basic_shape<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				
				explicit basic_chart_plus(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h), entag_(entag) {}
				
				virtual ~basic_chart_plus() throw() {}
				
				virtual bool operator()(ostream_type& out) {
					double weight = 1.0;
					double ox = -this->width() / 2.0;
					double oy = -this->height() / 2.0;
					double x = ox + this->width();
					double y = oy + this->height();
					
					if (entag_) out << LITERAL("q") << std::endl;
					
					this->setorigin(out);
					this->setborder(out);
					
					out << fmt(LITERAL("%f w")) % weight << std::endl;
					
					out << fmt(LITERAL("%f %f m")) % ox % 0 << std::endl;
					out << fmt(LITERAL("%f %f l")) % x % 0 << std::endl;
					out << LITERAL("S") << std::endl;
					
					out << fmt(LITERAL("%f %f m")) % 0 % oy << std::endl;
					out << fmt(LITERAL("%f %f l")) % 0 % y << std::endl;
					out << LITERAL("S") << std::endl;
					
					if (entag_) out << LITERAL("Q") << std::endl;
					
					return true;
				}
				
			private:
				typedef clx::basic_format<CharT, Traits> fmt;
				bool entag_;
			};
			
			/* ------------------------------------------------------------- */
			//  basic_chart_multi
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_chart_multi : public basic_shape<CharT, Traits> {
			public:
				typedef basic_shape<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				
				explicit basic_chart_multi(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h), entag_(entag) {}
				
				virtual ~basic_chart_multi() throw() {}
				
				virtual bool operator()(ostream_type& out) {
					double weight = 1.0;
					double ox = -this->width() / 2.0;
					double oy = -this->height() / 2.0;
					double x = ox + this->width();
					double y = oy + this->height();
					
					if (entag_) out << LITERAL("q") << std::endl;
					
					this->setorigin(out);
					this->setborder(out);
					
					out << fmt(LITERAL("%f w")) % weight << std::endl;
					
					out << fmt(LITERAL("%f %f m")) % ox % oy << std::endl;
					out << fmt(LITERAL("%f %f l")) % x % y << std::endl;
					out << LITERAL("S") << std::endl;
					
					out << fmt(LITERAL("%f %f m")) % ox % y << std::endl;
					out << fmt(LITERAL("%f %f l")) % x % oy << std::endl;
					out << LITERAL("S") << std::endl;
					
					if (entag_) out << LITERAL("Q") << std::endl;
					
					return true;
				}
				
			private:
				typedef clx::basic_format<CharT, Traits> fmt;
				bool entag_;
			};
			
			/* ------------------------------------------------------------- */
			//  basic_chart_oct_multi
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_chart_oct_multi : public basic_shape<CharT, Traits> {
			public:
				typedef basic_shape<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				
				explicit basic_chart_oct_multi(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h), entag_(entag) {}
				
				virtual ~basic_chart_oct_multi() throw() {}
				
				virtual bool operator()(ostream_type& out) {
					double weight = 1.0;
					double ox = -this->width() / 2.0;
					double oy = -this->height() / 2.0;
					double x = ox + this->width();
					double y = oy + this->height();
					
					if (entag_) out << LITERAL("q") << std::endl;
					
					this->setorigin(out);
					this->setborder(out);
					
					out << fmt(LITERAL("%f w")) % weight << std::endl;
					
					out << fmt(LITERAL("%f %f m")) % ox % oy << std::endl;
					out << fmt(LITERAL("%f %f l")) % x % y << std::endl;
					out << LITERAL("S") << std::endl;
					
					out << fmt(LITERAL("%f %f m")) % ox % y << std::endl;
					out << fmt(LITERAL("%f %f l")) % x % oy << std::endl;
					out << LITERAL("S") << std::endl;
					
					out << fmt(LITERAL("%f %f m")) % 0 % oy << std::endl;
					out << fmt(LITERAL("%f %f l")) % 0 % y << std::endl;
					out << LITERAL("S") << std::endl;
					
					if (entag_) out << LITERAL("Q") << std::endl;
					
					return true;
				}
				
			private:
				typedef clx::basic_format<CharT, Traits> fmt;
				bool entag_;
			};
		}
	}
}

#endif // FAML_PDF_CONTENTS_CHART_SHAPE_H
