/* ------------------------------------------------------------------------- */
/*
 *  contents/lightning_bolt.h
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
#ifndef FAML_PDF_CONTENTS_LIGHTNING_BOLT_H
#define FAML_PDF_CONTENTS_LIGHTNING_BOLT_H

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
			//  basic_lightning_bolt
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_lightning_bolt : public basic_polygon<CharT, Traits> {
			public:
				typedef basic_polygon<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				
				explicit basic_lightning_bolt(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h, entag) {}
				
				virtual ~basic_lightning_bolt() throw() {}
				
				virtual bool operator()(ostream_type& out) {
					double ox = -this->width() / 2.0;
					double oy = -this->height() / 2.0;
					
					this->add(coordinate(ox + this->width(), oy));
					this->add(coordinate(ox + this->width()*0.69, oy + this->height()*0.4));
					this->add(coordinate(ox + this->width()*0.77, oy + this->height()*0.45));
					this->add(coordinate(ox + this->width()*0.52, oy + this->height()*0.68));
					this->add(coordinate(ox + this->width()*0.59, oy + this->height()*0.72));
					this->add(coordinate(ox + this->width()*0.39, oy + this->height()));
					this->add(coordinate(ox, oy + this->height()*0.81));
					this->add(coordinate(ox + this->width()*0.35, oy + this->height()*0.61));
					this->add(coordinate(ox + this->width()*0.24, oy + this->height()*0.55));
					this->add(coordinate(ox + this->width()*0.56, oy + this->height()*0.35));
					this->add(coordinate(ox + this->width()*0.48, oy + this->height()*0.3));
					return super::operator()(out);
				}
			};
			typedef basic_lightning_bolt<char> lightning_bolt;
		}
	}
}

#endif // FAML_PDF_CONTENTS_LIGHTNING_BOLT_H
