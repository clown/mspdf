/* ------------------------------------------------------------------------- */
/*
 *  xslx/style.h
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
 *  Last-modified: Fri 05 Jun 2009 15:13:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_XLSX_STYLE_H
#define FAML_XLSX_STYLE_H

#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include "rapidxml/rapidxml.hpp"
#include "clx/lexical_cast.h"
#include "clx/literal.h"
#include "clx/utility.h"
#include "../officex/utility.h"
#include "../officex/font.h"
#include "../officex/theme.h"
#include "utility.h"
#include "format.h"

namespace faml {
	namespace xlsx {
		/* ----------------------------------------------------------------- */
		//  basic_border
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_border {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef std::pair<size_type, size_type> value_type;
			
			/* ------------------------------------------------------------- */
			//  border type (pp. 2673)
			/* ------------------------------------------------------------- */
			enum {
				none = 0, 			thin, 			medium, 		thick,		doubled,
				dotted = 10, 		dash_dot, 		dash_wdot,		dashed,		hair,
				m_dash_dot = 20,	m_dash_wdot,	m_dashed,
				diag_down = 0x0100,	diag_up = 0x0200,
				unknown = 255
			};
			
			basic_border() :
				left_(), right_(), top_(), bottom_(), diagonal_() {}
			
			explicit basic_border(const value_type& val) :
				left_(val), right_(val), top_(val), bottom_(val), diagonal_() {}
			
			explicit basic_border(const value_type& h, const value_type& v) :
				left_(h), right_(h), top_(v), bottom_(v), diagonal_() {}
			
			explicit basic_border(const value_type& l, const value_type& r,
				const value_type& t, const value_type& b) :
				left_(l), right_(r), top_(t), bottom_(b), diagonal_() {}
			
			virtual ~basic_border() throw() {}
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			const value_type& left() const { return left_; }
			const value_type& right() const { return right_; }
			const value_type& top() const { return top_; }
			const value_type& bottom() const { return bottom_; }
			const value_type& diagonal() const { return diagonal_; }
			
			/* ------------------------------------------------------------- */
			//  Access methods (set).
			/* ------------------------------------------------------------- */
			void left(const value_type& cp) { left_ = cp; }
			void right(const value_type& cp) { right_ = cp; }
			void top(const value_type& cp) { top_ = cp; }
			void bottom(const value_type& cp) { bottom_ = cp; }
			void diagonal(const value_type& cp) { diagonal_ = cp; }
			
			/* ------------------------------------------------------------- */
			/*
			 *  type
			 *
			 *  Converts from string to border type ID.
			 */
			/* ------------------------------------------------------------- */
			static size_type type(const string_type& s) {
				if (s == LITERAL("none")) return none;
				if (s == LITERAL("thin")) return thin;
				if (s == LITERAL("medium")) return medium;
				if (s == LITERAL("thick")) return thick;
				if (s == LITERAL("double")) return doubled;
				if (s == LITERAL("dotted")) return dotted;
				if (s == LITERAL("dashDot")) return dash_dot;
				if (s == LITERAL("dashDotDot")) return dash_wdot;
				if (s == LITERAL("dashed")) return dashed;
				if (s == LITERAL("slantDashDot")) return dashed;
				if (s == LITERAL("hair")) return hair;
				if (s == LITERAL("mediumDashDot")) return m_dash_dot;
				if (s == LITERAL("mediumDashDotDot")) return m_dash_wdot;
				if (s == LITERAL("mediumDashed")) return m_dashed;
				//return unknown;
				return thin;
			}
			
		private:
			value_type left_;
			value_type right_;
			value_type top_;
			value_type bottom_;
			value_type diagonal_;
		};
		
		/* ----------------------------------------------------------------- */
		//  basic_style
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_style {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef basic_border<CharT, Traits> border_type;
			typedef faml::officex::basic_font<CharT, Traits> font_type;
			
			basic_style() :
				border_(), font_(), fmt_(), rgb_(0xffffff),
				align_(5), valign_(1), indent_(0), rotate_(0), wrap_(false) {}
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			const border_type& border() const { return border_; }
			const font_type& font() const { return font_; }
			const string_type& numfmt() const { return fmt_; }
			size_type rgb() const { return rgb_; }
			size_type align() const { return align_; }
			size_type valign() const { return valign_; }
			size_type indent() const { return indent_; }
			size_type rotate() const { return rotate_; }
			bool wrap() const { return wrap_; }
			
			/* ------------------------------------------------------------- */
			//  Access methods (set).
			/* ------------------------------------------------------------- */
			void border(const border_type& cp) { border_ = cp; }
			void font(const font_type& cp) { font_ = cp; }
			void numfmt(const string_type& cp) { fmt_ = cp; }
			void rgb(size_type cp) { rgb_ = cp; }
			void align(size_type cp) { align_ = cp; }
			void valign(size_type cp) { valign_ = cp; }
			void indent(size_type cp) { indent_ = cp; }
			void rotate(size_type cp) { rotate_ = cp; }
			void wrap(bool cp) { wrap_ = cp; }
			
		private:
			border_type border_;
			font_type font_;
			string_type fmt_;
			size_type rgb_;
			size_type align_;
			size_type valign_;
			size_type indent_;
			size_type rotate_;
			bool wrap_;
		};
		
		/* ----------------------------------------------------------------- */
		//  basic_style_container
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_style_container {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef basic_style<CharT, Traits> value_type;
			typedef typename value_type::border_type border_type;
			typedef typename value_type::font_type font_type;
			typedef faml::officex::basic_theme<CharT, Traits> theme_type;
			
			basic_style_container() :
				v_(), theme_(NULL), fonts_(), fills_(), borders_(), masters_() {}
			
			template <class Ch, class Tr>
			basic_style_container(std::basic_istream<Ch, Tr>& in) :
				v_(), theme_(NULL), fonts_(), fills_(), borders_(), masters_() {
				this->read(in);
			}
			
			virtual ~basic_style_container() throw() {}
			
			/* ------------------------------------------------------------- */
			/*
			 *  read
			 *
			 *  Main operation of the class. Opens the xl/styles.xml and
			 *  read style of cells.
			 */
			/* ------------------------------------------------------------- */
			template <class Ch, class Tr>
			basic_style_container& read(std::basic_istream<Ch, Tr>& in) {
				STATIC_CHECK(sizeof(CharT) == sizeof(char), currently_limited_to_char_type);
				std::vector<char_type> s;
				clx::read(in, s);
				s.push_back(0); // make null terminated string.
				
				//rapidxml::xml_document<char_type> doc; // <- ??? compile error!!
				rapidxml::xml_document<char> doc;
				doc.parse<0>(reinterpret_cast<char_type*>(&s.at(0)));
				node_ptr root = doc.first_node(LITERAL("styleSheet"));
				if (!root) throw std::runtime_error("cannot find <styleSheet> (root) tag");
				
				this->xread_formats(root->first_node(LITERAL("numFmts")));
				this->xread_fonts(root->first_node(LITERAL("fonts")));
				this->xread_fills(root->first_node(LITERAL("fills")));
				this->xread_borders(root->first_node(LITERAL("borders")));
				this->xread_masters(root->first_node(LITERAL("cellStyleXfs")));
				this->xread_styles(root->first_node(LITERAL("cellXfs")));
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			bool empty() const { return v_.empty(); }
			size_type size() const { return v_.size(); }
			const value_type& at(size_type pos) const { return v_.at(pos); }
			const value_type& operator[](size_type pos) const { return v_[pos]; }
			
			/* ------------------------------------------------------------- */
			//  Access methods (set).
			/* ------------------------------------------------------------- */
			void theme(const theme_type* cp) { theme_ = cp; }
			
		private:
			typedef std::vector<value_type> container;
			typedef rapidxml::xml_node<CharT>* node_ptr;
			typedef rapidxml::xml_attribute<CharT>* attr_ptr;
			
			container v_;
			
			// internal variables
			const theme_type* theme_;
			
			std::map<size_type, string_type> formats_;
			std::vector<font_type> fonts_;
			std::vector<size_type> fills_;
			std::vector<border_type> borders_;
			std::vector<value_type> masters_;
			
			/* ------------------------------------------------------------- */
			//  xread_formats
			/* ------------------------------------------------------------- */
			basic_style_container& xread_formats(node_ptr root) {
				if (!root) return *this;
				
				for (node_ptr child = root->first_node(); child; child = child->next_sibling()) {
					if (string_type(child->name()) != LITERAL("numFmt")) continue;
					
					size_type index = 0;
					attr_ptr attr = child->first_attribute(LITERAL("numFmtId"));
					if (attr && attr->value_size() > 0) {
						index = clx::lexical_cast<size_type>(attr->value());
					}
					
					string_type s;
					attr = child->first_attribute(LITERAL("formatCode"));
					if (attr && attr->value_size() > 0) s = string_type(attr->value());
					if (!s.empty()) formats_[index] = s;
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_fonts
			/* ------------------------------------------------------------- */
			basic_style_container& xread_fonts(node_ptr root) {
				if (!root) throw std::runtime_error("cannot find <fonts> tag");
				
				node_ptr child = root->first_node(LITERAL("font"));
				if (!child) throw std::runtime_error("cannot find any <font> tags");
				for (; child; child = child->next_sibling()) {
					font_type elem(LITERAL("Arial"), 11, 0);
					
					// 1. font name
					node_ptr pos = child->first_node(LITERAL("name"));
					if (pos) {
						attr_ptr attr = pos->first_attribute(LITERAL("val"));
						if (attr) elem.name(attr->value());
					}
					
					// 2. font size
					pos = child->first_node(LITERAL("sz"));
					if (pos) {
						attr_ptr attr = pos->first_attribute(LITERAL("val"));
						if (attr) elem.size(clx::lexical_cast<double>(attr->value()));
					}
					
					// 3. font color
					pos = child->first_node(LITERAL("color"));
					if (pos) {
						if (theme_) elem.rgb(getrgb_from_attr(pos, theme_->palette()));
						else elem.rgb(getrgb_from_attr(pos));
					}
					
					// 4. font decoration
					size_type deco = 0;
					pos = child->first_node(LITERAL("b"));
					if (pos) deco |= font_type::bold;
					pos = child->first_node(LITERAL("i"));
					if (pos) deco |= font_type::italic;
					pos = child->first_node(LITERAL("u"));
					if (pos) deco |= font_type::underline;
					pos = child->first_node(LITERAL("strike"));
					if (pos) deco |= font_type::strike;
					elem.decorate(deco);
					
					fonts_.push_back(elem);
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_fills
			/* ------------------------------------------------------------- */
			basic_style_container& xread_fills(node_ptr root) {
				if (!root) throw std::runtime_error("cannot find <fills> tag");
				
				node_ptr child = root->first_node(LITERAL("fill"));
				if (!child) throw std::runtime_error("cannot find any <fill> tags");
				for (; child; child = child->next_sibling()) {
					size_type elem = 0xffffff;
					node_ptr tmp = child->first_node(LITERAL("patternFill"));
					if (tmp) {
						attr_ptr attr = tmp->first_attribute(LITERAL("patternType"));
						if (attr && attr->value_size() > 0) {
							string_type s(attr->value());
							if (s == LITERAL("none")) elem = 0xffffff;
							else if (s.compare(0, 4, LITERAL("gray")) == 0) elem = 0x808080;
							else if (s == LITERAL("solid")) {
								node_ptr pos = tmp->first_node(LITERAL("fgColor"));
								if (pos) {
									if (theme_) elem = getrgb_from_attr(pos, theme_->palette());
									else elem = getrgb_from_attr(pos);
								}
							}
						}
					}
					fills_.push_back(elem);
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_borders
			/* ------------------------------------------------------------- */
			basic_style_container& xread_borders(node_ptr root) {
				if (!root) throw std::runtime_error("cannot find <borders> tag");
				
				node_ptr child = root->first_node(LITERAL("border"));
				if (!child) throw std::runtime_error("cannot find any <border> tags");
				
				for (; child; child = child->next_sibling()) {
					border_type elem;
					typename border_type::value_type tmp;
					
					// diagonal type
					size_type diag = 0;
					if (child->first_attribute(LITERAL("diagonalDown"))) diag = 0x0100;
					else if (child->first_attribute(LITERAL("diagonalUp"))) diag = 0x0200;
					
					// 1. left
					tmp.first = border_type::none;
					tmp.second = 0;
					this->xread_border(child->first_node(LITERAL("left")), tmp);
					elem.left(tmp);
					
					// 2. right
					tmp.first = border_type::none;
					this->xread_border(child->first_node(LITERAL("right")), tmp);
					elem.right(tmp);
					
					// 3. top
					tmp.first = border_type::none;
					tmp.second = 0;
					this->xread_border(child->first_node(LITERAL("top")), tmp);
					elem.top(tmp);
					
					// 4. bottom
					tmp.first = border_type::none;
					this->xread_border(child->first_node(LITERAL("bottom")), tmp);
					elem.bottom(tmp);
					
					// 5. diagonal
					tmp.first = border_type::none;
					this->xread_border(child->first_node(LITERAL("diagonal")), tmp);
					tmp.first |= diag;
					elem.diagonal(tmp);
					
					borders_.push_back(elem);
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_masters
			/* ------------------------------------------------------------- */
			basic_style_container& xread_masters(node_ptr root) {
				if (!root) throw std::runtime_error("cannot find <cellStyleXfs> tag");
				
				node_ptr child = root->first_node(LITERAL("xf"));
				if (!child) throw std::runtime_error("cannot find any <xf> tags");
				for (; child; child = child->next_sibling()) {
					value_type elem;
					this->xread_style(child, elem);
					masters_.push_back(elem);
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_styles
			/* ------------------------------------------------------------- */
			basic_style_container& xread_styles(node_ptr root) {
				if (!root) throw std::runtime_error("cannot find <cellXfs> tag");
				
				node_ptr child = root->first_node(LITERAL("xf"));
				if (!child) throw std::runtime_error("cannot find any <xf> tags");
				for (; child; child = child->next_sibling()) {
					value_type elem;
					attr_ptr attr = child->first_attribute(LITERAL("xfId"));
					if (!attr) throw std::runtime_error("cannot find xfId attribute");
					size_type index = clx::lexical_cast<size_type>(attr->value());
					//if (index >= masters_.size()) throw std::runtime_error("wrong xfId");
					if (index < masters_.size()) elem = masters_.at(index);
					this->xread_style(child, elem);
					v_.push_back(elem);
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_border
			/* ------------------------------------------------------------- */
			template <class Type>
			basic_style_container& xread_border(node_ptr root, Type& dest) {
				if (!root) throw std::runtime_error("cannot find <left/right/top/bottom> tag");
				
				attr_ptr attr = root->first_attribute(LITERAL("style"));
				if (attr) {
					size_type t = border_type::type(attr->value());
					if (t == border_type::unknown) throw std::runtime_error("unknown border type");
					dest.first = t;
				}
				
				node_ptr pos = root->first_node(LITERAL("color"));
				if (pos) {
					if (theme_) dest.second = getrgb_from_attr(pos, theme_->palette());
					else dest.second = getrgb_from_attr(pos);
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_style
			/* ------------------------------------------------------------- */
			template <class Type>
			basic_style_container& xread_style(node_ptr root, Type& dest) {
				if (!root) throw std::runtime_error("cannot find <xf> tag");
				
				// 1. alignment
				node_ptr pos = root->first_node(LITERAL("alignment"));
				if (pos) {
					attr_ptr attr = pos->first_attribute(LITERAL("horizontal"));
					if (attr && attr->value_size() > 0) dest.align(faml::officex::getalign(attr->value()));
					dest.valign(2);
					attr = pos->first_attribute(LITERAL("vertical"));
					if (attr && attr->value_size() > 0) dest.valign(faml::officex::getvalign(attr->value()));
					attr = pos->first_attribute(LITERAL("wrapText"));
					if (attr) dest.wrap(true);
					attr = pos->first_attribute(LITERAL("textRotation"));
					if (attr && attr->value_size() > 0) dest.rotate(clx::lexical_cast<size_type>(attr->value()));
				}
				
				// 2. format code
				attr_ptr attr = root->first_attribute(LITERAL("numFmtId"));
				if (attr && attr->value_size() > 0) {
					size_type index = clx::lexical_cast<size_type>(attr->value());
					string_type code;
					if (formats_.find(index) != formats_.end()) code = formats_[index];
					else format_code(code, index);
					dest.numfmt(code);
				}
				
				// 3. font
				attr = root->first_attribute(LITERAL("fontId"));
				if (attr && attr->value_size() > 0) {
					size_type index = clx::lexical_cast<size_type>(attr->value());
					if (index >= fonts_.size()) throw std::runtime_error("wrong fontId");
					dest.font(fonts_.at(index));
				}
				
				// 4. fill color
				attr = root->first_attribute(LITERAL("fillId"));
				if (attr) {
					size_type index = clx::lexical_cast<size_type>(attr->value());
					if (index >= fills_.size()) throw std::runtime_error("wrong fontId");
					dest.rgb(fills_.at(index));
				}
				
				// 5. border
				attr = root->first_attribute(LITERAL("borderId"));
				if (attr) {
					size_type index = clx::lexical_cast<size_type>(attr->value());
					if (index >= borders_.size()) throw std::runtime_error("wrong fontId");
					dest.border(borders_.at(index));
				}
				
				return *this;
			}
		};
	}
}
#endif // FAML_XLSX_STYLE_H
