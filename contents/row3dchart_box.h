#ifndef FAML_PDF_CONTENTS_ROW3DCHART_BOX_H
#define FAML_PDF_CONTENTS_ROW3DCHART_BOX_H

#include <algorithm>
#include <string>
#include <ostream>
#include <map>
#include "babel/babel.h"
#include "clx/lexical_cast.h"
#include "../coordinate.h"
#include "../color.h"
#include "../code_convert.h"
#include "../font.h"
#include "../text_helper.h"
#include "../text_contents.h"
#include "../axis.h"
#include "polygon.h"

namespace faml {
	namespace pdf {
		namespace contents {
			template <
				class DataMap,
				class CharT = char,
				class Traits = std::char_traits<CharT>
			>
			class row3dchart_box {
			public:
				typedef size_t size_type;
				typedef CharT char_type;
				typedef std::basic_string<CharT, Traits> string_type;
				typedef std::basic_ostream<CharT, Traits> ostream_type;
				typedef DataMap container;
				typedef axis axis_type;
				typedef basic_font<CharT, Traits> font_type;
				typedef std::vector<size_type> palette_type;
				
				row3dchart_box() :
					in_(NULL), f_(NULL), fj_(NULL),
					option_(1), position_(0x12),
					margin_(10.0), axis_(0.0), legend_(0.0),
					weight_(0.75), size_(10.0),
					fg_(), bg_(),
					fgplot_(), bgplot_(),
					fgleg_(), bgleg_(),
					shadow_(0x808080),
					z_margin_w_(10.0), z_margin_h_(5.78),
					valax_(), palette_(NULL),
					origin_(), width_(0.0), height_(0.0) {}
				
				explicit row3dchart_box(const coordinate& o, double w, double h) :
					in_(NULL), f_(NULL), fj_(NULL),
					option_(1), position_(0x12),
					margin_(10.0), axis_(0.0), legend_(0.0),
					weight_(0.75), size_(10.0),
					fg_(), bg_(),
					fgplot_(), bgplot_(),
					fgleg_(), bgleg_(),
					shadow_(0x808080),
					z_margin_w_(10.0), z_margin_h_(5.78),
					valax_(), palette_(NULL),
					origin_(o), width_(w), height_(h) {}
				
				virtual ~row3dchart_box() throw() {}
				
				/* --------------------------------------------------------- */
				//  main operator
				/* --------------------------------------------------------- */
				bool operator()(ostream_type& out) {
					// ¶‰º‚©‚ç‡‚Éo—Í‚·‚éD
					this->xput_background(out);
					
					double ox = margin_;
					double oy = size_ + 2 * margin_;
					this->xput_ylabel(out, ox, oy);
					
					ox += axis_ + margin_;
					oy = margin_;
					this->put_xlabel(out, ox, oy);
					
					oy += size_ + margin_;
					this->xput_plotarea(out, ox, oy);
					this->xput_scale(out, ox, oy);
					this->put_data(out, ox, oy);
					this->xput_axis(out, ox, oy);
					
					ox += area_.first + margin_;
					oy = 0.0;
					this->put_legend(out, ox, oy);
					
					return true;
				}
				
				/* --------------------------------------------------------- */
				//  Access methods (get).
				/* --------------------------------------------------------- */
				void option(size_type cp) { option_ = cp; }
				void position(size_type cp) { position_ = cp; }
				const coordinate& origin() const { return origin_; }
				double width() const { return width_; }
				double height() const { return height_; }
				double weight() const { return weight_; }
				double z_margin_h() const { return z_margin_h_; }
				double z_margin_w() const { return z_margin_w_; }
				const color& shadow() const { return shadow_; }
				
				/* --------------------------------------------------------- */
				//  Access methods (set).
				/* --------------------------------------------------------- */
				void origin(const coordinate& cp) { origin_ = cp; }
				void width(double cp) { width_ = cp; }
				void height(double cp) { height_ = cp; }
				void weight(double cp) { weight_ = cp; }
				void font1st(const font_type& cp) { f_ = &cp; }
				void font2nd(const font_type& cp) { fj_ = &cp; }
				void font_size(const double cp) { size_ = cp; }
				
				void border(const color& cp) { fg_ = cp; }
				void background(const color& cp) { bg_ = cp; }
				void plot_border(const color& cp) { fgplot_ = cp; }
				void plot_area(const color& cp) { bgplot_ = cp; }
				void legend_border(const color& cp) { fgleg_ = cp; }
				void legend_area(const color& cp) { bgleg_ = cp; }
				void value_axis(const axis_type& cp) { valax_ = cp; }
				void palette(const palette_type& cp) { palette_ = &cp; }
				void shadow(const color& cp) { shadow_ = cp; }
				
				void data(const container& cp) {
					in_ = &cp;
					this->xcalc_width();
				}
				
			protected:
				enum { delta = 3 };
				
				const container* in_;
				const font_type* f_;
				const font_type* fj_;
				size_type option_;
				size_type position_;
				std::pair<int, int> scale_;
				std::pair<double, double> area_;
				double margin_;
				double axis_;
				double legend_;
				double weight_;
				double size_;
				color fg_;
				color bg_;
				color fgplot_;
				color bgplot_;
				color fgleg_;
				color bgleg_;
				color txt_;
				color shadow_;
				double z_margin_w_;
				double z_margin_h_;
				axis_type valax_;
				const palette_type* palette_;
				
				/* --------------------------------------------------------- */
				//  put_origin
				/* --------------------------------------------------------- */
				bool put_origin(ostream_type& out) {
					out << fmt(LITERAL("1 0 0 1 %f %f cm"))
						% this->origin().x()
						% this->origin().y()
					<< std::endl;
					
					return true;
				}
				
				/* --------------------------------------------------------- */
				//  put_border
				/* --------------------------------------------------------- */
				bool put_border(ostream_type& out) {
					out << fmt(LITERAL("%f w")) % weight_ << std::endl;
					out << fmt(LITERAL("%f %f %f RG"))
						% fgplot_.red()
						% fgplot_.green()
						% fgplot_.blue()
					<< std::endl;
					
					return true;
				}
				
				/* --------------------------------------------------------- */
				/*
				 *  put_data
				 *
				 *  Sub class implements the method for drawing data.
				 */
				/* --------------------------------------------------------- */
				bool put_data(ostream_type& out, double ox, double oy) {
					size_type n = (this->in_->size() + 1) * this->in_->labels().size();
					//double height = (this->area_.second - this->z_margin_h()) / n;
					double height = (this->area_.second - this->z_margin_h()) / n;
					double upper = this->scale_.first;
					
					// 3D start
					double z_h = height/4.0;
					double z_w = z_h / tan(3.14159265 / 6.0);
					// 3D end
					
					oy += height / 2.0;
					for (size_type i = 0; i < this->in_->labels().size(); ++i) {
						for (size_type j = 0; j < this->in_->size(); ++j) {
							if (static_cast<int>(this->in_->at(j).at(i)) == 0) {
								oy += height;
								continue;
							}
							
							// 3D start
							double width = (this->area_.first - this->z_margin_w()) * (this->in_->at(j).at(i) / upper);
							
							color fg_ = detail::getcolor(j, *this->palette_, this->palette_->size() - 6);
							color bg_ = detail::getcolor(j, *this->palette_, this->palette_->size() - 6);
							out << LITERAL("q") << std::endl;
							this->put_origin(out);
							this->put_border(out);
							
							out << fmt(LITERAL("%f %f %f RG")) % fg_.red() % fg_.green() % fg_.blue() << std::endl;
							out << fmt(LITERAL("%f %f %f rg")) % bg_.red() % bg_.green() % bg_.blue() << std::endl;
							out << fmt(LITERAL("%f %f m")) % (ox+z_w) % (oy) << std::endl;
							out << fmt(LITERAL("%f %f l")) % (ox+z_w) % (oy+height) << std::endl;
							out << fmt(LITERAL("%f %f l")) % (ox+z_w+width) % (oy+height) << std::endl;
							out << fmt(LITERAL("%f %f l")) % (ox+z_w+width) % (oy) << std::endl;
							out << LITERAL("b") << std::endl;
							
							out << fmt(LITERAL("%f %f %f RG")) % (fg_.red()*this->shadow().red()) % (fg_.green()*this->shadow().green()) % (fg_.blue()*this->shadow().blue()) << std::endl;
							out << fmt(LITERAL("%f %f %f rg")) % (bg_.red()*this->shadow().red()) % (bg_.green()*this->shadow().green()) % (bg_.blue()*this->shadow().blue()) << std::endl;
							out << fmt(LITERAL("%f %f m")) % (ox+z_w) % (oy+height) << std::endl;
							out << fmt(LITERAL("%f %f l")) % (ox+2*z_w) % (oy+height+z_h) << std::endl;
							out << fmt(LITERAL("%f %f l")) % (ox+2*z_w+width) % (oy+height+z_h) << std::endl;
							out << fmt(LITERAL("%f %f l")) % (ox+2*z_w+width) % (oy+z_h) << std::endl;
							out << fmt(LITERAL("%f %f l")) % (ox+z_w+width) % (oy) << std::endl;
							out << fmt(LITERAL("%f %f l")) % (ox+z_w+width) % (oy+height) << std::endl;
							out << LITERAL("b") << std::endl;
							
							out << LITERAL("Q") << std::endl;
							// 3D end
							
							oy += height;
						}
						oy += height / 2.0;
						
						out << LITERAL("q") << std::endl;
						this->put_origin(out);
						this->put_border(out);
						out << fmt(LITERAL("%f %f m")) % ox % oy << std::endl;
						out << fmt(LITERAL("%f %f l")) % (ox - delta) % oy << std::endl;
						out << LITERAL("s") << std::endl;
						out << LITERAL("Q") << std::endl;
						
						oy += height / 2.0;
					}
					return true;
				}
				
				/* --------------------------------------------------------- */
				//  xput_xlabel
				/* --------------------------------------------------------- */
				bool put_xlabel(ostream_type& out, double ox, double oy) {
					int first = 0;
					int last = scale_.first;
					int step = scale_.second;
					
					text_helper h(this->f_->property(), this->f_->charset());
					while (first <= last) {
						std::basic_stringstream<CharT, Traits> conv;
						conv << first;
						string_type tmp = conv.str();
						string_type s = win32conv(tmp.begin(), tmp.end());
						double n = h.width(s.begin(), s.end()) / 1000.0 * size_;
						
						text_contents t(s, coordinate(origin_.x() + ox - n / 2.0,
							origin_.y() + oy + size_), *f_, *fj_);
						t.font_size(size_);
						t.font_color(txt_);
						t(out);
						
						first += step;
						ox += (area_.first - z_margin_w_) * (step / static_cast<double>(last));
					}
					
					return true;
				}
				
				/* --------------------------------------------------------- */
				/*
				 *  put_legend
				 *
				 *  Sub class implements the method for drawing legend.
				 */
				/* --------------------------------------------------------- */
				bool put_legend(ostream_type& out, double ox, double oy) {
					double height = this->in_->size() * (this->size_ + 5.0);
					double sp = this->height() - height;
					if (sp < 0.0) sp = 0.0;
					oy += sp / 2.0;
					double y = oy;
					double width = 0.0;
					
					text_helper h(this->f_->property(), this->f_->charset());
					
					// get max text length
					for (size_type i = this->in_->labels().size(); i > 0; --i) {
						std::basic_string<wchar_t> tmp = babel::utf8_to_unicode(this->in_->labels().at(i - 1));
						string_type s = win32conv(tmp.begin(), tmp.end());
						double w = h.width(s.begin(), s.end()) / 1000.0 * this->size_;
						if (width < w) width = w;
					}
					
					// plot
					for (size_type i = 1; i <= this->in_->size(); ++i) {
						rectangle sh(coordinate(this->origin().x() + ox,
							this->origin().y() + y), 7.0, 7.0);
						if (this->palette_) {
							sh.background(detail::getcolor(i - 1, *this->palette_, this->palette_->size() - 6));
							sh.border(detail::getcolor(i - 1, *this->palette_, this->palette_->size() - 6));
						}
						else {
							sh.background(detail::getcolor(i - 1));
							sh.border(color(0x000000));
						}
						sh(out);
						
						std::basic_string<wchar_t> tmp = babel::utf8_to_unicode(this->in_->at(i - 1).name());
						string_type s = win32conv(tmp.begin(), tmp.end());
						double w = h.width(s.begin(), s.end()) / 1000.0 * this->size_;
						if (width < w) width = w;
						text_contents t(s, coordinate(this->origin().x() + ox + 10.0,
							this->origin().y() + y + this->size_), *(this->f_), *(this->fj_));
						t.font_size(this->size_);
						t.font_color(this->txt_);
						t(out);
						
						y += this->size_ + 5.0;
					}
					
					if (!this->palette_) {
						out << LITERAL("q") << std::endl;
						this->put_origin(out);
						out << fmt(LITERAL("%f w")) % this->weight_ << std::endl;
						out << LITERAL("0 0 0 RG") << std::endl;
						out << fmt(LITERAL("%f %f %f %f re"))
							% (ox - 3.0) % (oy - 3.0) % (width + 18.0) % (y - oy) << std::endl;
						out << LITERAL("s") << std::endl;
						out << LITERAL("Q") << std::endl;
					}
					
					return true;
				}
				
			private:
				typedef clx::basic_format<CharT, Traits> fmt;
				typedef basic_text_contents<CharT, Traits> text_contents;
				typedef basic_rectangle<CharT, Traits> rectangle;
				
				coordinate origin_;
				double width_;
				double height_;
				
				/* --------------------------------------------------------- */
				//  xcalc_width
				/* --------------------------------------------------------- */
				bool xcalc_width() {
					// width of label area.
					double n = 0;
					text_helper h(f_->property(), f_->charset());
					babel::init_babel();
					for (size_type i = 0; i < in_->size(); ++i) {
						std::basic_string<wchar_t> tmp = babel::utf8_to_unicode(in_->at(i).name());
						string_type s = win32conv(tmp.begin(), tmp.end());
						double sz = h.width(s.begin(), s.end()) / 1000.0;
						if (n < sz) n = sz;
					}
					legend_ = n * size_ * 0.5 + 10.0;
					
					// calc scale
					this->xcalc_scale();
					
					// width of y-axis
					n = 0;
					for (size_type i = 0; i < in_->labels().size(); ++i) {
						if (n < in_->labels().at(i).size()) n = in_->labels().at(i).size();
					}
					axis_ = n * size_ * 0.5;
					
					// widt/height of plot area.
					area_.first = width_ - legend_ - axis_ - 6 * margin_;
					area_.second = height_ - size_ - 3 * margin_;
					
					return true;
				}
				
				/* --------------------------------------------------------- */
				//  xcalc_scale
				/* --------------------------------------------------------- */
				bool xcalc_scale() {
					int lst[3] = { 1, 2, 5 };
					
					double mx = 0.0;
					for (size_type i = 0; i < in_->size(); ++i) {
						for (size_type j = 0; j < in_->at(i).size(); ++j) {
							if (mx < in_->at(i).at(j)) mx = in_->at(i).at(j);
						}
					}
					mx *= 0.99;
					
					int value = static_cast<int>(mx + 0.5);
					int digit = 1;
					while (1) {
						for (size_type i = 0; i < 3; ++i) {
							int div = lst[i] * digit;
							int n = value / div;
							if (n < 7) {
								scale_.first = (n + 1) * div;
								scale_.second = div;
								return true;
							}
						}
						
						digit *= 10;
					}
					return false;
				}
				
				/* --------------------------------------------------------- */
				//  put_background
				/* --------------------------------------------------------- */
				bool xput_background(ostream_type& out) {
					if (!fg_.is_valid() && bg_.is_valid()) return true;
					
					out << LITERAL("q") << std::endl;
					this->put_origin(out);
					out << fmt(LITERAL("%f w")) % weight_ << std::endl;
					out << fmt(LITERAL("%f %f %f RG"))
						% fg_.red() % fg_.green() % fg_.blue() << std::endl;
					out << fmt(LITERAL("%f %f %f rg"))
						% bg_.red() % bg_.green() % bg_.blue() << std::endl;
					out << fmt(LITERAL("%f %f %f %f re")) % 0 % 0 % width_ % height_ << std::endl;
					if (fg_.is_valid() && bg_.is_valid()) out << LITERAL("b") << std::endl;
					else if (bg_.is_valid()) out << LITERAL("f") << std::endl;
					else out << LITERAL("s") << std::endl;
					out << LITERAL("Q") << std::endl;
					
					return true;
				}
				
				/* --------------------------------------------------------- */
				//  xput_plotarea
				/* --------------------------------------------------------- */
				bool xput_plotarea(ostream_type& out, double ox, double oy) {
					if (!bgplot_.is_valid()) return true;
					
					out << LITERAL("q") << std::endl;
					this->put_origin(out);
					out << fmt(LITERAL("%f w")) % weight_ << std::endl;
					out << fmt(LITERAL("%f %f %f rg"))
						% bgplot_.red() % bgplot_.green() % bgplot_.blue() << std::endl;
					out << fmt(LITERAL("%f %f %f %f re")) % ox % oy % area_.first % area_.second << std::endl;
					out << LITERAL("f") << std::endl;
					out << LITERAL("Q") << std::endl;
					
					return true;
				}
				
				/* --------------------------------------------------------- */
				//  xput_axis
				/* --------------------------------------------------------- */
				bool xput_axis(ostream_type& out, double ox, double oy) {
					out << LITERAL("q") << std::endl;
					this->put_origin(out);
					this->put_border(out);
					out << fmt(LITERAL("%f %f m")) % (ox - delta) % oy << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox + area_.first - z_margin_w_) % oy << std::endl;
					out << fmt(LITERAL("%f %f m")) % ox % (oy - delta) << std::endl;
					out << fmt(LITERAL("%f %f l")) % ox % (oy + area_.second - z_margin_h_) << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox + z_margin_w_) % (oy + area_.second) << std::endl;
					out << LITERAL("S") << std::endl;
					out << LITERAL("Q") << std::endl;
					
					return true;
				}
				
				/* --------------------------------------------------------- */
				//  xput_scale
				/* --------------------------------------------------------- */
				bool xput_scale(ostream_type& out, double ox, double oy) {
					int first = 0;
					int last = scale_.first;
					int step = scale_.second;
					
					out << LITERAL("q") << std::endl;
					this->put_origin(out);
					this->put_border(out);
					while (first <= last) {
						out << fmt(LITERAL("%f %f m")) % ox % (oy - delta) << std::endl;
						out << fmt(LITERAL("%f %f l")) % ox % oy << std::endl;
						out << fmt(LITERAL("%f %f l")) % (ox + z_margin_w_) % (oy + z_margin_h_) << std::endl;
						out << fmt(LITERAL("%f %f l")) % (ox + z_margin_w_) % (oy + area_.second) << std::endl;
						out << fmt(LITERAL("%f %f m")) % (ox + z_margin_w_) % (oy + area_.second) << std::endl;
						out << LITERAL("s") << std::endl;
						first += step;
						ox += (area_.first - z_margin_w_) * (step / static_cast<double>(last));
					}
					out << LITERAL("Q") << std::endl;
					
					return true;
				}
				
				/* --------------------------------------------------------- */
				//  xput_ylabel
				/* --------------------------------------------------------- */
				bool xput_ylabel(ostream_type& out, double ox, double oy) {
					babel::init_babel();
					double height = (this->area_.second - this->z_margin_h()) / this->in_->labels().size();
					oy += height / 2.0;
					text_helper h(this->f_->property(), this->f_->charset());
					for (size_type i = 0; i < this->in_->labels().size(); ++i) {
						std::basic_string<wchar_t> tmp = babel::utf8_to_unicode(this->in_->labels().at(i));
						string_type s = win32conv(tmp.begin(), tmp.end());
						
						text_contents t(s, coordinate(this->origin().x() + ox,
							this->origin().y() + oy + this->size_), *(this->f_), *(this->fj_));
						t.font_size(this->size_);
						t.font_color(this->txt_);
						t(out);
						
						oy += height;
					}
					
					return true;
				}
			};
		}
	}
}

#endif // FAML_PDF_CONTENTS_ROW3DCHART_BOX_H
