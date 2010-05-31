/* ------------------------------------------------------------------------- */
/*
 *  contents/bracket.h
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
#ifndef FAML_PDF_CONTENTS_BRACKET_H
#define FAML_PDF_CONTENTS_BRACKET_H

#ifndef FAML_PDF_CONTENTS_SHAPE_H
#include "shape.h"
#endif // FAML_PDF_CONTENTS_SHAPE_H

#include <vector>
#include "circle.h"

namespace faml {
	namespace pdf {
		namespace contents {
			/* ------------------------------------------------------------- */
			//  basic_left_bracket
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_left_bracket : public basic_shape<CharT, Traits> {
			public:
				typedef basic_shape<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				
				explicit basic_left_bracket(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h), entag_(entag), adjust_(0.08333) {}
				
				virtual ~basic_left_bracket() throw() {}
				
				virtual size_type has_adjust() const { return 1; }
				virtual double adjust(size_type pos = 0) const { return adjust_; }
				virtual void adjust(double value, size_type pos = 0) { adjust_ = value; }
				
				/* --------------------------------------------------------- */
				//  main operation.
				/* --------------------------------------------------------- */
				bool operator()(ostream_type& out) {
					// No need to draw and/or fill the shape.
					if (!this->is_stroke()) return true;
					
					if (entag_) out << LITERAL("q") << std::endl;
					
					this->setorigin(out);
					this->setborder(out);
					this->setbackground(out);
					
					double ox = -this->width() / 2.0;
					double oy = -this->height() / 2.0;
					double delta = (this->width() < this->height()) ? this->width() * adjust_ : this->height() * adjust_;
					double x = this->width();
					double y = this->height() - delta;
					double w = this->width();
					double h = delta;
					
					bool status = detail::draw_arc(out, ox + x, oy + y, w, h, 2);
					y = delta;
					out << fmt(LITERAL("%f %f l")) % ox % (oy + y) << std::endl;
					status &= detail::draw_arc(out, ox + x, oy + y, w, h, 3, true);
					out << LITERAL("S") << std::endl;
					if (this->entag_) out << LITERAL("Q") << std::endl;
					
					return status;
				}
				
			private:
				typedef clx::basic_format<CharT, Traits> fmt;
				
				bool entag_;
				double adjust_;
			};
			
			/* ------------------------------------------------------------- */
			//  basic_right_bracket
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_right_bracket : public basic_shape<CharT, Traits> {
			public:
				typedef basic_shape<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				
				explicit basic_right_bracket(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h), entag_(entag), adjust_(0.08333) {}
				
				virtual ~basic_right_bracket() throw() {}
				
				virtual size_type has_adjust() const { return 1; }
				virtual double adjust(size_type pos = 0) const { return adjust_; }
				virtual void adjust(double value, size_type pos = 0) { adjust_ = value; }
				
				
				/* --------------------------------------------------------- */
				//  main operation.
				/* --------------------------------------------------------- */
				bool operator()(ostream_type& out) {
					// No need to draw and/or fill the shape.
					if (!this->is_stroke()) return true;
					
					if (entag_) out << LITERAL("q") << std::endl;
					
					this->setorigin(out);
					this->setborder(out);
					this->setbackground(out);
					
					double ox = -this->width() / 2.0;
					double oy = -this->height() / 2.0;
					double delta = (this->width() < this->height()) ? this->width() * adjust_ : this->height() * adjust_;
					double x = 0;
					double y = delta;
					double w = this->width();
					double h = delta;
					
					bool status = detail::draw_arc(out, ox + x, oy + y, w, h, 4);
					y = this->height() - delta;
					out << fmt(LITERAL("%f %f l")) % (ox + this->width()) % (oy + y) << std::endl;
					status &= detail::draw_arc(out, ox + x, oy + y, w, h, 1, true);
					out << LITERAL("S") << std::endl;
					if (this->entag_) out << LITERAL("Q") << std::endl;
					
					return status;
				}
				
			private:
				typedef clx::basic_format<CharT, Traits> fmt;
				
				bool entag_;
				double adjust_;
			};
			
			/* ------------------------------------------------------------- */
			//  basic_left_brace
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_left_brace : public basic_shape<CharT, Traits> {
			public:
				typedef basic_shape<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				typedef std::vector<double> container;
				
				explicit basic_left_brace(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h), entag_(entag), adjust_() {
					adjust_.push_back(0.08333);
					adjust_.push_back(0.5);
				}
				
				virtual ~basic_left_brace() throw() {}
				
				virtual size_type has_adjust() const { return 2; }
				virtual double adjust(size_type pos = 0) const { return adjust_.at(pos); }
				virtual void adjust(double value, size_type pos = 0) { adjust_.at(pos) = value; }
				
				/* --------------------------------------------------------- */
				//  main operation.
				/* --------------------------------------------------------- */
				bool operator()(ostream_type& out) {
					// No need to draw and/or fill the shape.
					if (!this->is_stroke()) return true;
					
					if (entag_) out << LITERAL("q") << std::endl;
					
					this->setorigin(out);
					this->setborder(out);
					this->setbackground(out);
					
					double ox = -this->width() / 2.0;
					double oy = -this->height() / 2.0;
					double delta1 = (this->width() < this->height()) ? this->width() * adjust_.at(0) : this->height() * adjust_.at(0);
					double delta2 = this->height() * adjust_.at(1);
					
					double x = this->width() / 2.0;
					double y = this->height() - delta1;
					double w = this->width() / 2.0;
					double h = delta1;
					if((adjust_.at(1) < 0.5) && (delta2 < delta1)){
						y = this->height() - delta2;
						h = delta2;
					}else if((adjust_.at(1) >= 0.5) && (this->height() - delta2 < delta1)){
						y = delta2;
						h = this->height() - delta2;
					}
					
					bool status = detail::draw_arc(out, ox + this->width(), oy + y, w, h, 2);
					
					y = this->height() * (1 - adjust_.at(1)) + h;
					out << fmt(LITERAL("%f %f l")) % (ox + x) % (oy + y) << std::endl;
					status &= detail::draw_arc_rev(out, ox, oy + y, w, h, 4, true);
					
					y = this->height() * (1 - adjust_.at(1)) - h;
					status &= detail::draw_arc_rev(out, ox, oy + y, w, h, 1, true);
					
					y = h;
					out << fmt(LITERAL("%f %f l")) % (ox + x) % (oy + y) << std::endl;
					status &= detail::draw_arc(out, ox + this->width(), oy + y, w, h, 3, true);
					out << LITERAL("S") << std::endl;
					if (this->entag_) out << LITERAL("Q") << std::endl;
					
					return status;
				}
				
			private:
				typedef clx::basic_format<CharT, Traits> fmt;
				
				bool entag_;
				container adjust_;
			};
			
			/* ------------------------------------------------------------- */
			//  basic_right_brace
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_right_brace : public basic_shape<CharT, Traits> {
			public:
				typedef basic_shape<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				typedef std::vector<double> container;
				
				explicit basic_right_brace(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h), entag_(entag), adjust_() {
					adjust_.push_back(0.08333);
					adjust_.push_back(0.5);
				}
				
				virtual ~basic_right_brace() throw() {}
				
				virtual size_type has_adjust() const { return 2; }
				virtual double adjust(size_type pos = 0) const { return adjust_.at(pos); }
				virtual void adjust(double value, size_type pos = 0) { adjust_.at(pos) = value; }
				
				/* --------------------------------------------------------- */
				//  main operation.
				/* --------------------------------------------------------- */
				bool operator()(ostream_type& out) {
					// No need to draw and/or fill the shape.
					if (!this->is_stroke()) return true;
					
					if (entag_) out << LITERAL("q") << std::endl;
					
					this->setorigin(out);
					this->setborder(out);
					this->setbackground(out);
					
					double ox = -this->width() / 2.0;
					double oy = -this->height() / 2.0;
					double delta1 = (this->width() < this->height()) ? this->width() * adjust_.at(0) : this->height() * adjust_.at(0);
					double delta2 = this->height() * adjust_.at(1);
					
					double x = this->width() / 2.0;
					double y = this->height() - delta1;
					double w = this->width() / 2.0;
					double h = delta1;
					if((adjust_.at(1) < 0.5) && (delta2 < delta1)){
						y = this->height() - delta2;
						h = delta2;
					}else if((adjust_.at(1) >= 0.5) && (this->height() - delta2 < delta1)){
						y = delta2;
						h = this->height() - delta2;
					}
					
					bool status = detail::draw_arc_rev(out, ox, oy + y, w, h, 1);
					
					y = this->height() * (1 - adjust_.at(1)) + h;
					out << fmt(LITERAL("%f %f l")) % (ox + x) % (oy + y) << std::endl;
					status &= detail::draw_arc(out, ox + this->width(), oy + y, w, h, 3, true);
					
					y = this->height() * (1 - adjust_.at(1)) - h;
					status &= detail::draw_arc(out, ox + this->width(), oy + y, w, h, 2, true);
					
					y = h;
					out << fmt(LITERAL("%f %f l")) % (ox + x) % (oy + y) << std::endl;
					status &= detail::draw_arc_rev(out, ox, oy + y, w, h, 4, true);
					out << LITERAL("S") << std::endl;
					if (this->entag_) out << LITERAL("Q") << std::endl;
					
					return status;
				}
				
			private:
				typedef clx::basic_format<CharT, Traits> fmt;
				
				bool entag_;
				container adjust_;
			};
		}
	}
}

#endif // FAML_PDF_CONTENTS_BRACKET_H
