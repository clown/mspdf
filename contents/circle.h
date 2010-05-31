/* ------------------------------------------------------------------------- */
/*
 *  contents/circle.h
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
#ifndef FAML_PDF_CONTENTS_CIRCLE_H
#define FAML_PDF_CONTENTS_CIRCLE_H

#ifndef FAML_PDF_CONTENTS_SHAPE_H
#include "shape.h"
#endif // FAML_PDF_CONTENTS_SHAPE_H

#include <cassert>
#include <cmath>
#include "clx/format.h"
#include "clx/literal.h"
#include "../color.h"

namespace faml {
	namespace pdf {
		namespace contents {
			namespace detail {
				/* --------------------------------------------------------- */
				/*
				 *  draw_arc
				 *
				 *  Draw the n-th quarter arc.
				 */
				/* --------------------------------------------------------- */
				template <class OutStream>
				bool draw_arc(OutStream& out,
					double x, double y, double w, double h, size_t n,
					bool moved = false) {
					typedef typename OutStream::char_type char_type;
					typedef clx::basic_format<char_type> fmt;
					
					static const double a = 4.0 * (std::sqrt(2.0) - 1.0) / 3.0;
					
					switch (n) {
					case 1:	// 0 <= theta <= 1/4pi
						if (!moved) out << fmt(LITERAL("%f %f m")) % (x + w) % y << std::endl;
						out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (x + w) % (y + a * h)
							% (x + a * w) % (y + h)
							% x % (y + h) << std::endl;
						break;
					case 2:	// 1/4pi <= theta <= 1/2pi
						if (!moved) out << fmt(LITERAL("%f %f m")) % x % (y + h) << std::endl;
						out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (x - a * w) % (y + h)
							% (x - w) % (y + a * h)
							% (x - w) % y << std::endl;
						break;
					case 3:	// 1/2pi <= theta <= 3/4pi
						if (!moved) out << fmt(LITERAL("%f %f m")) % (x - w) % y << std::endl;
						out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (x - w) % (y - a * h)
							% (x - a * w) % (y - h)
							% x % (y - h) << std::endl;
						break;
					case 4:	// 3/4pi <= theta <= pi
						if (!moved) out << fmt(LITERAL("%f %f m")) % x % (y - h) << std::endl;
						out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (x + a * w) % (y - h)
							% (x + w) % (y - a * h)
							% (x + w) % y << std::endl;
						break;
					default:
						assert(1 <= n && n <= 4);
						return false;
						break;
					}
					
					return true;
				}
				
				/* --------------------------------------------------------- */
				/*
				 *  draw_arc_rev
				 *
				 *  Draw the n-th quarter arc.
				 */
				/* --------------------------------------------------------- */
				template <class OutStream>
				bool draw_arc_rev(OutStream& out,
					double x, double y, double w, double h, size_t n,
					bool moved = false) {
					typedef typename OutStream::char_type char_type;
					typedef clx::basic_format<char_type> fmt;
					
					static const double a = 4.0 * (std::sqrt(2.0) - 1.0) / 3.0;
					
					switch (n) {
					case 1:	// 0 <= theta <= 1/4pi
						if (!moved) out << fmt(LITERAL("%f %f m")) % x % (y + h) << std::endl;
						out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (x + a * w) % (y + h)
							% (x + w) % (y + a * h)
							% (x + w) % y << std::endl;
						break;
					case 2:	// 1/4pi <= theta <= 1/2pi
						if (!moved) out << fmt(LITERAL("%f %f m")) % (x - w) % y << std::endl;
						out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (x - w) % (y + a * h)
							% (x - a * w) % (y + h)
							% x % (y + h) << std::endl;
						break;
					case 3:	// 1/2pi <= theta <= 3/4pi
						if (!moved) out << fmt(LITERAL("%f %f m")) % x % (y - h) << std::endl;
						out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (x - a * w) % (y - h)
							% (x - w) % (y - a * h)
							% (x - w) % y << std::endl;
						break;
					case 4:	// 3/4pi <= theta <= pi
						if (!moved) out << fmt(LITERAL("%f %f m")) % (x + w) % y << std::endl;
						out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (x + w) % (y - a * h)
							% (x + a * w) % (y - h)
							% x % (y - h) << std::endl;
						break;
					default:
						assert(1 <= n && n <= 4);
						return false;
						break;
					}
					
					return true;
				}
				
				/* --------------------------------------------------------- */
				/*
				 *  draw__val_arc
				 *
				 *  Draw the variable arc.
				 */
				/* --------------------------------------------------------- */
				template <class OutStream>
				bool draw_val_arc(OutStream& out,
					double x, double y, double w, double h,
					double start_angle, double end_angle, bool moved = false) {
					typedef typename OutStream::char_type char_type;
					typedef clx::basic_format<char_type> fmt;
					
					while(start_angle < 0.0){start_angle += 360.0;}
					while(start_angle > 360.0){start_angle -= 360.0;}
					while(end_angle < 0.0){end_angle += 360.0;}
					while(end_angle > 360.0){end_angle -= 360.0;}
					
					while(start_angle > end_angle){end_angle += 360.0;}
					
					static const double PI = 3.1415926535;
					double start_rad = start_angle * (PI / 180.0);
					double end_rad = end_angle * (PI / 180.0);
					double a;
					
					if (!moved) out << fmt(LITERAL("%f %f m")) % (x + cos(start_rad)*w) % (y + sin(start_rad)*h) << std::endl;
					
					double cur_rad = 0.0;
					while(cur_rad <= start_rad){cur_rad += PI / 2.0;}
					if(cur_rad >= end_rad){
						a = 4.0 * tan((end_rad - start_rad) / 4.0) / 3.0;
						out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (x + (cos(start_rad) - a*sin(start_rad))*w) % (y + (sin(start_rad) + a*cos(start_rad))*h)
							% (x + (cos(end_rad) + a*sin(end_rad))*w) % (y + (sin(end_rad) - a*cos(end_rad))*h)
							% (x + cos(end_rad)*w) % (y + sin(end_rad)*h) << std::endl;
					}else{
						a = 4.0 * tan((cur_rad - start_rad) / 4.0) / 3.0;
						out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (x + (cos(start_rad) - a*sin(start_rad))*w) % (y + (sin(start_rad) + a*cos(start_rad))*h)
							% (x + (cos(cur_rad) + a*sin(cur_rad))*w) % (y + (sin(cur_rad) - a*cos(cur_rad))*h)
							% (x + cos(cur_rad)*w) % (y + sin(cur_rad)*h) << std::endl;
						
						while(cur_rad + PI/2.0 <= end_rad){
							a = 4.0 * tan(PI / 8.0) / 3.0;
							out << fmt(LITERAL("%f %f %f %f %f %f c"))
								% (x + (cos(cur_rad) - a*sin(cur_rad))*w) % (y + (sin(cur_rad) + a*cos(cur_rad))*h)
								% (x + (cos(cur_rad + PI/2.0) + a*sin(cur_rad + PI/2.0))*w) % (y + (sin(cur_rad + PI/2.0) - a*cos(cur_rad + PI/2.0))*h)
								% (x + cos(cur_rad + PI/2.0)*w) % (y + sin(cur_rad + PI/2.0)*h) << std::endl;
							cur_rad += PI/ 2.0;
						}
						
						a = 4.0 * tan((end_rad - cur_rad) / 4.0) / 3.0;
						out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (x + (cos(cur_rad) - a*sin(cur_rad))*w) % (y + (sin(cur_rad) + a*cos(cur_rad))*h)
							% (x + (cos(end_rad) + a*sin(end_rad))*w) % (y + (sin(end_rad) - a*cos(end_rad))*h)
							% (x + cos(end_rad)*w) % (y + sin(end_rad)*h) << std::endl;
					}
					
					return true;
				}
				
				/* --------------------------------------------------------- */
				/*
				 *  draw__val_arc_rev
				 *
				 *  Draw the variable arc.
				 */
				/* --------------------------------------------------------- */
				template <class OutStream>
				bool draw_val_arc_rev(OutStream& out,
					double x, double y, double w, double h,
					double start_angle, double end_angle, bool moved = false) {
					typedef typename OutStream::char_type char_type;
					typedef clx::basic_format<char_type> fmt;
					
					while(start_angle < 0.0){start_angle += 360.0;}
					while(start_angle > 360.0){start_angle -= 360.0;}
					while(end_angle < 0.0){end_angle += 360.0;}
					while(end_angle > 360.0){end_angle -= 360.0;}
					
					while(start_angle < end_angle){start_angle += 360.0;}
					
					static const double PI = 3.1415926535;
					double start_rad = start_angle * (PI / 180.0);
					double end_rad = end_angle * (PI / 180.0);
					double a;
					
					if (!moved) out << fmt(LITERAL("%f %f m")) % (x + cos(start_rad)*w) % (y + sin(start_rad)*h) << std::endl;
					
					double cur_rad = 0.0;
					while(cur_rad <= start_rad){cur_rad += PI / 2.0;}
					cur_rad -= PI / 2.0;
					if(cur_rad <= end_rad){
						a = 4.0 * tan((start_rad - end_rad) / 4.0) / 3.0;
						out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (x + (cos(start_rad) + a*sin(start_rad))*w) % (y + (sin(start_rad) - a*cos(start_rad))*h)
							% (x + (cos(end_rad) - a*sin(end_rad))*w) % (y + (sin(end_rad) + a*cos(end_rad))*h)
							% (x + cos(end_rad)*w) % (y + sin(end_rad)*h) << std::endl;
					}else{
						a = 4.0 * tan((start_rad - cur_rad) / 4.0) / 3.0;
						out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (x + (cos(start_rad) + a*sin(start_rad))*w) % (y + (sin(start_rad) - a*cos(start_rad))*h)
							% (x + (cos(cur_rad) - a*sin(cur_rad))*w) % (y + (sin(cur_rad) + a*cos(cur_rad))*h)
							% (x + cos(cur_rad)*w) % (y + sin(cur_rad)*h) << std::endl;
						
						while(cur_rad - PI/2.0 >= end_rad){
							a = 4.0 * tan(PI / 8.0) / 3.0;
							out << fmt(LITERAL("%f %f %f %f %f %f c"))
								% (x + (cos(cur_rad) + a*sin(cur_rad))*w) % (y + (sin(cur_rad) - a*cos(cur_rad))*h)
								% (x + (cos(cur_rad - PI/2.0) - a*sin(cur_rad - PI/2.0))*w) % (y + (sin(cur_rad - PI/2.0) + a*cos(cur_rad - PI/2.0))*h)
								% (x + cos(cur_rad - PI/2.0)*w) % (y + sin(cur_rad - PI/2.0)*h) << std::endl;
							cur_rad -= PI/ 2.0;
						}
						
						a = 4.0 * tan((cur_rad - end_rad) / 4.0) / 3.0;
						out << fmt(LITERAL("%f %f %f %f %f %f c"))
							% (x + (cos(cur_rad) + a*sin(cur_rad))*w) % (y + (sin(cur_rad) - a*cos(cur_rad))*h)
							% (x + (cos(end_rad) - a*sin(end_rad))*w) % (y + (sin(end_rad) + a*cos(end_rad))*h)
							% (x + cos(end_rad)*w) % (y + sin(end_rad)*h) << std::endl;
					}
					
					return true;
				}
				
				/* --------------------------------------------------------- */
				/*
				 *  draw_angle_line
				 *
				 *  Draw the variable 3d pie.
				 */
				/* --------------------------------------------------------- */
				template <class OutStream>
				bool draw_angle_line(OutStream& out,
					double x, double y, double w, double h, double d, double angle, bool moved = false) {
					typedef typename OutStream::char_type char_type;
					typedef clx::basic_format<char_type> fmt;
					
					while(angle < 0.0){angle += 360.0;}
					while(angle > 360.0){angle -= 360.0;}
					
					static const double PI = 3.1415926535;
					double rad = angle * (PI / 180.0);
					
					if (!moved) out << fmt(LITERAL("%f %f m")) % (x + cos(rad)*w) % (y + sin(rad)*h) << std::endl;
					
					out << fmt(LITERAL("%f %f l")) % (x + cos(rad)*w) % (y + sin(rad)*h - d) << std::endl;
					
					return true;
				}
				
				/* --------------------------------------------------------- */
				//  draw_ellipse
				/* --------------------------------------------------------- */
				template <class OutStream>
				bool draw_ellipse(OutStream& out, double x, double y, double w, double h) {
					// does not moved the starting point.
					draw_arc(out, x, y, w, h, 1);
					draw_arc(out, x, y, w, h, 2, true);
					draw_arc(out, x, y, w, h, 3, true);
					draw_arc(out, x, y, w, h, 4, true);
					return true;
				}
			}
			
			/* ------------------------------------------------------------- */
			//  basic_ellipse
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_ellipse : public basic_shape<CharT, Traits> {
			public:
				typedef basic_shape<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				
				explicit basic_ellipse(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h), entag_(entag) {}
				
				virtual ~basic_ellipse() throw() {}
				
				/* --------------------------------------------------------- */
				//  main operation.
				/* --------------------------------------------------------- */
				bool operator()(ostream_type& out) {
					// No need to draw and/or fill the shape.
					if (!this->is_stroke() && !this->is_fill()) return true;
					
					if (entag_) out << LITERAL("q") << std::endl;
					
					this->setorigin(out);
					this->setborder(out);
					this->setbackground(out);
					
					double ox = -this->width() / 2.0;
					double oy = -this->height() / 2.0;
					double x = this->width() / 2.0;
					double y = this->height() / 2.0;
					double w = this->width() / 2.0;
					double h = this->height() / 2.0;
					
					bool status = detail::draw_ellipse(out, ox + x, oy + y, w, h);
					if (this->is_stroke() && this->is_fill()) out << LITERAL("b") << std::endl;
					else if (this->is_fill()) out << LITERAL("f") << std::endl;
					else out << LITERAL("s") << std::endl;
					if (this->entag_) out << LITERAL("Q") << std::endl;
					
					return status;
				}
				
			private:
				typedef clx::basic_format<CharT, Traits> fmt;
				
				bool entag_;
			};
			
			/* ------------------------------------------------------------- */
			//  basic_circle
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_circle : public basic_ellipse<CharT, Traits> {
				typedef basic_ellipse<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				
				explicit basic_circle(const coordinate& o, double r, bool entag = true) :
					super(o, r, r, entag) {}
				
				virtual ~basic_circle() throw() {}
			};
			
			typedef basic_ellipse<char> ellipse;
			typedef basic_circle<char> circle;
		}
	}
}

#endif // FAML_PDF_CONTENTS_CIRCLE_H
