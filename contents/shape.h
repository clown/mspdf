/* ------------------------------------------------------------------------- */
/*
 *  contents/shape.h
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
 *  Last-modified: Thu 11 Jun 2009 09:58:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PDF_CONTENTS_SHAPE_H
#define FAML_PDF_CONTENTS_SHAPE_H

#include <cmath>
#include <ostream>
#include <string>
#include "../coordinate.h"
#include "../color.h"
#include "../shape_effect.h"
#include "clx/format.h"
#include "clx/literal.h"

namespace faml {
	namespace pdf {
		namespace contents {
			/* ------------------------------------------------------------- */
			//  basic_shape
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_shape {
			public:
				typedef size_t size_type;
				typedef std::basic_ostream<CharT, Traits> ostream_type;
				typedef std::basic_string<CharT, Traits> string_type;
				typedef faml::basic_shape_effect<CharT, Traits> effect_type;
				typedef std::map<string_type, effect_type> effect_map;
				
				basic_shape() :
					origin_(coordinate(0.0, 0.0)),
					width_(0.0), height_(0.0), angle_(0.0), flip_(0), weight_(0.5),
					type_(1), shading_(), alpha_(), border_(), background_(),
					shadow_(0x808080), highlight_(0xcbcbcb), effects_() {}
				
				explicit basic_shape(const coordinate& o, double w = 0.0, double h = 0.0) :
					origin_(o), width_(w), height_(h), angle_(0.0), flip_(0), weight_(0.5),
					type_(1), shading_(), alpha_(), border_(), background_(),
					shadow_(0x808080), highlight_(0xcbcbcb), effects_() {}
				
				virtual ~basic_shape() throw() {}
				virtual bool operator()(ostream_type& out) = 0;
				
				/* --------------------------------------------------------- */
				/*
				 *  Adjustment values
				 *
				 *  The values are used when adjusting the shape
				 *  of objects. The range of each value is from 0 to 1.
				 */
				/* --------------------------------------------------------- */
				virtual size_type has_adjust() const { return 0; }
				virtual double adjust(size_type pos = 0) const { return 0.0; }
				virtual void adjust(double value, size_type pos = 0) {}
				
				/* --------------------------------------------------------- */
				//  Access methods (get).
				/* --------------------------------------------------------- */
				double width() const { return width_; }
				double height() const { return height_; }
				double angle() const { return angle_; }
				size_type flip() const { return flip_; }
				double weight() const { return weight_; }
				const coordinate& origin() const { return origin_; }
				size_type type() const { return type_; }
				const string_type& shading() const { return shading_; }
				const string_type& alpha() const { return alpha_; }
				const color& border() const { return border_; }
				const color& background() const { return background_; }
				const color& shadow() const { return shadow_; }
				const color& highlight() const { return highlight_; }
				const effect_map& effects() const { return effects_; }
				effect_map& effects() { return effects_; }
				
				/* --------------------------------------------------------- */
				//  Access methods (set).
				/* --------------------------------------------------------- */
				void origin(const coordinate& cp) { origin_ = cp; }
				void width(double cp) { width_ = cp; }
				void height(double cp) { height_ = cp; }
				void angle(double cp) { angle_ = cp; }
				void flip(size_type cp) { flip_ = cp; }
				void weight(double cp) { weight_ = cp; }
				void type(size_type cp) { type_ = cp; }
				void shading(const string_type& cp) { shading_ = cp; }
				void alpha(const string_type& cp) { alpha_ = cp; }
				void border(const color& cp) { border_ = cp; }
				void background(const color& cp) { background_ = cp; }
				void shadow(const color& cp) { shadow_ = cp; }
				void highlight(const color& cp) { highlight_ = cp; }
				void effect(const string_type& name, const effect_type& cp) { effects_[name] = cp; }
				
			protected:
				bool is_fill() const { return background_.is_valid(); }
				bool is_stroke() const { return border_.is_valid(); }
				bool is_shading() const { return (!shading_.empty()); }
				bool is_alpha() const { return (!alpha_.empty()); }
				
				/* --------------------------------------------------------- */
				/*
				 *  setorigin
				 *
				 *  Set the starting coordinate of the drawing shape.
				 *  This method is for future extension to rotate the
				 *  shape.
				 */
				/* --------------------------------------------------------- */
				void setorigin(ostream_type& out) {
					static const double pi = 3.141592653589793;
					out << fmt(LITERAL("1 0 0 1 %f %f cm"))
						% (this->origin().x() + width_ / 2.0)
						% (this->origin().y() + height_ / 2.0)
					<< std::endl;
					
					if (flip_ != 0) {
						int h = (flip_ & 0x01) ? -1 : 1;
						int v = (flip_ & 0x02) ? -1 : 1;
						out << fmt(LITERAL("%d 0 0 %d 0 0 cm")) % h % v << std::endl;
					}
					
					if (angle_ > 0.0) {
						double x = 2.0 * pi - angle_;
						out << fmt(LITERAL("%f %f %f %f 0 0 cm"))
							% std::cos(x) % std::sin(x)
							% (-std::sin(x)) % std::cos(x)
						<< std::endl;
					}
					
				}
				
				/* --------------------------------------------------------- */
				//  setborder
				/* --------------------------------------------------------- */
				void setborder(ostream_type& out) {
					double w = this->weight();
					if (this->is_shading()) w += 1.0;
					out << fmt(LITERAL("%f w")) % w << std::endl;
					switch (type_) {
					case 0x001:
						out << LITERAL("[] 0 d") << std::endl;
						break;
					case 0x002:
						out << fmt(LITERAL("[%f %f] 0 d"))
							% (w * 5) % (w * 3) << std::endl;
						break;
					case 0x003:
						out << fmt(LITERAL("[%f %f] 0 d"))
							% (w * 1) % (w * 1) << std::endl;
						break;
					case 0x004:
						out << fmt(LITERAL("[%f %f] 0 d"))
							% (w * 1) % (w * 1) << std::endl;
						break;
					case 0x005:
						out << fmt(LITERAL("[%f %f %f %f] 0 d"))
							% (w * 5) % (w * 2) % (w * 2) % (w * 2) << std::endl;
						break;
					case 0x006:
						out << fmt(LITERAL("[%f %f %f %f %f %f] 2 d"))
							% (w * 5) % (w * 2) % (w * 2) % (w * 2) % (w * 2) % (w * 2) << std::endl;
						break;
					default:
						out << LITERAL("[] 0 d") << std::endl;
						break;
					}
					
					out << fmt(LITERAL("%f %f %f RG"))
						% this->border().red()
						% this->border().green()
						% this->border().blue()
					<< std::endl;
				}
				
				/* --------------------------------------------------------- */
				//  setbackground
				/* --------------------------------------------------------- */
				void setbackground(ostream_type& out) {
					out << fmt(LITERAL("%f %f %f rg"))
						% this->background().red()
						% this->background().green()
						% this->background().blue()
					<< std::endl;
					if (this->is_alpha()) {
						out << fmt(LITERAL("/%s gs")) % this->alpha() << std::endl;
					}
				}
				
				/* --------------------------------------------------------- */
				//  draw
				/* --------------------------------------------------------- */
				void draw(ostream_type& out, bool close = false) {
					/*if (this->is_alpha()) {
						out << fmt(LITERAL("/%s gs")) % this->alpha() << std::endl;
						if (this->is_stroke()) {
							if (close) out << LITERAL("b") << std::endl;
							else out << LITERAL("B") << std::endl;
						}else{
							if (close) out << LITERAL("f") << std::endl;
							else out << LITERAL("F") << std::endl;
						}
					}*/
					if (this->is_shading()) {
						string_type ln(LITERAL("n"));
						if (this->is_stroke()) {
							if (close) ln = LITERAL("s");
							else ln = LITERAL("S");
						}
						out << fmt(LITERAL("W %s")) % ln << std::endl;
						out << LITERAL("q") << std::endl;
						out << fmt(LITERAL("%f 0 0 %f %f %f cm"))
							% this->width() % this->height()
							% (-this->width() / 2.0)
							% (-this->height() / 2.0)
							<< std::endl;
						out << fmt(LITERAL("/%s sh")) % this->shading() << std::endl;
						out << LITERAL("Q") << std::endl;
					}
					else if (this->is_stroke() && this->is_fill()) {
						if (close) out << LITERAL("b") << std::endl;
						else out << LITERAL("B") << std::endl;
					}
					else if (this->is_fill()) {
						if (close) out << LITERAL("f") << std::endl;
						else out << LITERAL("F") << std::endl;
					}
					else {
						if (close) out << LITERAL("s") << std::endl;
						else out << LITERAL("S") << std::endl;
					}
				}
				
			private:
				typedef clx::basic_format<CharT, Traits> fmt;
				
				coordinate origin_;
				double width_;
				double height_;
				double angle_;
				size_type flip_;
				double weight_;
				size_type type_;
				string_type shading_;
				string_type alpha_;
				color border_;
				color background_;
				color shadow_;
				color highlight_;
				effect_map effects_;
			};
		}
	}
}

#endif // FAML_PDF_SHAPE_CONTENTS_H
