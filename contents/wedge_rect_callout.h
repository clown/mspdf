/* ------------------------------------------------------------------------- */
/*
 *  contents/wedge_rect_callout.h
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
#ifndef FAML_PDF_CONTENTS_WEDGE_RECT_CALLOUT_H
#define FAML_PDF_CONTENTS_WEDGE_RECT_CALLOUT_H

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
			//  basic_wedge_rect_callout
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_wedge_rect_callout : public basic_polygon<CharT, Traits> {
			public:
				typedef basic_polygon<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				typedef std::vector<double> container;
				
				explicit basic_wedge_rect_callout(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h, entag), adjust_() {
					adjust_.push_back(-0.4375); // 1350 - 10800
					adjust_.push_back(0.7); // 25920 - 10800
				}
				
				virtual ~basic_wedge_rect_callout() throw() {}
				
				virtual bool operator()(ostream_type& out) {
					// No need to draw and/or fill the shape.
					if (!this->is_stroke() && !this->is_fill()) return true;
					
					adjust_.at(0) += 0.5;
					adjust_.at(1) += 0.5;
					double ox = -this->width() / 2.0;
					double oy = -this->height() / 2.0;
					double x = adjust_.at(0);
					double y = 1-adjust_.at(1);
					double slope = (double) this->height() / (double) this->width();
					double long_path_w = this->width() * sh(3600);
					double short_path_w = this->width() * sh(1800);
					double long_path_h = this->height() * sh(3600);
					double short_path_h = this->height() * sh(1800);
					
					this->add(coordinate(ox, oy));
					if(y<0.5 && y*this->height()>slope*x*this->width()){
						this->add(coordinate(ox, oy + long_path_h));
						this->add(coordinate(ox + this->width()*x, oy + this->height()*y));
						this->add(coordinate(ox, oy + this->height()/2.0 - short_path_h));
					}
					else if(y>0.5 && y*this->height()<(-slope*x*this->width()+this->height())){
						this->add(coordinate(ox, oy + this->height()/2.0 + short_path_h));
						this->add(coordinate(ox + this->width()*x, oy + this->height()*y));
						this->add(coordinate(ox, oy + this->height() - long_path_h));
					}
					
					this->add(coordinate(ox, oy + this->height()));
					if(x<0.5 && y*this->height()>(-slope*x*this->width()+this->height())){
						this->add(coordinate(ox + long_path_w, oy + this->height()));
						this->add(coordinate(ox + this->width()*x, oy + this->height()*y));
						this->add(coordinate(ox + this->width()/2.0 - short_path_w, oy + this->height()));
					}
					else if(x>0.5 && y*this->height()>slope*x*this->width()){
						this->add(coordinate(ox + this->width()/2.0 + short_path_w, oy + this->height()));
						this->add(coordinate(ox + this->width()*x, oy + this->height()*y));
						this->add(coordinate(ox + this->width() - long_path_w, oy + this->height()));
					}
					
					this->add(coordinate(ox + this->width(), oy + this->height()));
					if(y>0.5 && y*this->height()<slope*x*this->width()){
						this->add(coordinate(ox + this->width(), oy + this->height() - long_path_h));
						this->add(coordinate(ox + this->width()*x, oy + this->height()*y));
						this->add(coordinate(ox + this->width(), oy + this->height()/2.0 + short_path_h));
					}
					else if(y<0.5 && y*this->height()>(-slope*x*this->width()+this->height())){
						this->add(coordinate(ox + this->width(), oy + this->height()/2.0 - short_path_h));
						this->add(coordinate(ox + this->width()*x, oy + this->height()*y));
						this->add(coordinate(ox + this->width(), oy + long_path_h));
					}
					
					this->add(coordinate(ox + this->width(), oy));
					if(x>0.5 && y*this->height()<(-slope*x*this->width()+this->height())){
						this->add(coordinate(ox + this->width() - long_path_w, oy));
						this->add(coordinate(ox + this->width()*x, oy + this->height()*y));
						this->add(coordinate(ox + this->width()/2.0 + short_path_w, oy));
					}
					else if(x<0.5 && y*this->height()<slope*x*this->width()){
						this->add(coordinate(ox + this->width()/2.0 - short_path_w, oy));
						this->add(coordinate(ox + this->width()*x, oy + this->height()*y));
						this->add(coordinate(ox + long_path_w, oy));
					}
					
					return super::operator()(out);
				}
				
				virtual size_type has_adjust() const { return 2; }
				virtual double adjust(size_type pos = 0) const { return adjust_.at(pos); }
				virtual void adjust(double value, size_type pos = 0) { adjust_.at(pos) = value; }
				
			private:
				typedef clx::basic_format<CharT, Traits> fmt;
				
				bool entag_;
				container adjust_;
			};
			typedef basic_wedge_rect_callout<char> wedge_rect_callout;
		}
	}
}

#endif // FAML_PDF_CONTENTS_WEDGE_RECT_CALLOUT_H
