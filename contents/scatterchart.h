#ifndef FAML_PDF_CONTENTS_SCATTERCHART_H
#define FAML_PDF_CONTENTS_SCATTERCHART_H

#include <string>
#include "babel/babel.h"
#include "clx/format.h"
#include "chart2d.h"
#include "polygon.h"

namespace faml {
	namespace pdf {
		namespace contents {
			template <
				class DataMap,
				class CharT = char,
				class Traits = std::char_traits<CharT>
			>
			class scatterchart : public chart2d<DataMap, CharT, Traits> {
			public:
				typedef chart2d<DataMap, CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::char_type char_type;
				typedef typename super::string_type string_type;
				typedef typename super::ostream_type ostream_type;
				typedef typename super::container container;
				
				scatterchart() : super() {}
				
				scatterchart(const coordinate& o, double w, double h) :
					super(o, w, h) {}
				
				virtual ~scatterchart() throw() {}
				
			private:
				typedef clx::basic_format<CharT, Traits> fmt;
				typedef basic_text_contents<CharT, Traits> text_contents;
				typedef basic_rectangle<CharT, Traits> rectangle;
				typedef clx::shared_ptr<basic_shape<CharT, Traits> > shape_ptr;
				
				/* --------------------------------------------------------- */
				//  put_data
				/* --------------------------------------------------------- */
				virtual bool put_data(ostream_type& out, double ox, double oy) {
					double width = this->area_.first / (this->in_->labels().size() + 1);
					double lower = this->valax_.min();
					double upper = this->valax_.max();
					
					out << LITERAL("q") << std::endl;
					this->put_origin(out);
					
					double y0 = oy;
					double y1 = oy + super::delta;
					if (this->palette_) {
						y0 -= super::delta;
						y1 -= super::delta;
					}
					
					out << fmt(LITERAL("%f %f m")) % ox % y0 << std::endl;
					out << fmt(LITERAL("%f %f l")) % ox % y1 << std::endl;
					out << LITERAL("s") << std::endl;
					double x = ox + width;
					for (size_type i = 0; i < this->in_->labels().size(); ++i) {
						this->put_border(out);
						out << fmt(LITERAL("%f %f m")) % x % y0 << std::endl;
						out << fmt(LITERAL("%f %f l")) % x % y1 << std::endl;
						out << LITERAL("s") << std::endl;
						x += width;
					}
					out << fmt(LITERAL("%f %f m")) % (ox + this->area_.first) % oy << std::endl;
					out << fmt(LITERAL("%f %f l")) % (ox + this->area_.first) % (oy - super::delta) << std::endl;
					out << LITERAL("s") << std::endl;
					
					for (size_type i = 0; i < this->in_->size(); ++i) {
						x = ox + width;
						color ln;
						if (this->palette_) ln = color(
							detail::getcolor(i, *this->palette_, this->palette_->size() - 6));
						else ln = color(detail::getcolor(i));
						
						for (size_type j = 0; j < this->in_->at(i).size(); ++j) {
							//double y = oy + this->area_.second * (this->in_->at(i).at(j) / upper);
							double y = 0.0;
							if (this->valax_.type() == axis::backward) {
								y = oy + this->area_.second
									* (1.0 - (this->in_->at(i).at(j) - lower) / (upper - lower));
							}
							else y = oy + this->area_.second * ((this->in_->at(i).at(j) - lower) / (upper - lower));
							shape_ptr sh = detail::getmark<CharT, Traits>(coordinate(x, y), i);
							sh->background(ln);
							sh->border(ln);
							(*sh)(out);
							x += width;
						}
						//out << LITERAL("S") << std::endl;
					}
					out << LITERAL("Q") << std::endl;
					
					return true;
				}
				
				/* --------------------------------------------------------- */
				//  put_xlabel
				/* --------------------------------------------------------- */
				virtual bool put_xlabel(ostream_type& out, double ox, double oy) {
					babel::init_babel();
					double width = this->area_.first / (this->in_->labels().size() + 1);
					text_helper h(this->f_->property(), this->f_->charset());
					for (size_type i = 0; i < this->in_->labels().size() + 2; i += 2) {
						string_type s = clx::lexical_cast<string_type>(i);
						std::basic_string<wchar_t> tmp = babel::utf8_to_unicode(s);
						s = win32conv(tmp.begin(), tmp.end());
						double n = h.width(s.begin(), s.end()) / 1000.0 * this->size_;
						
						text_contents t(s, coordinate(this->origin().x() + ox + width * i - n / 2.0,
							this->origin().y() + oy + this->size_), *(this->f_), *(this->fj_));
						t.font_size(this->size_);
						t.font_color(this->txt_);
						t(out);
					}
					
					return true;
				}
				
				/* --------------------------------------------------------- */
				//  put_legend
				/* --------------------------------------------------------- */
				virtual bool put_legend(ostream_type& out, double ox, double oy) {
					double height = this->in_->size() * (this->size_ + 5.0);
					double sp = this->height() - height;
					if (sp < 0.0) sp = 0.0;
					oy += sp / 2.0;
					double y = oy;
					double width = 0.0;
					
					text_helper h(this->f_->property(), this->f_->charset());
					for (size_type i = this->in_->size(); i > 0; --i) {
						shape_ptr sh = detail::getmark<CharT, Traits>(coordinate(
							this->origin().x() + ox, this->origin().y() + y), i - 1);
						if (this->palette_) {
							sh->background(detail::getcolor(i - 1, *this->palette_, this->palette_->size() - 6));
							sh->border(detail::getcolor(i - 1, *this->palette_,this->palette_->size() - 6));
						}
						else {
							sh->background(detail::getcolor(i - 1));
							sh->border(color(0x000000));
						}
						(*sh)(out);
						
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
					
					this->put_legend_border(out, ox, oy, ox + width + 18, y);
					
					return true;
				}
				
				/* --------------------------------------------------------- */
				//  put_legend_border
				/* --------------------------------------------------------- */
				bool put_legend_border(ostream_type& out, double ox, double oy, double x, double y) {
					if (this->fgleg_.is_valid() || this->bgleg_.is_valid()) {
						out << LITERAL("q") << std::endl;
						this->put_origin(out);
						out << fmt(LITERAL("%f w")) % this->weight_ << std::endl;
						
						if (this->fgleg_.is_valid()) {
							out << fmt(LITERAL("%f %f %f RG"))
								% this->fgleg_.red()
								% this->fgleg_.green()
								% this->fgleg_.blue()
							<< std::endl;
						}
						
						if (this->bgleg_.is_valid()) {
							out << fmt(LITERAL("%f %f %f rg"))
								% this->bgleg_.red()
								% this->bgleg_.green()
								% this->bgleg_.blue()
							<< std::endl;
						}
						
						out << fmt(LITERAL("%f %f %f %f re"))
							% (ox - 3.0) % (oy - 3.0) % (x - ox) % (y - oy) << std::endl;
						if (this->fgleg_.is_valid() && this->bgleg_.is_valid()) {
							out << LITERAL("b") << std::endl;
						}
						else if (this->bgleg_.is_valid()) {
							out << LITERAL("f") << std::endl;
						}
						else out << LITERAL("s") << std::endl;
						out << LITERAL("Q") << std::endl;
					}
					
					return true;
				}
			};
		}
	}
}

#endif // FAML_PDF_CONTETNS_SCATTERCHART_H
