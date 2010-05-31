#ifndef FAML_PDF_CONTENTS_CHART2D_H
#define FAML_PDF_CONTENTS_CHART2D_H

#include <algorithm>
#include <string>
#include <ostream>
#include <map>
#include "clx/lexical_cast.h"
#include "clx/utf8.h"
#include "../coordinate.h"
#include "../color.h"
#include "../code_convert.h"
#include "../font.h"
#include "../text_helper.h"
#include "../text_contents.h"
#include "axis.h"
#include "polygon.h"
#include "chart_shape.h"
#include "shape_factory.h"

namespace faml {
	namespace pdf {
		namespace contents {
			namespace detail {
				
				inline size_t xentint(size_t rgb, double percent) {
					size_t red = rgb >> 16;
					size_t green = (rgb >> 8) & 0xff;
					size_t blue = rgb & 0xff;
					
					size_t dest = 0;
					dest |= (static_cast<size_t>(red * percent + 255 * (1.0 - percent)) << 16);
					dest |= (static_cast<size_t>(green * percent + 255 * (1.0 - percent)) << 8);
					dest |= (static_cast<size_t>(blue * percent + 255 * (1.0 - percent)));
					
					return dest;
				}
				
				template <class Container>
				inline const color getcolor(size_t pos, const Container& palette, size_t count) {
					color c(palette[pos % count + 4]);
					if (pos / count == 0) return c;
					
					// TODO: 最終的には白になってしまうが，その後どうするのか？
					size_t rgb = palette[pos % count + 4];
					double rate = 1.0 - std::min(pos / count * 0.25, 1.0);
					rgb = xentint(rgb, rate);
					return color(rgb);
				}
				
				inline const color getcolor(size_t pos) {
					static const size_t palette[] = {
						0x000000, 0xffffff, 0x1f497d, 0xeeece1,
						0x9999ff, 0x993366, 0xffffcc, 0xccffff,
						0x660066, 0xff8080, 0x0066cc, 0xccccff,
						0x000080, 0xff00ff, 0xffff00, 0x00ffff,
						0x800080, 0x800000, 0x0000ff
					};
					
					return getcolor(pos, palette, 15);
				}
				
				template <class Ch, class Tr>
				clx::shared_ptr<basic_shape<Ch, Tr> > getmark(const coordinate& o, size_t id) {
					typedef clx::shared_ptr<basic_shape<Ch, Tr> > shape_ptr;
					double w = 7.0;
					double h = 7.0;
					
					shape_ptr dest;
					size_t t = id % 9;
					
					bool entag = true;
					switch (t) {
					case 0: 
						dest = shape_ptr(new basic_diamond<Ch, Tr>(o, w, h, entag));
						break;
					case 1:
						dest = shape_ptr(new basic_rectangle<Ch, Tr>(o, w, h, entag));
						break;
					case 2:
						dest = shape_ptr(new basic_triangle<Ch, Tr>(o, w, h, entag));
						break;
					case 3:
						//dest = shape_ptr(new basic_math_multiply<Ch, Tr>(o, w, h, entag));
						dest = shape_ptr(new basic_chart_multi<Ch, Tr>(o, w, h, entag));
						break;
					case 4:
						//dest = shape_ptr(new basic_star8<Ch, Tr>(o, w, h, entag));
						dest = shape_ptr(new basic_chart_oct_multi<Ch, Tr>(o, w, h, entag));
						break;
					case 5:
						dest = shape_ptr(new basic_ellipse<Ch, Tr>(o, w, h, entag));
						break;
					case 6:
						//dest = shape_ptr(new basic_math_plus<Ch, Tr>(o, w, h, entag));
						dest = shape_ptr(new basic_chart_plus<Ch, Tr>(o, w, h, entag));
						break;
					case 7:
						dest = shape_ptr(new basic_math_minus<Ch, Tr>(o, w, h, entag));
						break;
					case 8:
						dest = shape_ptr(new basic_math_minus<Ch, Tr>(o, w, h, entag));
						break;
					default:
						dest = shape_ptr(new basic_diamond<Ch, Tr>(o, w, h, entag));
						break;
					}
					
					return dest;
				}
			}
			
			template <
				class DataMap,
				class CharT = char,
				class Traits = std::char_traits<CharT>
			>
			class chart2d {
			public:
				typedef size_t size_type;
				typedef CharT char_type;
				typedef std::basic_string<CharT, Traits> string_type;
				typedef std::basic_ostream<CharT, Traits> ostream_type;
				typedef DataMap container;
				typedef axis axis_type;
				typedef basic_font<CharT, Traits> font_type;
				typedef std::vector<size_type> palette_type;
				
				enum { // options
					inner	= 0x00010000,
					marked	= 0x00020000
				};
				
				chart2d() :
					in_(NULL), f_(NULL), fj_(NULL),
					option_(0), position_(0x12),
					scale_(), area_(),
					margin_(10.0), axis_(0.0), legend_(0.0),
					weight_(0.75), size_(10.0),
					fg_(), bg_(),
					fgplot_(), bgplot_(),
					fgleg_(), bgleg_(),
					txt_(0),
					valax_(), palette_(NULL),
					origin_(), width_(0.0), height_(0.0) {}
				
				explicit chart2d(const coordinate& o, double w, double h) :
					in_(NULL), f_(NULL), fj_(NULL),
					option_(0), position_(0x12),
					margin_(10.0), axis_(0.0), legend_(0.0),
					weight_(0.75), size_(10.0),
					fg_(), bg_(),
					fgplot_(), bgplot_(),
					fgleg_(), bgleg_(),
					txt_(0),
					valax_(), palette_(NULL),
					origin_(o), width_(w), height_(h) {}
				
				virtual ~chart2d() throw() {}
				
				/* --------------------------------------------------------- */
				//  main operator
				/* --------------------------------------------------------- */
				bool operator()(ostream_type& out) {
					// 左下から順に出力する．
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
					if ((option_ & inner)) ox -= legend_;
					oy = 0.0;
					if ((option_ & inner)) this->xcalc_valign();
					this->put_legend(out, ox, oy);
					
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
						size_t tmp = clx::utf8::distance(in_->at(i).name());
						if (n < tmp) n = tmp;
					}
					legend_ = (n + 1) * size_;
					
					// calc scale
					this->xcalc_scale();
					
					// width of y-axis
					string_type s = clx::lexical_cast<string_type>(scale_.first);
					axis_ = s.size() * size_ * 0.5;
					
					// widt/height of plot area.
					area_.first = width_ - legend_ - axis_ - 4 * margin_;
					if ((option_ & inner)) area_.first += legend_;
					area_.second = height_ - size_ - 3 * margin_;
					
					return true;
				}
				
				/* --------------------------------------------------------- */
				//  xcalc_scale
				/* --------------------------------------------------------- */
				bool xcalc_scale() {
					static const int lst[3] = { 1, 2, 5 };
					
					if (valax_.max() > 0.0 && valax_.unit() > 0.0) {
						// temporary
						scale_.first = valax_.max();
						scale_.second = valax_.unit();
						return true;
					}
					
					double mx = 0.0;
					for (size_type i = 0; i < in_->size(); ++i) {
						for (size_type j = 0; j < in_->at(i).size(); ++j) {
							if (mx < in_->at(i).at(j)) mx = in_->at(i).at(j);
						}
					}
					
					int value = static_cast<int>(mx + 0.5);
					int digit = 1;
					while (1) {
						for (size_type i = 0; i < 3; ++i) {
							int div = lst[i] * digit;
							int n = value / div;
							if (n < 10) {
								valax_.max((n + 1) * div);
								valax_.unit(div);
								
								// temporary
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
				//  xcalc_valign
				/* --------------------------------------------------------- */
				void xcalc_valign() {
					double lower = this->valax_.min();
					double upper = this->valax_.max();
					double mid1 = lower + (upper - lower) / 3.0;
					double mid2 = lower + (upper - lower) / 3.0 * 2.0;
					
					size_type top = 0;
					size_type center = 0;
					size_type bottom = 0;
					size_type pos = in_->at(0).size() - 1;
					for (size_type i = 0; i < in_->size(); ++i) {
						for (size_type j = pos; j < in_->at(i).size(); ++j) {
							if (this->in_->at(i).at(j) < mid1) {
								if (this->valax_.type() == axis::backward) ++top;
								else ++bottom;
							}
							else if (this->in_->at(i).at(j) < mid2) ++center;
							else {
								if (this->valax_.type() == axis::backward) ++bottom;
								else ++top;
							}
						}
					}
					
					// 1 番要素の少ない所へラベルを出力．
					position_ &= ~(0x000000f0);
					if (bottom < center && bottom < top) position_ |= 0x0020;
					else if (center < top) position_ |= 0x0010;
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
					
					double x0 = ox;
					double y0 = oy;
					double x1 = ox + area_.first;
					double y1 = oy + area_.second;
					if (palette_) {
						x0 -= delta;
						y0 -= delta;
					}
					
					out << fmt(LITERAL("%f %f m")) % x0 % oy << std::endl;
					out << fmt(LITERAL("%f %f l")) % x1 % oy << std::endl;
					out << fmt(LITERAL("%f %f m")) % ox % y0 << std::endl;
					out << fmt(LITERAL("%f %f l")) % ox % y1 << std::endl;
					out << LITERAL("S") << std::endl;
					out << LITERAL("Q") << std::endl;
					
					return true;
				}
				
				/* --------------------------------------------------------- */
				//  xput_scale
				/* --------------------------------------------------------- */
				bool xput_scale(ostream_type& out, double ox, double oy) {
#if 0
					int first = 0;
					int last = scale_.first;
					int step = scale_.second;
#else
					int first = static_cast<int>(valax_.min());
					int last = static_cast<int>(valax_.max());
					int step = static_cast<int>(valax_.unit());
#endif
					int pos = first;
					out << LITERAL("q") << std::endl;
					this->put_origin(out);
					this->put_border(out);
					while (pos <= last) {
						double x0 = ox;
						double x1 = (option_ & 0x00040000) ? ox + area_.first : ox + delta;
						if (palette_) {
							x0 -= delta;
							if (!(option_ & 0x00040000)) x1 -= delta;
						}
						out << fmt(LITERAL("%f %f m")) % x0 % oy << std::endl;
						out << fmt(LITERAL("%f %f l")) % x1 % oy << std::endl;
						out << LITERAL("s") << std::endl;
						pos += step;
						oy += area_.second * (step / static_cast<double>(last - first));
					}
					out << LITERAL("Q") << std::endl;
					
					return true;
				}
				
				/* --------------------------------------------------------- */
				//  xput_ylabel
				/* --------------------------------------------------------- */
				bool xput_ylabel(ostream_type& out, double ox, double oy) {
#if 0
					int first = 0;
					int last = scale_.first;
					int step = scale_.second;
#else
					int first = static_cast<int>(valax_.min());
					int last = static_cast<int>(valax_.max());
					int step = static_cast<int>(valax_.unit());
#endif
					int pos = first;
					text_helper h(f_->property(), f_->charset());
					if (valax_.type() == axis::backward) oy += area_.second;
					while (pos <= last) {
						std::basic_stringstream<CharT, Traits> conv;
						conv << pos;
						string_type tmp = conv.str();
						//string_type s = win32conv(tmp.begin(), tmp.end());
						string_type s = to_narrow(tmp.begin(), tmp.end());
						double n = h.width(s.begin(), s.end()) / 1000.0;
						double sp = axis_ - n * size_;
						if (sp < 0.0) sp = 0.0;
						
						text_contents t(s, coordinate(origin_.x() + ox + sp,
							origin_.y() + oy + size_ / 2.0), *f_, *fj_);
						t.font_size(size_);
						t.font_color(txt_);
						t(out);
						
						pos += step;
						if (valax_.type() == axis::backward) {
							oy -= area_.second * (step / static_cast<double>(last - first));
						}
						else oy += area_.second * (step / static_cast<double>(last - first));
					}
					
					return true;
				}
			};
		}
	}
}

#endif // FAML_PDF_CONTENTS_CHART2D_H
