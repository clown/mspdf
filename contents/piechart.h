#ifndef FAML_PDF_CONTENTS_PIECHART_H
#define FAML_PDF_CONTENTS_PIECHART_H

#include <algorithm>
#include <string>
#include <ostream>
#include <map>
#include "clx/lexical_cast.h"
#include "../coordinate.h"
#include "../color.h"
#include "../code_convert.h"
#include "../font.h"
#include "../text_helper.h"
#include "../text_contents.h"
#include "../axis.h"

namespace faml {
	namespace pdf {
		namespace contents {
			template <
				class DataMap,
				class CharT = char,
				class Traits = std::char_traits<CharT>
			>
			class piechart {
			public:
				typedef size_t size_type;
				typedef CharT char_type;
				typedef std::basic_string<CharT, Traits> string_type;
				typedef std::basic_ostream<CharT, Traits> ostream_type;
				typedef DataMap container;
				typedef axis axis_type;
				typedef std::vector<double> vector_type;
				typedef basic_font<CharT, Traits> font_type;
				typedef std::vector<size_type> palette_type;
				
				piechart() :
					in_(NULL), f_(NULL), fj_(NULL),
					option_(1), position_(0x12),
					scale_(), area_(),
					margin_(10.0), legend_(50.0),
					weight_(0.75), size_(10.0), title_size_(18.0), title_h_(30.0),
					fg_(), bg_(),
					fgplot_(), bgplot_(),
					fgleg_(), bgleg_(),
					txt_(0),
					valax_(), palette_(NULL),
					origin_(), width_(0.0), height_(0.0) {}
				
				explicit piechart(const coordinate& o, double w, double h) :
					in_(NULL), f_(NULL), fj_(NULL),
					option_(1), position_(0x12),
					scale_(), area_(),
					margin_(10.0), legend_(50.0),
					weight_(0.75), size_(10.0), title_size_(18.0), title_h_(30.0),
					fg_(), bg_(),
					fgplot_(), bgplot_(),
					fgleg_(), bgleg_(),
					txt_(0),
					valax_(), palette_(NULL),
					origin_(o), width_(w), height_(h) {}
				
				virtual ~piechart() throw() {}
				
				/* --------------------------------------------------------- */
				//  main operator
				/* --------------------------------------------------------- */
				bool operator()(ostream_type& out) {
					// ¶‰º‚©‚ç‡‚Éo—Í‚·‚éD
					this->xput_background(out);
					
					double ox = 0;
					double oy = 0;
					
					this->put_title(out, ox, oy);
					this->put_legend(out, ox, oy);
					this->put_data(out, ox, oy);
					
					return true;
				}
				
				/* --------------------------------------------------------- */
				//  Access methods (get).
				/* --------------------------------------------------------- */
				const coordinate& origin() const { return origin_; }
				double width() const { return width_; }
				double height() const { return height_; }
				
				/* --------------------------------------------------------- */
				//  Access methods (set).
				/* --------------------------------------------------------- */
				void option(size_type cp) { option_ = cp; }
				void position(size_type cp) { position_ = cp; }
				void origin(const coordinate& cp) { origin_ = cp; }
				void width(double cp) { width_ = cp; }
				void height(double cp) { height_ = cp; }
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
				
				void data(const container& cp) { in_ = &cp; }
				
			private:
				const container* in_;
				const font_type* f_;
				const font_type* fj_;
				size_type option_;
				size_type position_;
				std::pair<int, int> scale_;
				std::pair<double, double> area_;
				double margin_;
				double legend_;
				double weight_;
				double size_;
				double title_size_;
				double title_h_;
				color fg_;
				color bg_;
				color fgplot_;
				color bgplot_;
				color fgleg_;
				color bgleg_;
				color txt_;
				axis_type valax_;
				
				const palette_type* palette_;
				typedef clx::basic_format<CharT, Traits> fmt;
				typedef basic_text_contents<CharT, Traits> text_contents;
				
				coordinate origin_;
				double width_;
				double height_;
				
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
				 */
				/* --------------------------------------------------------- */
				bool put_data(ostream_type& out, double ox, double oy) {
					// total
					if(this->in_->size() <= 0) return false;
					double sum = 0.0;
					for (size_type i = 0; i < this->in_->labels().size(); ++i) {
						sum += this->in_->at(0).at(i);
					}
					
					// rate
					vector_type rates;
					for (size_type i = 0; i < this->in_->labels().size(); ++i) {
						rates.push_back(this->in_->at(0).at(i) / sum);
					}
					
					// plot
					double delta = (this->width() < this->height()) ? (this->width() - legend_ - 3*margin_) / 2.0 : (this->height() - title_h_ - 3*margin_) / 2.0;
					double start_angle = 90.0;
					for (size_type i = 0; i < this->in_->labels().size(); ++i) {
						this->border(detail::getcolor(i, *this->palette_, this->palette_->size() - 6));
						this->background(detail::getcolor(i, *this->palette_, this->palette_->size() - 6));
						double end_angle = start_angle - 360.0 * rates.at(i);
						out << LITERAL("q") << std::endl;
						this->put_origin(out);
						this->put_border(out);
						out << fmt(LITERAL("%f %f %f RG")) % fg_.red() % fg_.green() % fg_.blue() << std::endl;
						out << fmt(LITERAL("%f %f %f rg")) % bg_.red() % bg_.green() % bg_.blue() << std::endl;
						detail::draw_val_arc_rev(out, ox+(this->width()-margin_-legend_)/2.0, oy+margin_+delta, delta, delta, start_angle, end_angle);
						out << fmt(LITERAL("%f %f l")) % (ox+(this->width()-margin_-legend_)/2.0) % (oy+margin_+delta) << std::endl;
						out << LITERAL("b") << std::endl;
						out << LITERAL("Q") << std::endl;
						start_angle = end_angle;
					}
					
					return true;
				}
				
				/* --------------------------------------------------------- */
				/*
				 *  put_legend
				 */
				/* --------------------------------------------------------- */
				bool put_legend(ostream_type& out, double ox, double oy) {
					double height = this->in_->labels().size() * (this->size_ + 5.0);
					double sp = this->height() - margin_ - title_h_ - height;
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
					legend_ = width + 10.0;
					for (size_type i = this->in_->labels().size(); i > 0; --i) {
						rectangle sh(coordinate(this->origin().x() + ox + this->width() - margin_ - legend_,
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
						
						std::basic_string<wchar_t> tmp = babel::utf8_to_unicode(this->in_->labels().at(i - 1));
						string_type s = win32conv(tmp.begin(), tmp.end());
						text_contents t(s, coordinate(this->origin().x() + ox + 10.0 + this->width() - margin_ - legend_,
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
				
				/* --------------------------------------------------------- */
				/*
				 *  put_title
				 */
				/* --------------------------------------------------------- */
				bool put_title(ostream_type& out, double ox, double oy) {
					text_helper h(this->f_->property(), this->f_->charset());
					std::basic_string<wchar_t> tmp = babel::utf8_to_unicode(this->in_->at(0).name());
					string_type s = win32conv(tmp.begin(), tmp.end());
					double w = h.width(s.begin(), s.end()) / 1000.0 * this->title_size_;
					text_contents t(s, coordinate(this->origin().x() + ox + this->width()/2.0 - w/2.0,
						this->origin().y() + oy +this->height() - margin_), *(this->f_), *(this->fj_));
					t.font_size(this->title_size_);
					t.font_color(this->txt_);
					t(out);
					
					return true;
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
			};
		}
	}
}

#endif // FAML_PDF_CONTENTS_PIECHART_H
