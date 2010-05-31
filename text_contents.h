/* ------------------------------------------------------------------------- */
/*
 *  text_contents.h
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
 *  Last-modified: Sun 19 Apr 2009 17:33:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PDF_TEXT_CONTENTS_H
#define FAML_PDF_TEXT_CONTENTS_H

#include <string>
#include "coordinate.h"
#include "color.h"
#include "font.h"
#include "text_helper.h"
#include "clx/format.h"
#include "clx/literal.h"
#include "clx/hexdump.h"
#include "clx/unit.h"

namespace faml {
	namespace pdf {
		/* ----------------------------------------------------------------- */
		//  basic_text_contents
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_text_contents {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef basic_font<CharT, Traits> font_type;
			
			enum {
				none		= 0x0000,
				bold		= 0x0001,
				italic		= 0x0002,
				underline	= 0x0004,
				strike		= 0x0008,
				hyperlink	= 0x0010,
				highlight	= 0x0020,
				ruby		= 0x0040,
				overlap		= 0x0080,
				box			= 0x0100,
				doubled		= 0x1000,
				emboss		= 0x2000
			};
			
			explicit basic_text_contents(const string_type s, const coordinate& origin,
				const font_type& f, bool entag = true) :
				s_(s), ext_(), origin_(origin), font_(&f), fontj_(NULL),
				size_(12.0), extsz_(6.0), color_(), bg_(), space_(0.0), decorate_(0), scale_(100),
				baseline_(0.0), weight_(0.6), angle_(0.3), rot_(0), flip_(0), tag_(entag) {}
			
			explicit basic_text_contents(const string_type s, const coordinate& origin,
				const font_type& f, const font_type& fj, bool entag = true) :
				s_(s), ext_(), origin_(origin), font_(&f), fontj_(&fj),
				size_(12.0), extsz_(6.0), color_(), bg_(), space_(0.0), decorate_(0), scale_(100),
				baseline_(0.0), weight_(0.25), angle_(0.3), rot_(0), flip_(0), tag_(entag) {}
			
			template <class OutStream>
			bool operator()(OutStream& out) {
				if (fontj_ == NULL) return this->xsimple_output(out);
				else return this->xalternative_output(out);
			}
			
			template <class OutStream, class PDFManager>
			bool operator()(OutStream& out, PDFManager& pm) {
				if (fontj_ == NULL) return this->xsimple_output(out);
				else return this->xalternative_output(out);
			}
			
			/* ------------------------------------------------------------- */
			//  Access methods (set).
			/* ------------------------------------------------------------- */
			void ext(const string_type& cp) { ext_ = cp; }
			void font1st(const font_type& cp) { font_ = &cp; }
			void font2nd(const font_type& cp) { fontj_ = &cp; }
			void font_size(double cp) { size_ = cp; }
			void font_color(const color& cp) { color_ = cp; }
			void extsize(double cp) { extsz_ = cp; }
			void background(const color& cp) { bg_ = cp; }
			void space(double cp) { space_ = cp; }
			void decorate(size_type cp) { decorate_ = cp; }
			void scale(size_type cp) { scale_ = cp; }
			void baseline(double cp) { baseline_ = cp; }
			
			void rotation(double cp) {
				while (rot_ < 0.0) rot_ += 360.0;
				while (rot_ > 360.0) rot_ -= 360.0;
				rot_ = cp;
			}
			
			void flip(size_type cp) { flip_ = cp; }
			
		private:
			typedef clx::basic_format<CharT, Traits> fmt;
			
			string_type s_;
			string_type ext_;
			coordinate origin_;
			const font_type* font_;
			const font_type* fontj_;
			double size_;
			double extsz_;
			color color_;
			color bg_;
			double space_;
			size_type decorate_;
			size_type scale_;
			double baseline_;
			double weight_;
			double angle_;
			double rot_;
			size_type flip_;
			bool tag_;
			
			template <class OutStream>
			bool xsimple_output(OutStream& out) {
				static const double delta = 0.0;
				double x = origin_.x();
				text_helper helper(font_->property(), font_->charset());
				size_type n = helper.count(s_.begin(), s_.end());
				double sp = (n > 1) ? space_ / (n - 1) : space_;
				//double sp = space_;
				
				if ((decorate_ & highlight)) {
					double x0 = origin_.x();
					double y0 = origin_.y();
					double y1 = origin_.y() - size_ * 1.1 - 2.0;
					double w = helper.width(s_.begin(), s_.end()) / 1000.0 * size_
						+ sp * helper.count(s_.begin(), s_.end());
					this->xput_highlight(out, x0, y1 + baseline_, w, y0 - y1 + baseline_);
				}
				
				if (tag_) out << LITERAL("BT") << std::endl;
				out << fmt(LITERAL("/%s %d Tf")) % font_->label() % size_ << std::endl;
				out << fmt(LITERAL("%f Tc")) % sp << std::endl;
				out << fmt(LITERAL("%d Tz")) % scale_ << std::endl;
				out << fmt(LITERAL("%f Ts")) % baseline_ << std::endl;
				this->xput_rendering(out);
				if ((decorate_ & emboss)) {
					out << LITERAL("0.8 0.8 0.8 rg") << std::endl;
					out << LITERAL("0.8 0.8 0.8 RG") << std::endl;
				}
				else {
					out << fmt(LITERAL("%f %f %f rg")) % color_.red() % color_.green() % color_.blue() << std::endl;
					out << fmt(LITERAL("%f %f %f RG")) % color_.red() % color_.green() % color_.blue() << std::endl;
				}
				if (rot_ > 0.0 || flip_) {
					double cosx = std::cos(rot_);
					double sinx = std::sin(rot_);
					double fliph = (flip_ & 0x01) ? -1 : 1;
					double flipv = (flip_ & 0x02) ? -1 : 1;
					out << fmt(LITERAL("%f %f %f %f %f %f Tm"))
						% (fliph * cosx) % (fliph * sinx)
						% (flipv * -sinx) % (flipv * cosx)
#if 1
						% origin_.x() % (origin_.y() - size_)
#else
						% origin_.x() % origin_.y()
#endif
					<< std::endl;
				}
				else {
					double angle = (decorate_ & italic) ? angle_ : 0.0;
					out << fmt(LITERAL("1 0 %f 1 %f %f Tm"))
#if 1
						% angle % origin_.x() % (origin_.y() - size_)
#else
						% angle % origin_.x() % origin_.y()
#endif
					<< std::endl;
				}
				out << LITERAL("<");
				string_type s;
				if (font_->property().type() == 1) s = strip(s_, font_->charset());
				else s = s_;
				out << clx::hexdump(s) << LITERAL("> Tj") << std::endl;
				double used = helper.width(s_.begin(), s_.end()) / 1000.0;
				x += used * size_ + space_;
				if ((decorate_ & underline)) {
					//this->xput_line(out, origin_.x() - delta, origin_.y() - size_ * 1.1, x - origin_.x() + delta);
					this->xput_line(out, origin_.x(), origin_.y() - size_ * 1.1 + baseline_, x - origin_.x());
					if ((decorate_ & doubled)) {
						this->xput_line(out, origin_.x() - delta,
							origin_.y() - size_ * 1.1 - 2.0 + baseline_, x - origin_.x() + delta);
					}
				}
				if ((decorate_ & strike)) {
					this->xput_line(out, origin_.x() - delta, origin_.y() - size_ * 0.7 + baseline_, x - origin_.x() + delta);
					if ((decorate_ & doubled)) {
						this->xput_line(out, origin_.x() - delta,
							origin_.y() - size_ * 0.7 + 2.0 + baseline_, x - origin_.x() + delta);
					}
				}
				
				if ((decorate_ & box)) {
					double x0 = origin_.x();
					double x1 = x;
					double y0 = origin_.y();
					double y1 = origin_.y() - size_ * 1.1 - 2.0;
					this->xput_box(out, x0, y1, x1 - x0, y0 - y1);
					if ((decorate_ & doubled)) {
						this->xput_box(out, x0 + 1.0, y1 + 1.0 + baseline_, x1 - x0 - 2.0, y0 - y1 - 2.0 + baseline_);
					}
				}
				
				if (tag_) out << LITERAL("ET") << std::endl;
				
				return true;
			}
			
			template <class OutStream>
			bool xalternative_output(OutStream& out) {
				double x = origin_.x();
				double y = origin_.y() - size_ * 0.9;
				typename string_type::iterator pos = s_.begin();
				text_helper h(font_->property(), font_->charset());
				text_helper hj(fontj_->property(), fontj_->charset());
				
				bool ascii = is_ascii(pos, s_.end(), font_->charset());
				size_type n = h.count(s_.begin(), s_.end());
				double sp = (n > 1) ? space_ / (n - 1) : space_;
				
				if ((decorate_ & highlight)) {
					double x0 = origin_.x();
					double y0 = origin_.y();
					double y1 = origin_.y() - size_ * 1.1;
					double w = is_ascii(s_.begin(), s_.end(), font_->charset())
						? h.width(s_.begin(), s_.end())
						: hj.width(s_.begin(), s_.end());
					w = w / 1000.0 * size_ + sp * h.count(s_.begin(), s_.end());
					this->xput_highlight(out, x0, y1, w, y0 - y1);
				}
				
				if (tag_) out << LITERAL("BT") << std::endl;
				
				if ((decorate_ & ruby) && !ext_.empty()) {
					y = origin_.y() - extsz_ * 0.9;
					double w = is_ascii(s_.begin(), s_.end(), font_->charset())
						? h.width(s_.begin(), s_.end())
						: hj.width(s_.begin(), s_.end());
					w = w / 1000.0 * size_;
					double sw = is_ascii(ext_.begin(), ext_.end(), font_->charset())
						? h.width(ext_.begin(), ext_.end())
						: hj.width(ext_.begin(), ext_.end());
					sw = sw / 1000.0 * extsz_;
					size_type sn = h.count(ext_.begin(), ext_.end());
					double ssp = std::max((w - sw) / (sn + 1), 0.0);
					
					if (ascii) out << fmt(LITERAL("/%s %d Tf")) % font_->label() % extsz_ << std::endl;
					else out << fmt(LITERAL("/%s %d Tf")) % fontj_->label() % extsz_ << std::endl;
					out << fmt(LITERAL("%f Tc")) % ssp << std::endl;
					out << fmt(LITERAL("%f %f %f rg")) % color_.red() % color_.green() % color_.blue() << std::endl;
					out << fmt(LITERAL("1 0 0 1 %f %f Tm")) % (origin_.x() + ssp) % y << std::endl;
					out << LITERAL("<");
					out << clx::hexdump(ext_) << LITERAL("> Tj") << std::endl;
					y -= size_ * 0.9;
				}
				
				while (pos != s_.end()) {
					string_type token = ascii
						? h.substr(pos, s_.end(), ascii)
						: hj.substr(pos, s_.end(), ascii);
					double used = is_ascii(token.begin(), token.end(), font_->charset())
						? h.width(token.begin(), token.end()) / 1000.0
						: hj.width(token.begin(), token.end()) / 1000.0;
					if (ascii) out << fmt(LITERAL("/%s %d Tf")) % font_->label() % size_ << std::endl;
					else out << fmt(LITERAL("/%s %d Tf")) % fontj_->label() % size_ << std::endl;
					out << fmt(LITERAL("%f Tc")) % sp << std::endl;
					out << fmt(LITERAL("%d Tz")) % scale_ << std::endl;
					this->xput_rendering(out);
					out << fmt(LITERAL("%f %f %f rg")) % color_.red() % color_.green() % color_.blue() << std::endl;
					out << fmt(LITERAL("%f %f %f RG")) % color_.red() % color_.green() % color_.blue() << std::endl;
					if (rot_ > 0.0) {
						double cosx = std::cos(rot_);
						double sinx = std::sin(rot_);
						out << fmt(LITERAL("%f %f %f %f %f %f Tm"))
							% cosx % sinx % (-sinx) % cosx % x % y
						<< std::endl;
					}
					else {
						double angle = (decorate_ & italic) ? angle_ : 0.0;
						out << fmt(LITERAL("1 0 %f 1 %f %f Tm")) % angle % x % y << std::endl;
					}
					out << LITERAL("<");
					if (ascii && font_->property().type() == 1) out << clx::hexdump(strip(token, font_->charset()));
					else out << clx::hexdump(token);
					out << LITERAL("> Tj") << std::endl;
					
					x += used * size_ + sp * h.count(token.begin(), token.end());
					ascii = !ascii;
				}
				
				if ((decorate_ & overlap) && !ext_.empty()) {
					out << fmt(LITERAL("/%s %d Tf")) % fontj_->label() % size_ << std::endl;
					out << fmt(LITERAL("1 0 0 1 %f %f Tm")) % origin_.x() % y << std::endl;
					out << LITERAL("<");
					out << clx::hexdump(ext_) << LITERAL("> Tj") << std::endl;
				}
				
				if ((decorate_ & underline)) {
					this->xput_line(out, origin_.x(), origin_.y() - size_ * 1.1, x - origin_.x());
					if ((decorate_ & doubled)) {
						this->xput_line(out, origin_.x(), origin_.y() - size_ * 1.1 - 2.0, x - origin_.x());
					}
				}
				
				if ((decorate_ & strike)) {
					this->xput_line(out, origin_.x(), origin_.y() - size_ * 0.6, x - origin_.x());
					if ((decorate_ & doubled)) {
						this->xput_line(out, origin_.x(), origin_.y() - size_ * 0.6 + 2.0, x - origin_.x());
					}
				}
				
				if ((decorate_ & box)) {
					double x0 = origin_.x();
					double x1 = x;
					double y0 = origin_.y();
					double y1 = origin_.y() - size_ * 1.1;
					this->xput_box(out, x0, y1, x1 - x0, y0 - y1);
					if ((decorate_ & doubled)) {
						this->xput_box(out, x0 + 1.0, y1 + 1.0, x1 - x0 - 2.0, y0 - y1 - 2.0);
					}
				}
				
				if (tag_) out << LITERAL("ET") << std::endl;
				
				return true;
			}
			
			template <class OutStream>
			bool xput_rendering(OutStream& out) {
				if ((decorate_ & bold)) {
					out << LITERAL("2 Tr") << std::endl;
					out << fmt(LITERAL("%f w")) % weight_ << std::endl;
				}
				else if ((decorate_ & emboss)) {
					out << LITERAL("1 Tr") << std::endl;
					out << LITERAL("1 w") << std::endl;
				}
				else {
					out << LITERAL("0 Tr") << std::endl;
					out << LITERAL("1 w") << std::endl;
				}
				return true;
			}
			
			template <class OutStream>
			bool xput_line(OutStream& out, double x, double y, double width) {
				out << LITERAL("ET") << std::endl;
				out << fmt(LITERAL("%f %f %f RG")) % color_.red() % color_.green() % color_.blue() << std::endl;
				out << fmt(LITERAL("%f w")) % (size_ / 20.0) << std::endl;
				out << fmt(LITERAL("%f %f m")) % x % y << std::endl;
				out << fmt(LITERAL("%f %f l")) % (x + width) % y << std::endl;
				out << LITERAL("s") << std::endl;
				if (!tag_) out << LITERAL("BT") << std::endl;
				return true;
			}
			
			template <class OutStream>
			bool xput_box(OutStream& out, double x, double y, double w, double h) {
				out << LITERAL("ET") << std::endl;
				out << fmt(LITERAL("%f %f %f RG")) % color_.red() % color_.green() % color_.blue() << std::endl;
				out << fmt(LITERAL("%f w")) % (size_ / 20.0) << std::endl;
				out << fmt(LITERAL("%f %f %f %f re")) % x % y % w % h << std::endl;
				out << LITERAL("s") << std::endl;
				if (!tag_) out << LITERAL("BT") << std::endl;
				return true;
			}
			
			template <class OutStream>
			bool xput_highlight(OutStream& out, double x, double y, double w, double h) {
				if (!tag_) out << LITERAL("ET") << std::endl;
				out << fmt(LITERAL("%f %f %f rg")) % bg_.red() % bg_.green() % bg_.blue() << std::endl;
				out << fmt(LITERAL("%f %f %f %f re")) % x % y % w % h << std::endl;
				out << LITERAL("f") << std::endl;
				if (!tag_) out << LITERAL("BT") << std::endl;
				return true;
			}
		};
	}
}

#endif // FAML_PDF_TEXT_CONTENTS_H
