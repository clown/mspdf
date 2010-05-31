/* ------------------------------------------------------------------------- */
/*
 *  contents/custom_shape.h
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
 *  Last-modified: Fri 05 Jun 2009 15:13:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PDF_CONTENTS_CUSTOM_SHAPE_H
#define FAML_PDF_CONTENTS_CUSTOM_SHAPE_H

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
			//  basic_custom_shape
			/* ------------------------------------------------------------- */
			template <
				class PathContainer,
				class CharT = char,
				class Traits = std::char_traits<CharT>
			>
			class basic_custom_shape : public basic_shape<CharT, Traits> {
			public:
				typedef basic_shape<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				typedef PathContainer container;
				
				explicit basic_custom_shape(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h), v_(NULL), entag_(entag) {}
				
				virtual ~basic_custom_shape() throw() {}
				
				virtual size_type has_adjust() const { return 0; }
				virtual double adjust(size_type pos = 0) const { return 0.0; }
				virtual void adjust(double value, size_type pos = 0) { return; }
				
				/* --------------------------------------------------------- */
				//  main operation.
				/* --------------------------------------------------------- */
				bool operator()(ostream_type& out) {
					if (v_ == NULL || v_->empty()) return false;
					if (!this->is_fill() && !this->is_stroke()) return true;
					
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
					
					for (size_type i = 0; i < v_->size(); ++i) {
						this->xputpath(out, v_->at(i));
					}
					
					if (entag_) out << LITERAL("Q") << std::endl;
					
					return true;
				}
				
				/* --------------------------------------------------------- */
				//  Access Methods (set).
				/* --------------------------------------------------------- */
				void paths(const container& cp) { v_ = &cp; }
				
			private:
				typedef clx::basic_format<CharT, Traits> fmt;
				
				const container* v_;
				bool entag_;
				
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
						
					for (size_type i = 0; i < v_->size(); ++i) {
						for (size_type j = 0; j < v_->at(i).size(); ++j) {
							switch (v_->at(i).at(j).command()) {
							case 0:
								this->xputmove(out, v_->at(i).at(j), x, -y);
								break;
							case 1:
								this->xputline(out, v_->at(i).at(j), x, -y);
								break;
							case 2:
								this->xputcurve(out, v_->at(i).at(j), x, -y);
								break;
							default:
								break;
							}
						}
						out << LITERAL("f") << std::endl;
					}
					
					return true;
				}
				
				/* --------------------------------------------------------- */
				//  xputpath
				/* --------------------------------------------------------- */
				template <class Container>
				bool xputpath(ostream_type& out, const Container& src, double x = 0, double y = 0) {
					//bool fill = src.is_fill();
					bool fill = this->is_fill();
					bool closed = false;
					for (size_type i = 0; i < src.size(); ++i) {
						if (src.at(i).command() == 3) {
							// close command.
							closed = true;
							break;
						}
						
						switch (src.at(i).command()) {
						case 0:
							this->xputmove(out, src.at(i), x, y);
							break;
						case 1:
							this->xputline(out, src.at(i), x, y);
							break;
						case 2:
							this->xputcurve(out, src.at(i), x, y);
							break;
						default:
							break;
						}
					}
					
					if (closed) {
					//if (true) {
						if(this->is_stroke() && fill) out << LITERAL("b") << std::endl;
						else if (fill) out << LITERAL("f") << std::endl;
						else out << LITERAL("s") << std::endl;
					}
					else {
						if(this->is_stroke() && fill) out << LITERAL("B") << std::endl;
						else if (fill) out << LITERAL("F") << std::endl;
						else out << LITERAL("S") << std::endl;
					}
					
					return true;
				}
				
				/* --------------------------------------------------------- */
				//  xputmove
				/* --------------------------------------------------------- */
				template <class Type>
				bool xputmove(ostream_type& out, const Type& src, double x, double y) {
					if (src.size() < 1) return false;
					
					double ox = -this->width() / 2.0 + x;
					double oy = -this->height() / 2.0 + y;
					out << fmt(LITERAL("%f %f m"))
						% (ox + this->width() * src.at(0).x())
						% (oy + this->height() * (1 - src.at(0).y()))
					<< std::endl;
					
					return true;
				}
				
				/* --------------------------------------------------------- */
				//  xputline
				/* --------------------------------------------------------- */
				template <class Type>
				bool xputline(ostream_type& out, const Type& src, double x, double y) {
					if (src.size() < 1) return false;
					
					double ox = -this->width() / 2.0 + x;
					double oy = -this->height() / 2.0 + y;
					out << fmt(LITERAL("%f %f l"))
						% (ox + this->width() * src.at(0).x())
						% (oy + this->height() * (1 - src.at(0).y()))
					<< std::endl;
					
					return true;
				}
				
				/* --------------------------------------------------------- */
				//  xputcurve
				/* --------------------------------------------------------- */
				template <class Type>
				bool xputcurve(ostream_type& out, const Type& src, double x, double y) {
					if (src.size() < 3) return false;
					
					double ox = -this->width() / 2.0 + x;
					double oy = -this->height() / 2.0 + y;
					out << fmt(LITERAL("%f %f %f %f %f %f c"))
						% (ox + this->width() * src.at(0).x())
						% (oy + this->height() * (1 - src.at(0).y()))
						% (ox + this->width() * src.at(1).x())
						% (oy + this->height() * (1 - src.at(1).y()))
						% (ox + this->width() * src.at(2).x())
						% (oy + this->height() * (1 - src.at(2).y()))
					<< std::endl;
					
					return true;
				}
			};
		}
	}
}

#endif // FAML_PDF_CONTENTS_CUSTOM_SHAPE_H
