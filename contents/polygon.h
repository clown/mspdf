/* ------------------------------------------------------------------------- */
/*
 *  contents/polygon.h
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
#ifndef FAML_PDF_CONTENTS_POLYGON_H
#define FAML_PDF_CONTENTS_POLYGON_H

#ifndef FAML_PDF_CONTENTS_SHAPE_H
#include "shape.h"
#endif // FAML_PDF_CONTENTS_SHAPE_H

#include <cassert>
#include <cmath>
#include <vector>
#include "clx/format.h"
#include "clx/literal.h"
#include "../unit.h"

namespace faml {
	namespace pdf {
		namespace contents {
			/* ------------------------------------------------------------- */
			//  basic_polygon
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_polygon : public basic_shape<CharT, Traits> {
			public:
				typedef basic_shape<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				typedef std::vector<coordinate> container;
				
				basic_polygon() :
					super(coordinate(0, 0), 0.0, 0.0), entag_(true), v_() {}
				
				explicit basic_polygon(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h), entag_(entag), v_() {}
				
				virtual ~basic_polygon() throw() {}
				
				/* --------------------------------------------------------- */
				//  Access methods (get).
				/* --------------------------------------------------------- */
				bool empty() const { return v_.empty(); }
				size_type size() const { return v_.size(); }
				const coordinate& at(size_type pos) const { return v_.at(pos); }
				const coordinate& operator[](size_type pos) const { return v_[pos]; }
				
				/* --------------------------------------------------------- */
				//  Access methods (set).
				/* --------------------------------------------------------- */
				void assign(const container& cp) { v_.assign(cp.begin(), cp.end()); }
				void add(const coordinate& o) { v_.push_back(o); }
				void clear() { v_.clear(); }
				
				/* --------------------------------------------------------- */
				//  main operation.
				/* --------------------------------------------------------- */
				virtual bool operator()(ostream_type& out) {
					// No need to draw and/or fill the shape.
					if (!this->is_stroke() && !this->is_fill() && !this->is_shading()) return true;
					
					if (entag_) out << LITERAL("q") << std::endl;
					
					this->setorigin(out);
					
					// effects.
					typename super::effect_map::const_iterator pos;
					pos = this->effects().find(LITERAL("shadow"));
					if (pos != this->effects().end()) {
						this->xput_effect(out, pos->second);
					}
					
					// main shape.
					this->setborder(out);
					this->setbackground(out);
					out << fmt(LITERAL("%f %f m")) % v_.at(0).x() % v_.at(0).y() << std::endl;
					for (size_type i = 1; i < v_.size(); ++i) {
						out << fmt(LITERAL("%f %f l")) % v_.at(i).x() % v_.at(i).y() << std::endl;
					}
					this->draw(out, true);
					
					if ((this->type() & 0x010)) this->xput_double(out);
					if (entag_) out << LITERAL("Q") << std::endl;
					
					return true;
				}
				
			private:
				typedef clx::basic_format<CharT, Traits> fmt;
				bool entag_;
				container v_;
				
				/* --------------------------------------------------------- */
				//  xput_effect
				/* --------------------------------------------------------- */
				template <class Type>
				bool xput_effect(ostream_type& out, const Type& src) {
					if (src.fills().empty()) return true;
					
					double angle = src.direction() + clx::degree(this->angle());
					if (angle < 0.0) angle += 360.0;
					else if (angle > 360.0) angle -= 360.0;
					int h = (this->flip() & 0x01) ? -1 : 1;
					int v = (this->flip() & 0x02) ? -1 : 1;
					double x = std::cos(clx::radian(angle)) * src.distance() * h;
					double y = std::sin(clx::radian(angle)) * src.distance() * v;
					
					out << fmt(LITERAL("%f %f %f rg"))
						% src.fill().red() % src.fill().green() % src.fill().blue() << std::endl;
					out << fmt(LITERAL("%f %f m"))
						% (v_.at(0).x() + x) % (v_.at(0).y() - y) << std::endl;
					for (size_type i = 1; i < v_.size(); ++i) {
						out << fmt(LITERAL("%f %f l"))
							% (v_.at(i).x() + x) % (v_.at(i).y() - y) << std::endl;
					}
					out << LITERAL("f") << std::endl;
					
					return true;
				}
				
				/* --------------------------------------------------------- */
				/*
				 *  xput_double
				 *
				 *  二重線の外側を表示．
				 */
				/* --------------------------------------------------------- */
				bool xput_double(ostream_type& out) {
					double weight = this->weight();
					if ((this->type() & 0x020)) weight /= 2.0;
					else if ((this->type() & 0x040)) weight *= 2.0;
					out << fmt(LITERAL("%f w")) % weight << std::endl;
					
					double delta = weight + 2.0; //std::max(4.0, std::max(weight, this->weight()));
					double x = (v_.at(0).x() > 0) ? v_.at(0).x() + delta :
						((v_.at(0).x() < 0) ? v_.at(0).x() - delta : v_.at(0).x());
					double y = (v_.at(0).y() > 0) ? v_.at(0).y() + delta :
						((v_.at(0).y() < 0) ? v_.at(0).y() - delta : v_.at(0).y());
					out << fmt(LITERAL("%f %f m")) % x % y << std::endl;
					
					for (size_type i = 1; i < v_.size(); ++i) {
						x = (v_.at(i).x() > 0) ? v_.at(i).x() + delta :
							((v_.at(i).x() < 0) ? v_.at(i).x() - delta : v_.at(i).x());
						y = (v_.at(i).y() > 0) ? v_.at(i).y() + delta :
							((v_.at(i).y() < 0) ? v_.at(i).y() - delta : v_.at(i).y());
						out << fmt(LITERAL("%f %f l")) % x % y << std::endl;
					}
					
					out << LITERAL("s") << std::endl;
					
					return true;
				}
			};
			
			/* ------------------------------------------------------------- */
			//  basic_triangle
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_triangle : public basic_polygon<CharT, Traits> {
			public:
				typedef basic_polygon<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				
				explicit basic_triangle(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h, entag), adjust_(0.5) {}
				
				virtual ~basic_triangle() throw() {}
				
				virtual bool operator()(ostream_type& out) {
					double ox = -this->width() / 2.0;
					double oy = -this->height() / 2.0;
					
					this->add(coordinate(ox, oy));
					this->add(coordinate(ox + this->width(), oy));
					this->add(coordinate(ox + this->width() * adjust_, oy + this->height()));
					return super::operator()(out);
				}
				
				virtual size_type has_adjust() const { return 1; }
				virtual double adjust(size_type pos = 0) const { return adjust_; }
				virtual void adjust(double value, size_type pos = 0) { adjust_ = value; }
				
			private:
				double adjust_;
			};
			
			/* ------------------------------------------------------------- */
			//  basic_right_triangle
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_right_triangle : public basic_polygon<CharT, Traits> {
			public:
				typedef basic_polygon<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				
				explicit basic_right_triangle(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h, entag) {}
				
				virtual ~basic_right_triangle() throw() {}
				
				virtual bool operator()(ostream_type& out) {
					double ox = -this->width() / 2.0;
					double oy = -this->height() / 2.0;
					
					this->add(coordinate(ox, oy));
					this->add(coordinate(ox + this->width(), oy));
					this->add(coordinate(ox, oy + this->height()));
					return super::operator()(out);
				}
			};
			
			/* ------------------------------------------------------------- */
			//  basic_rectangle
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_rectangle : public basic_polygon<CharT, Traits> {
			public:
				typedef basic_polygon<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				
				explicit basic_rectangle(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h, entag) {}
				
				virtual ~basic_rectangle() throw() {}
				
				virtual bool operator()(ostream_type& out) {
					double ox = -this->width() / 2.0;
					double oy = -this->height() / 2.0;
					
					this->add(coordinate(ox, oy));
					this->add(coordinate(ox + this->width(), oy));
					this->add(coordinate(ox + this->width(), oy + this->height()));
					this->add(coordinate(ox, oy + this->height()));
					return super::operator()(out);
				}
			};
			
			/* ------------------------------------------------------------- */
			//  basic_pentagon
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_pentagon : public basic_polygon<CharT, Traits> {
			public:
				typedef basic_polygon<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				
				explicit basic_pentagon(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h, entag) {}
				
				virtual ~basic_pentagon() throw() {}
				
				virtual bool operator()(ostream_type& out) {
					double ox = -this->width() / 2.0;
					double oy = -this->height() / 2.0;
					double a = sh(4200);
					double b = sh(13341);
					
					this->add(coordinate(ox + this->width() * a, oy));
					this->add(coordinate(ox + this->width() * (1 - a), oy));
					this->add(coordinate(ox + this->width(), oy + this->height() * b));
					this->add(coordinate(ox + this->width() / 2.0, oy + this->height()));
					this->add(coordinate(ox, oy + this->height() * b));
					return super::operator()(out);
				}
			};
			
			/* ------------------------------------------------------------- */
			//  basic_hexagon
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_hexagon : public basic_polygon<CharT, Traits> {
			public:
				typedef basic_polygon<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				
				explicit basic_hexagon(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h, entag), adjust_(0.23485) {}
				
				virtual ~basic_hexagon() throw() {}
				
				virtual bool operator()(ostream_type& out) {
					double ox = -this->width() / 2.0;
					double oy = -this->height() / 2.0;
					double delta = (this->width() < this->height()) ? this->width() * adjust_ : this->height() * adjust_;
					
					this->add(coordinate(ox + delta, oy));
					this->add(coordinate(ox + this->width() - delta, oy));
					this->add(coordinate(ox + this->width(), oy + this->height() / 2.0));
					this->add(coordinate(ox + this->width() - delta, oy + this->height()));
					this->add(coordinate(ox + delta, oy + this->height()));
					this->add(coordinate(ox, oy + this->height() / 2.0));
					return super::operator()(out);
				}
				
				virtual size_type has_adjust() const { return 1; }
				virtual double adjust(size_type pos = 0) const { return adjust_; }
				virtual void adjust(double value, size_type pos = 0) { adjust_ = value; }
				
			private:
				double adjust_;
			};
			
			typedef basic_triangle<char> triangle;
			typedef basic_right_triangle<char> right_triangle;
			typedef basic_rectangle<char> rectangle;
			typedef basic_pentagon<char> pentagon;
			typedef basic_hexagon<char> hexagon;
		}
	}
}

#endif // FAML_PDF_CONTENTS_POLYGON_H
