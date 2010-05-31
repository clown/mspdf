#ifndef FAML_PDF_CONTENTS_BARCHART_H
#define FAML_PDF_CONTENTS_BARCHART_H

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
			class barchart : public chart2d<DataMap, CharT, Traits> {
			public:
				typedef chart2d<DataMap, CharT, Traits> super;
				typedef typename super::size_type size_type;
				typedef typename super::char_type char_type;
				typedef typename super::string_type string_type;
				typedef typename super::ostream_type ostream_type;
				typedef typename super::container container;
				
				barchart() : super() {}
				
				barchart(const coordinate& o, double w, double h) :
					super(o, w, h) {}
				
				virtual ~barchart() throw() {}
				
			private:
				typedef clx::basic_format<CharT, Traits> fmt;
				typedef basic_text_contents<CharT, Traits> text_contents;
				typedef basic_rectangle<CharT, Traits> rectangle;
				
				/* --------------------------------------------------------- */
				//  put_data
				/* --------------------------------------------------------- */
				virtual bool put_data(ostream_type& out, double ox, double oy) {
					size_type n = (this->in_->size() + 1) * this->in_->labels().size();
					double width = this->area_.first / n;
					double upper = this->scale_.first;
					ox += width / 2.0;
					for (size_type i = 0; i < this->in_->labels().size(); ++i) {
						for (size_type j = 0; j < this->in_->size(); ++j) {
							if (static_cast<int>(this->in_->at(j).at(i)) == 0) {
								ox += width;
								continue;
							}
							
							double height = this->area_.second * (this->in_->at(j).at(i) / upper);
							rectangle sh(coordinate(this->origin().x() + ox,
								this->origin().y() + oy), width, height);
							if (this->palette_) {
								sh.border(detail::getcolor(j, *this->palette_, this->palette_->size() - 6));
								sh.background(detail::getcolor(j, *this->palette_, this->palette_->size() - 6));
							}
							else {
								sh.border(color(0x000000));
								sh.background(detail::getcolor(j));
							}
							sh(out);
							ox += width;
						}
						ox += width / 2.0;
						
						out << LITERAL("q") << std::endl;
						this->put_origin(out);
						this->put_border(out);
						
						double y0 = oy;
						double y1 = oy + super::delta;
						if (this->palette_) {
							y0 -= super::delta;
							y1 -= super::delta;
						}
						
						out << fmt(LITERAL("%f %f m")) % ox % y0 << std::endl;
						out << fmt(LITERAL("%f %f l")) % ox % y1 << std::endl;
						out << LITERAL("s") << std::endl;
						out << LITERAL("Q") << std::endl;
						
						ox += width / 2.0;
					}
					
					return true;
				}
				
				/* --------------------------------------------------------- */
				//  put_xlabel
				/* --------------------------------------------------------- */
				virtual bool put_xlabel(ostream_type& out, double ox, double oy) {
					babel::init_babel();
					double width = this->area_.first / this->in_->labels().size();
					text_helper h(this->f_->property(), this->f_->charset());
					for (size_type i = 0; i < this->in_->labels().size(); ++i) {
						std::basic_string<wchar_t> tmp = babel::utf8_to_unicode(this->in_->labels().at(i));
						string_type s = win32conv(tmp.begin(), tmp.end());
						double n = h.width(s.begin(), s.end()) / 1000.0;
						double sp = width - n * this->size_;
						if (sp < 0.0) sp = 0.0;
						
						text_contents t(s, coordinate(this->origin().x() + ox + sp / 2.0,
							this->origin().y() + oy + this->size_), *(this->f_), *(this->fj_));
						t.font_size(this->size_);
						t.font_color(this->txt_);
						t(out);
						
						ox += width;
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
			};
		}
	}
}

#endif // FAML_PDF_CONTENTS_BARCHART_H
