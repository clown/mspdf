#ifndef FAML_PDF_CONTENTS_CHART3D_H
#define FAML_PDF_CONTENTS_CHART3D_H

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
#include "polygon.h"

namespace faml {
	namespace pdf {
		namespace contents {
			template <
				class DataMap,
				class CharT = char,
				class Traits = std::char_traits<CharT>
			>
			class chart3d {
			public:
				typedef size_t size_type;
				typedef CharT char_type;
				typedef std::basic_string<CharT, Traits> string_type;
				typedef std::basic_ostream<CharT, Traits> ostream_type;
				typedef DataMap container;
				typedef axis axis_type;
				typedef basic_font<CharT, Traits> font_type;
				typedef std::vector<size_type> palette_type;
				
				chart3d() :
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
				
				explicit chart3d(const coordinate& o, double w, double h) :
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
				
				virtual ~chart3d() throw() {}
				
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
				virtual bool put_data(ostream_type& out, double ox, double oy) {
					return true;
				}
				
				/* --------------------------------------------------------- */
				//  xput_xlabel
				/* --------------------------------------------------------- */
				virtual bool put_xlabel(ostream_type& out, double ox, double oy) {
					return true;
				}
				
				/* --------------------------------------------------------- */
				/*
				 *  put_legend
				 *
				 *  Sub class implements the method for drawing legend.
				 */
				/* --------------------------------------------------------- */
				virtual bool put_legend(ostream_type& out, double ox, double oy) {
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
					size_type n = 0;
					for (size_type i = 0; i < in_->size(); ++i) {
						if (n < in_->at(i).name().size()) n = in_->at(i).name().size();
					}
					legend_ = n * size_ * 0.5 + 10.0;
					
					// calc scale
					this->xcalc_scale();
					
					// width of y-axis
					string_type s = clx::lexical_cast<string_type>(scale_.first);
					axis_ = s.size() * size_ * 0.5;
					
					// widt/height of plot area.
					area_.first = width_ - legend_ - axis_ - 4 * margin_;
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
							if (n < 10) {
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
					out << fmt(LITERAL("%f %f l")) % (ox + area_.first) % (oy + z_margin_h_) << std::endl;
					out << fmt(LITERAL("%f %f m")) % ox % (oy - delta) << std::endl;
					out << fmt(LITERAL("%f %f l")) % ox % (oy + area_.second - z_margin_h_) << std::endl;
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
						out << fmt(LITERAL("%f %f m")) % (ox - delta) % oy << std::endl;
						out << fmt(LITERAL("%f %f l")) % (ox) % oy << std::endl;
						out << fmt(LITERAL("%f %f l")) % (ox + z_margin_w_) % (oy + z_margin_h_) << std::endl;
						out << fmt(LITERAL("%f %f l")) % (ox + area_.first) % (oy + z_margin_h_) << std::endl;
						out << fmt(LITERAL("%f %f m")) % (ox + area_.first) % (oy + z_margin_h_) << std::endl;
						out << LITERAL("s") << std::endl;
						first += step;
						oy += (area_.second - z_margin_h_) * (step / static_cast<double>(last));
					}
					out << LITERAL("Q") << std::endl;
					
					return true;
				}
				
				/* --------------------------------------------------------- */
				//  xput_ylabel
				/* --------------------------------------------------------- */
				bool xput_ylabel(ostream_type& out, double ox, double oy) {
					int first = 0;
					int last = scale_.first;
					int step = scale_.second;
					
					text_helper h(f_->property(), f_->charset());
					while (first <= last) {
						std::basic_stringstream<CharT, Traits> conv;
						conv << first;
						string_type tmp = conv.str();
						string_type s = win32conv(tmp.begin(), tmp.end());
						double n = h.width(s.begin(), s.end()) / 1000.0;
						double sp = axis_ - n * size_;
						if (sp < 0.0) sp = 0.0;
						
						text_contents t(s, coordinate(origin_.x() + ox + sp,
							origin_.y() + oy + size_ / 2.0), *f_, *fj_);
						t.font_size(size_);
						t.font_color(txt_);
						t(out);
						
						first += step;
						oy += (area_.second - z_margin_h_) * (step / static_cast<double>(last));
					}
					
					return true;
				}
			};
		}
	}
}

#endif // FAML_PDF_CONTENTS_CHART3D_H
