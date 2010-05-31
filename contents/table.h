/* ------------------------------------------------------------------------- */
/*
 *  contents/table.h
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
 *  Last-modified: Sat 18 Apr 2009 01:11:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PDF_CONTENTS_TABLE_H
#define FAML_PDF_CONTENTS_TABLE_H

#include <cmath>
#include <string>
#include <vector>
#include "clx/literal.h"
#include "clx/format.h"
#include "color.h"
#include "border.h"

namespace faml {
	namespace pdf {
		/* ------------------------------------------------------------- */
		//  table_cell
		/* ------------------------------------------------------------- */
		class table_cell {
		public:
			typedef size_t size_type;
			
			enum {
				none		= 0x000,
				row_beg		= 0x010,
				row_merge	= 0x020,
				row_end		= 0x040,
				col_beg		= 0x100,
				col_merge	= 0x200,
				col_end		= 0x400
			};
			
			table_cell() :
				width_(0.0), height_(0.0),
				type_(0), background_(),
				top_(), bottom_(), left_(), right_(), diagonal_() {}
			
			explicit table_cell(double w, double h) :
				width_(w), height_(h),
				type_(0), background_(),
				top_(), bottom_(), left_(), right_(), diagonal_() {}
			
			/* --------------------------------------------------------- */
			//  Access methods (get).
			/* --------------------------------------------------------- */
			double width() const { return width_; }
			double height() const { return height_; }
			size_type type() const { return type_; }
			const color& background() const { return background_; }
			const border& top() const { return top_; }
			const border& bottom() const { return bottom_; }
			const border& left() const { return left_; }
			const border& right() const { return right_; }
			const border& diagonal() const { return diagonal_; }
			
			/* --------------------------------------------------------- */
			//  Access methods (set).
			/* --------------------------------------------------------- */
			double& width() { return width_; }
			double& height() { return height_; }
			size_type& type() { return type_; }
			color& background() { return background_; }
			border& top() { return top_; }
			border& bottom() { return bottom_; }
			border& left() { return left_; }
			border& right() { return right_; }
			border& diagonal() { return diagonal_; }
			
		private:
			double width_;
			double height_;
			size_type type_;
			color background_;
			border top_;
			border bottom_;
			border left_;
			border right_;
			border diagonal_;
		};
		
		/* ------------------------------------------------------------- */
		//  table_row
		/* ------------------------------------------------------------- */
		class table_row {
		public:
			typedef size_t size_type;
			typedef table_cell value_type;
			typedef std::vector<value_type> container;
			typedef container::const_iterator const_iterator;
			
			table_row() : v_(), maxheight_(0.0) {}
			
			void add(const value_type& elem) {
				v_.push_back(elem);
				if (maxheight_ < elem.height()) maxheight_ = elem.height();
			}
			
			/* --------------------------------------------------------- */
			//  Access methods (get).
			/* --------------------------------------------------------- */
			double height() const { return maxheight_; }
			bool empty() const { return v_.empty(); }
			size_type size() const { return v_.size(); }
			const value_type& at(size_type pos) const { return v_.at(pos); }
			const value_type& operator[](size_type pos) const { return v_[pos]; }
			const_iterator begin() const { return v_.begin(); }
			const_iterator end() const { return v_.end(); }
			
		private:
			container v_;
			double maxheight_;
		};
			
		namespace contents {
			/* ------------------------------------------------------------- */
			//  basic_table
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_table {
			public:
				typedef size_t size_type;
				typedef CharT char_type;
				typedef std::basic_string<CharT, Traits> string_type;
				typedef table_cell value_type;
				typedef table_row subcontainer;
				typedef std::vector<subcontainer> container;
				
				basic_table() : origin_(), v_() {}
				
				explicit basic_table(const coordinate& o) :
					origin_(o), v_() {}
				
				template <class OutStream>
				bool operator()(OutStream& out) {
					return this->xcontents(out);
				}
				
				void add(const value_type& elem) {
					if (v_.empty()) this->newline();
					v_.back().add(elem);
				}
				
				void newline() {
					v_.push_back(subcontainer());
				}
				
				/* --------------------------------------------------------- */
				//  Access methods (set).
				/* --------------------------------------------------------- */
				coordinate& origin() { return origin_; }
				
			private:
				typedef clx::basic_format<CharT, Traits> fmt;
				
				coordinate origin_;
				container v_;
				
				/* --------------------------------------------------------- */
				//  xcontents
				/* --------------------------------------------------------- */
				template <class OutStream>
				bool xcontents(OutStream& out) {
					if (v_.empty()) return false;
					
					double pos = 0;
					for (size_type i = 0; i < v_.size(); ++i) pos += v_.at(i).height();
					
					out << LITERAL("q") << std::endl;
					double oy = pos;
					out << fmt(LITERAL("1 0 0 1 %f %f cm")) % origin_.x() % (origin_.y() - oy) << std::endl;
					for (size_type i = 0; i < v_.size(); ++i) {
						double ox = 0;
						for (size_type j = 0; j < v_.at(i).size(); ++j) {
							double x = ox + v_.at(i).at(j).width();
							//double y = oy - v_.at(i).at(j).height();
							double y = oy - v_.at(i).height();
							
							if (v_.at(i).at(j).background().is_valid()) {
								this->xput_background(out, v_.at(i).at(j).background(),
									coordinate(ox, oy), coordinate(x, y));
							}
							ox += v_.at(i).at(j).width();
						}
						oy -= v_.at(i).height();
					}
					
					oy = pos;
					for (size_type i = 0; i < v_.size(); ++i) {
						double ox = 0;
						for (size_type j = 0; j < v_.at(i).size(); ++j) {
							double x = ox + v_.at(i).at(j).width();
							//double y = oy - v_.at(i).at(j).height();
							double y = oy - v_.at(i).height();
							
							// 1. ã‚Ì˜gü
							if ((v_.at(i).at(j).type() & 0x060) == 0 && v_.at(i).at(j).top().fill().is_valid()) {
								this->xput_border(out, v_.at(i).at(j).top(),
									coordinate(ox, oy), coordinate(x, oy));
								
								double doubled = false;
								if (i > 0 && j < v_.at(i - 1).size() &&
									(v_.at(i - 1).at(j).bottom().type() & 0x010)) {
									doubled = true;
								}
								
								if ((v_.at(i).at(j).top().type() & 0x010) && !doubled) {
									//double left = (v_.at(i).at(j).left().type() & 0x010) ? 1.0 : 0.0;
									//double right = (v_.at(i).at(j).right().type() & 0x010) ? 1.0 : 0.0;
									double left = 0.0;
									double right = 0.0;
									this->xput_border(out, v_.at(i).at(j).top(),
										coordinate(ox + left, oy - 1.0), coordinate(x - right, oy - 1.0));
								}
								
							}
							
							// 2. ‰E‚Ì˜gü
							if ((v_.at(i).at(j).type() & 0x300) == 0 && v_.at(i).at(j).right().fill().is_valid()) {
								bool stronger = false;
								if (j + 1 >= v_.at(i).size()) {
									stronger = true;
								}
								else if ((v_.at(i).at(j + 1).type() & 0x600) != 0) {
									stronger = true;
								}
								else if (!v_.at(i).at(j + 1).left().fill().is_valid()) {
									stronger = true;
								}
								else if (v_.at(i).at(j).right().weight() > v_.at(i).at(j + 1).left().weight()) {
									stronger = true;
								}
								else if (v_.at(i).at(j + 1).left().type() == 0x000) {
									stronger = true;
								}
								else if (v_.at(i).at(j).right().type() < v_.at(i).at(j + 1).left().type()) {
									stronger = true;
								}
								
								if (stronger) {
									this->xput_border(out, v_.at(i).at(j).right(),
										coordinate(x, oy), coordinate(x, y));
								}
								
								if ((v_.at(i).at(j).right().type() & 0x010)) {
									//double top = (v_.at(i).at(j).top().type() & 0x010) ? 1.0 : 0.0;
									//double bottom = (v_.at(i).at(j).bottom().type() & 0x010) ? 1.0 : 0.0;
									double top = 0.0;
									double bottom = 0.0;
									this->xput_border(out, v_.at(i).at(j).right(),
										coordinate(x - 1.0, oy - top), coordinate(x - 1.0, y + bottom));
								}
							}
							
							// 3. ‰º‚Ì˜gü
							if ((v_.at(i).at(j).type() & 0x030) == 0 && v_.at(i).at(j).bottom().fill().is_valid()) {
								bool stronger = false;
								if (i + 1 >= v_.size() || j >= v_.at(i + 1).size()) {
									stronger = true;
								}
								else {
									if ((v_.at(i + 1).at(j).type() & 0x060) != 0) {
										stronger = true;
									}
									else if (v_.at(i).at(j).bottom().weight() > v_.at(i + 1).at(j).top().weight()) {
										stronger = true;
									}
									else if (v_.at(i + 1).at(j).top().type() == 0x000) {
										stronger = true;
									}
									else if (v_.at(i).at(j).bottom().type() < v_.at(i + 1).at(j).top().type()) {
										stronger = true;
									}
								}
								
								if (stronger) {
									this->xput_border(out, v_.at(i).at(j).bottom(),
										coordinate(x, y), coordinate(ox, y));
								}
								
								if ((v_.at(i).at(j).bottom().type() & 0x010)) {
									//double left = (v_.at(i).at(j).left().type() & 0x010) ? 1.0 : 0.0;
									//double right = (v_.at(i).at(j).right().type() & 0x010) ? 1.0 : 0.0;
									double left = 0.0;
									double right = 0.0;
									this->xput_border(out, v_.at(i).at(j).bottom(),
										coordinate(x - right, y + 1.0), coordinate(ox + left, y + 1.0));
								}
							}
							
							// 4. ¶‚Ì˜gü
							if ((v_.at(i).at(j).type() & 0x600) == 0 && v_.at(i).at(j).left().fill().is_valid()) {
								this->xput_border(out, v_.at(i).at(j).left(),
									coordinate(ox, y), coordinate(ox, oy));
								
								double doubled = false;
								if (j > 0 && (v_.at(i).at(j - 1).right().type() & 0x010)) {
									doubled = true;
								}
								
								if ((v_.at(i).at(j).left().type() & 0x010) && !doubled) {
									//double top = (v_.at(i).at(j).top().type() & 0x010) ? 1.0 : 0.0;
									//double bottom = (v_.at(i).at(j).bottom().type() & 0x010) ? 1.0 : 0.0;
									double top = 0.0;
									double bottom = 0.0;
									this->xput_border(out, v_.at(i).at(j).left(),
										coordinate(ox + 1.0, y + top), coordinate(ox + 1.0, oy - bottom));
								}
							}
							
							if (v_.at(i).at(j).diagonal().fill().is_valid()) {
								if ((v_.at(i).at(j).diagonal().type() & 0xff00) == 0x0100) {
									this->xput_border(out, v_.at(i).at(j).diagonal(),
										coordinate(ox, oy), coordinate(x, y));
								}
								else {
									this->xput_border(out, v_.at(i).at(j).diagonal(),
										coordinate(ox, y), coordinate(x, oy));
								}
							}
							
							ox += v_.at(i).at(j).width();
						}
						oy -= v_.at(i).height();
					}
					out << LITERAL("Q") << std::endl;
					
					return true;
				}
				
				/* --------------------------------------------------------- */
				//  xput_background
				/* --------------------------------------------------------- */
				template <class OutStream>
				bool xput_background(OutStream& out, const color& src,
					const coordinate& from, const coordinate& to) {
					out << fmt(LITERAL("%f %f %f rg"))
						% src.red() % src.green() % src.blue() << std::endl;
					out << fmt(LITERAL("%f %f %f %f re"))
						% from.x() % from.y() % (to.x() - from.x()) % (to.y() - from.y())
						<< std::endl;
					out << LITERAL("F") << std::endl;
					return true;
				}
				
				/* --------------------------------------------------------- */
				//  xput_border
				/* --------------------------------------------------------- */
				template <class OutStream>
				bool xput_border(OutStream& out, const border& src,
					const coordinate& from, const coordinate& to) {
					if (src.type() == 0) return true;
					out << fmt(LITERAL("%f w")) % src.weight() << std::endl;
					switch (src.type() & 0x0f) {
					case border::solid:
						out << LITERAL("[] 0 d") << std::endl;
						break;
					case border::dash:
						out << LITERAL("[5 2] 0 d") << std::endl;
						break;
					case border::dot:
						out << LITERAL("[2 2] 1 d") << std::endl;
						break;
					case border::hair:	// dot ‚æ‚è‚à‚³‚ç‚É–Ú‚Ì×‚©‚¢“_ü‚©H
						out << LITERAL("[1 1] 0 d") << std::endl;
						break;
					case border::dash_dot:
						out << LITERAL("[5 2 2 2] 0 d") << std::endl;
						break;
					case border::dash_wdot:
						out << LITERAL("[5 2 2 2 2 2] 2 d") << std::endl;
						break;
					default:
						out << LITERAL("[] 0 d") << std::endl;
						break;
					}
					
					out << fmt(LITERAL("%f %f %f RG"))
						% src.fill().red() % src.fill().green() % src.fill().blue() << std::endl;
					out << fmt(LITERAL("%f %f m")) % from.x() % from.y() << std::endl;
					out << fmt(LITERAL("%f %f l")) % to.x() % to.y() << std::endl;
					out << LITERAL("S") << std::endl;
					
					return true;
				}
			};
		}
	}
}

#endif // FAML_PDF_CONTENTS_TABLE_H
