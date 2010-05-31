/* ------------------------------------------------------------------------- */
/*
 *  contents/arrow.h
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
 *  Last-modified: Sat 06 Jun 2009 15:29:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PDF_CONTENTS_ARROW_H
#define FAML_PDF_CONTENTS_ARROW_H

#include "polygon.h"

namespace faml {
	namespace pdf {
		namespace contents {
			/* ------------------------------------------------------------- */
			//  basic_right_arrow
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_right_arrow : public basic_polygon<CharT, Traits> {
			public:
				typedef basic_polygon<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				typedef std::vector<double> container;
				
				explicit basic_right_arrow(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h, entag), adjust_() {
					adjust_.push_back(0.5);
					adjust_.push_back(0.5);
				}
				
				virtual ~basic_right_arrow() throw() {}
				
				virtual bool operator()(ostream_type& out) {
					double ox = -this->width() / 2.0;
					double oy = -this->height() / 2.0;
					
					double short_edge = (this->width() < this->height()) ? this->width() : this->height();
					double head = short_edge * adjust_.at(1);
					if(head > this->width()){
						head = this->width();
					}
					this->add(coordinate(ox, oy + this->height() * (0.5 - 0.5 * adjust_.at(0))));
					this->add(coordinate(ox + this->width() - head, oy + this->height() * (0.5 - 0.5 * adjust_.at(0))));
					this->add(coordinate(ox + this->width() - head, oy));
					this->add(coordinate(ox + this->width(), oy + this->height() / 2.0));
					this->add(coordinate(ox + this->width() - head, oy + this->height()));
					this->add(coordinate(ox + this->width() - head, oy + this->height() * (0.5 + 0.5 * adjust_.at(0))));
					this->add(coordinate(ox, oy + this->height() * (0.5 + 0.5 * adjust_.at(0))));
					
					bool status = super::operator()(out);
					
					double degree = clx::degree(this->angle());
					if ((degree >= 45.0 && degree < 135.0) || (degree >= 225.0 && degree < 315.0)) {
						double w = this->width();
						this->width(w * adjust_.at(0) + 7.2);
						
						coordinate o = this->origin();
						o.x(this->origin().x() + w * (0.5 - 0.5 * adjust_.at(0)) - 3.6);
						this->origin(o);
					}
					
					return status;
				}
				
				virtual size_type has_adjust() const { return 2; }
				virtual double adjust(size_type pos = 0) const { return adjust_.at(pos); }
				virtual void adjust(double value, size_type pos = 0) { adjust_.at(pos) = value; }
				
			private:
				container adjust_;
			};
			
			/* ------------------------------------------------------------- */
			//  basic_left_arrow
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_left_arrow : public basic_polygon<CharT, Traits> {
			public:
				typedef basic_polygon<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				typedef std::vector<double> container;
				
				explicit basic_left_arrow(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h, entag), adjust_() {
					adjust_.push_back(0.5);
					adjust_.push_back(0.5);
				}
				
				virtual ~basic_left_arrow() throw() {}
				
				virtual bool operator()(ostream_type& out) {
					double ox = -this->width() / 2.0;
					double oy = -this->height() / 2.0;
					
					double short_edge = (this->width() < this->height()) ? this->width() : this->height();
					double head = short_edge * adjust_.at(1);
					if(head > this->width()){
						head = this->width();
					}
					this->add(coordinate(ox, oy + this->height() / 2.0));
					this->add(coordinate(ox + head, oy));
					this->add(coordinate(ox + head, oy + this->height() * (0.5 - 0.5 * adjust_.at(0))));
					this->add(coordinate(ox + this->width(), oy + this->height() * (0.5 - 0.5 * adjust_.at(0))));
					this->add(coordinate(ox + this->width(), oy + this->height() * (0.5 + 0.5 * adjust_.at(0))));
					this->add(coordinate(ox + head, oy + this->height() * (0.5 + 0.5 * adjust_.at(0))));
					this->add(coordinate(ox + head, oy + this->height()));
					return super::operator()(out);
				}
				
				virtual size_type has_adjust() const { return 2; }
				virtual double adjust(size_type pos = 0) const { return adjust_.at(pos); }
				virtual void adjust(double value, size_type pos = 0) { adjust_.at(pos) = value; }
				
			private:
				container adjust_;
			};
			
			/* ------------------------------------------------------------- */
			//  basic_up_arrow
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_up_arrow : public basic_polygon<CharT, Traits> {
			public:
				typedef basic_polygon<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				typedef std::vector<double> container;
				
				explicit basic_up_arrow(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h, entag), adjust_() {
					adjust_.push_back(0.5);
					adjust_.push_back(0.5);
				}
				
				virtual ~basic_up_arrow() throw() {}
				
				virtual bool operator()(ostream_type& out) {
					double ox = -this->width() / 2.0;
					double oy = -this->height() / 2.0;
					
					double short_edge = (this->width() < this->height()) ? this->width() : this->height();
					double head = short_edge * adjust_.at(1);
					if(head > this->height()){
						head = this->height();
					}
					this->add(coordinate(ox + this->width() / 2.0, oy + this->height()));
					this->add(coordinate(ox, oy + this->height() - head));
					this->add(coordinate(ox + this->width() * (0.5 - 0.5 * adjust_.at(0)), oy + this->height() - head));
					this->add(coordinate(ox + this->width() * (0.5 - 0.5 * adjust_.at(0)), oy));
					this->add(coordinate(ox + this->width() * (0.5 + 0.5 * adjust_.at(0)), oy));
					this->add(coordinate(ox + this->width() * (0.5 + 0.5 * adjust_.at(0)), oy + this->height() - head));
					this->add(coordinate(ox + this->width(), oy + this->height() - head));
					bool status = super::operator()(out);
					
					return status;
				}
				
				virtual size_type has_adjust() const { return 2; }
				virtual double adjust(size_type pos = 0) const { return adjust_.at(pos); }
				virtual void adjust(double value, size_type pos = 0) { adjust_.at(pos) = value; }
				
			private:
				container adjust_;
			};
			
			/* ------------------------------------------------------------- */
			//  basic_down_arrow
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_down_arrow : public basic_polygon<CharT, Traits> {
			public:
				typedef basic_polygon<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				typedef std::vector<double> container;
				
				explicit basic_down_arrow(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h, entag), adjust_() {
					adjust_.push_back(0.5);
					adjust_.push_back(0.5);
				}
				
				virtual ~basic_down_arrow() throw() {}
				
				virtual bool operator()(ostream_type& out) {
					double ox = -this->width() / 2.0;
					double oy = -this->height() / 2.0;
					
					double short_edge = (this->width() < this->height()) ? this->width() : this->height();
					double head = short_edge * adjust_.at(1);
					if(head > this->height()){
						head = this->height();
					}
					this->add(coordinate(ox + this->width() / 2.0, oy));
					this->add(coordinate(ox, oy + head));
					this->add(coordinate(ox + this->width() * (0.5 - 0.5 * adjust_.at(0)), oy + head));
					this->add(coordinate(ox + this->width() * (0.5 - 0.5 * adjust_.at(0)), oy + this->height()));
					this->add(coordinate(ox + this->width() * (0.5 + 0.5 * adjust_.at(0)), oy + this->height()));
					this->add(coordinate(ox + this->width() * (0.5 + 0.5 * adjust_.at(0)), oy + head));
					this->add(coordinate(ox + this->width(), oy + head));
					return super::operator()(out);
				}
				
				virtual size_type has_adjust() const { return 2; }
				virtual double adjust(size_type pos = 0) const { return adjust_.at(pos); }
				virtual void adjust(double value, size_type pos = 0) { adjust_.at(pos) = value; }
				
			private:
				container adjust_;
			};
			
			/* ------------------------------------------------------------- */
			//  basic_left_right_arrow
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_left_right_arrow : public basic_polygon<CharT, Traits> {
			public:
				typedef basic_polygon<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				typedef std::vector<double> container;
				
				explicit basic_left_right_arrow(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h, entag), adjust_() {
					adjust_.push_back(0.5);
					adjust_.push_back(0.5);
				}
				
				virtual ~basic_left_right_arrow() throw() {}
				
				virtual bool operator()(ostream_type& out) {
					double ox = -this->width() / 2.0;
					double oy = -this->height() / 2.0;
					
					double short_edge = (this->width() < this->height()) ? this->width() : this->height();
					double head = short_edge * adjust_.at(1);
					if(head > this->width() / 2.0){
						head = this->width() / 2.0;
					}
					double height_scale = 0.5 * (1 - adjust_.at(0));
					this->add(coordinate(ox, oy + this->height() / 2.0));
					this->add(coordinate(ox + head, oy));
					this->add(coordinate(ox + head, oy + this->height() * height_scale));
					this->add(coordinate(ox + this->width() - head, oy + this->height() * height_scale));
					this->add(coordinate(ox + this->width() - head, oy));
					this->add(coordinate(ox + this->width(), oy + this->height() / 2.0));
					this->add(coordinate(ox + this->width() - head, oy + this->height()));
					this->add(coordinate(ox + this->width() - head, oy + this->height() * (1 - height_scale)));
					this->add(coordinate(ox + head, oy + this->height() * (1 - height_scale)));
					this->add(coordinate(ox + head, oy + this->height()));
					
					return super::operator()(out);
				}
				
				virtual size_type has_adjust() const { return 2; }
				virtual double adjust(size_type pos = 0) const { return adjust_.at(pos); }
				virtual void adjust(double value, size_type pos = 0) { adjust_.at(pos) = value; }
				
			private:
				container adjust_;
			};
			
			/* ------------------------------------------------------------- */
			//  basic_up_down_arrow
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_up_down_arrow : public basic_polygon<CharT, Traits> {
			public:
				typedef basic_polygon<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				typedef std::vector<double> container;
				
				explicit basic_up_down_arrow(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h, entag), adjust_() {
					adjust_.push_back(0.5);
					adjust_.push_back(0.5);
				}
				
				virtual ~basic_up_down_arrow() throw() {}
				
				virtual bool operator()(ostream_type& out) {
					double ox = -this->width() / 2.0;
					double oy = -this->height() / 2.0;
					
					double short_edge = (this->width() < this->height()) ? this->width() : this->height();
					double head = short_edge * adjust_.at(1);
					if(head > this->height() / 2.0){
						head = this->height() / 2.0;
					}
					double width_scale = 0.5 * (1 - adjust_.at(0));
					this->add(coordinate(ox + this->width() / 2.0, oy + this->height()));
					this->add(coordinate(ox, oy + this->height() - head));
					this->add(coordinate(ox + this->width() * width_scale, oy + this->height() - head));
					this->add(coordinate(ox + this->width() * width_scale, oy + head));
					this->add(coordinate(ox, oy + head));
					this->add(coordinate(ox + this->width() / 2.0, oy));
					this->add(coordinate(ox + this->width(), oy + head));
					this->add(coordinate(ox + this->width() * (1 - width_scale), oy + head));
					this->add(coordinate(ox + this->width() * (1 - width_scale), oy + this->height() - head));
					this->add(coordinate(ox + this->width(), oy + this->height() - head));
					
					return super::operator()(out);
				}
				
				virtual size_type has_adjust() const { return 2; }
				virtual double adjust(size_type pos = 0) const { return adjust_.at(pos); }
				virtual void adjust(double value, size_type pos = 0) { adjust_.at(pos) = value; }
				
			private:
				container adjust_;
			};
			
			/* ------------------------------------------------------------- */
			//  basic_quad_arrow
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_quad_arrow : public basic_polygon<CharT, Traits> {
			public:
				typedef basic_polygon<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				typedef std::vector<double> container;
				
				explicit basic_quad_arrow(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h, entag), adjust_() {
					adjust_.push_back(0.225);
					adjust_.push_back(0.225);
					adjust_.push_back(0.225);
				}
				
				virtual ~basic_quad_arrow() throw() {}
				
				virtual bool operator()(ostream_type& out) {
					double ox = -this->width() / 2.0;
					double oy = -this->height() / 2.0;
					
					double short_edge = (this->width() < this->height()) ? this->width() : this->height();
					double head_w_half = short_edge * adjust_.at(1);
					double head_h = short_edge * adjust_.at(2);
					if(head_w_half + head_h > short_edge / 2.0){
						head_h = short_edge / 2.0 - head_w_half;
					}
					double stick_w_half = short_edge / 2.0 * adjust_.at(0);
					if(head_w_half < stick_w_half){
						stick_w_half = head_w_half;
					}
					
					this->add(coordinate(ox + this->width() / 2.0, oy + this->height()));
					this->add(coordinate(ox + this->width() / 2.0 - head_w_half, oy + this->height() - head_h));
					this->add(coordinate(ox + this->width() / 2.0 - stick_w_half, oy + this->height() - head_h));
					this->add(coordinate(ox + this->width() / 2.0 - stick_w_half, oy + this->height() / 2.0 + stick_w_half));
					this->add(coordinate(ox + head_h, oy + this->height() / 2.0 + stick_w_half));
					this->add(coordinate(ox + head_h, oy + this->height() / 2.0 + head_w_half));
					this->add(coordinate(ox, oy + this->height() / 2.0));
					this->add(coordinate(ox + head_h, oy + this->height() / 2.0 - head_w_half));
					this->add(coordinate(ox + head_h, oy + this->height() / 2.0 - stick_w_half));
					this->add(coordinate(ox + this->width() / 2.0 - stick_w_half, oy + this->height() / 2.0 - stick_w_half));
					this->add(coordinate(ox + this->width() / 2.0 - stick_w_half, oy + head_h));
					this->add(coordinate(ox + this->width() / 2.0 - head_w_half, oy + head_h));
					this->add(coordinate(ox + this->width() / 2.0, oy));
					this->add(coordinate(ox + this->width() / 2.0 + head_w_half, oy + head_h));
					this->add(coordinate(ox + this->width() / 2.0 + stick_w_half, oy + head_h));
					this->add(coordinate(ox + this->width() / 2.0 + stick_w_half, oy + this->height() / 2.0 - stick_w_half));
					this->add(coordinate(ox + this->width() - head_h, oy + this->height() / 2.0 - stick_w_half));
					this->add(coordinate(ox + this->width() - head_h, oy + this->height() / 2.0 - head_w_half));
					this->add(coordinate(ox + this->width(), oy + this->height() / 2.0));
					this->add(coordinate(ox + this->width() - head_h, oy + this->height() / 2.0 + head_w_half));
					this->add(coordinate(ox + this->width() - head_h, oy + this->height() / 2.0 + stick_w_half));
					this->add(coordinate(ox + this->width() / 2.0 + stick_w_half, oy + this->height() / 2.0 + stick_w_half));
					this->add(coordinate(ox + this->width() / 2.0 + stick_w_half, oy + this->height() - head_h));
					this->add(coordinate(ox + this->width() / 2.0 + head_w_half, oy + this->height() - head_h));
					
					return super::operator()(out);
				}
				
				virtual size_type has_adjust() const { return 2; }
				virtual double adjust(size_type pos = 0) const { return adjust_.at(pos); }
				virtual void adjust(double value, size_type pos = 0) { adjust_.at(pos) = value; }
				
			private:
				container adjust_;
			};
			
			/* ------------------------------------------------------------- */
			//  basic_left_right_up_arrow
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_left_right_up_arrow : public basic_polygon<CharT, Traits> {
			public:
				typedef basic_polygon<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				typedef std::vector<double> container;
				
				explicit basic_left_right_up_arrow(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h, entag), adjust_() {
					adjust_.push_back(0.25);
					adjust_.push_back(0.25);
					adjust_.push_back(0.25);
				}
				
				virtual ~basic_left_right_up_arrow() throw() {}
				
				virtual bool operator()(ostream_type& out) {
					double ox = -this->width() / 2.0;
					double oy = -this->height() / 2.0;
					
					double short_edge = (this->width() < this->height()) ? this->width() : this->height();
					double head_w_half = short_edge * adjust_.at(1);
					double head_h = short_edge * adjust_.at(2);
					if(head_w_half + head_h > short_edge / 2.0){
						head_h = short_edge / 2.0 - head_w_half;
					}
					double stick_w_half = short_edge / 2.0 * adjust_.at(0);
					if(head_w_half < stick_w_half){
						stick_w_half = head_w_half;
					}
					
					this->add(coordinate(ox + this->width() / 2.0, oy + this->height()));
					this->add(coordinate(ox + this->width() / 2.0 - head_w_half, oy + this->height() - head_h));
					this->add(coordinate(ox + this->width() / 2.0 - stick_w_half, oy + this->height() - head_h));
					this->add(coordinate(ox + this->width() / 2.0 - stick_w_half, oy + head_w_half + stick_w_half));
					this->add(coordinate(ox + head_h, oy + head_w_half + stick_w_half));
					this->add(coordinate(ox + head_h, oy + head_w_half * 2.0));
					this->add(coordinate(ox, oy + head_w_half));
					this->add(coordinate(ox + head_h, oy));
					this->add(coordinate(ox + head_h, oy + head_w_half - stick_w_half));
					this->add(coordinate(ox + this->width() - head_h, oy + head_w_half - stick_w_half));
					this->add(coordinate(ox + this->width() - head_h, oy));
					this->add(coordinate(ox + this->width(), oy + head_w_half));
					this->add(coordinate(ox + this->width() - head_h, oy + head_w_half * 2.0));
					this->add(coordinate(ox + this->width() - head_h, oy + head_w_half + stick_w_half));
					this->add(coordinate(ox + this->width() / 2.0 + stick_w_half, oy + head_w_half + stick_w_half));
					this->add(coordinate(ox + this->width() / 2.0 + stick_w_half, oy + this->height() - head_h));
					this->add(coordinate(ox + this->width() / 2.0 + head_w_half, oy + this->height() - head_h));
					
					return super::operator()(out);
				}
				
				virtual size_type has_adjust() const { return 2; }
				virtual double adjust(size_type pos = 0) const { return adjust_.at(pos); }
				virtual void adjust(double value, size_type pos = 0) { adjust_.at(pos) = value; }
				
			private:
				container adjust_;
			};
			
			/* ------------------------------------------------------------- */
			//  basic_notched_right_arrow
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_notched_right_arrow : public basic_polygon<CharT, Traits> {
			public:
				typedef basic_polygon<CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::ostream_type ostream_type;
				typedef std::vector<double> container;
				
				explicit basic_notched_right_arrow(const coordinate& o, double w, double h, bool entag = true) :
					super(o, w, h, entag), adjust_() {
					adjust_.push_back(0.5);
					adjust_.push_back(0.5);
				}
				
				virtual ~basic_notched_right_arrow() throw() {}
				
				virtual bool operator()(ostream_type& out) {
					double ox = -this->width() / 2.0;
					double oy = -this->height() / 2.0;
					
					double short_edge = (this->width() < this->height()) ? this->width() : this->height();
					double head = short_edge * adjust_.at(1);
					if(head > this->width()){
						head = this->width();
					}
					
					this->add(coordinate(ox + head * adjust_.at(0), oy + this->height() / 2.0));
					this->add(coordinate(ox, oy + this->height() * (0.5 - 0.5 * adjust_.at(0))));
					this->add(coordinate(ox + this->width() - head, oy + this->height() * (0.5 - 0.5 * adjust_.at(0))));
					this->add(coordinate(ox + this->width() - head, oy));
					this->add(coordinate(ox + this->width(), oy + this->height() / 2.0));
					this->add(coordinate(ox + this->width() - head, oy + this->height()));
					this->add(coordinate(ox + this->width() - head, oy + this->height() * (0.5 + 0.5 * adjust_.at(0))));
					this->add(coordinate(ox, oy + this->height() * (0.5 + 0.5 * adjust_.at(0))));
					return super::operator()(out);
				}
				
				virtual size_type has_adjust() const { return 2; }
				virtual double adjust(size_type pos = 0) const { return adjust_.at(pos); }
				virtual void adjust(double value, size_type pos = 0) { adjust_.at(pos) = value; }
				
			private:
				container adjust_;
			};
		}
	}
}

#endif // FAML_PDF_CONTENTS_ARROW_H
