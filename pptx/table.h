/* ------------------------------------------------------------------------- */
/*
 *  pptx/table.h
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
 *  Last-modified: Mon 15 Jun 2009 14:43:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PPTX_TABLE_H
#define FAML_PPTX_TABLE_H

#include <stdexcept>
#include <string>
#include <vector>
#include "rapidxml/rapidxml.hpp"
#include "clx/literal.h"
#include "clx/lexical_cast.h"
#include "../officex/txbox.h"
#include "../officex/color.h"
#include "../officex/utility.h"
#include "../coordinate.h"

namespace faml {
	namespace pptx {
		using faml::pdf::coordinate;
		
		/* ----------------------------------------------------------------- */
		//  basic_cell
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_cell {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef faml::officex::basic_paragraph<CharT, Traits> value_type;
			typedef std::vector<value_type> container;
			typedef typename container::const_iterator const_iterator;
			
			basic_cell() : v_(), width_(0.0), fill_(-1),
			weight_left_(0.75), weight_right_(0.75), weight_top_(0.75), weight_bottom_(0.75) {}
			
			template <class XMLNode>
			basic_cell(XMLNode* root) :
				v_(), width_(0.0), fill_(-1), valign_(0),
				weight_left_(0.75), weight_right_(0.75), weight_top_(0.75), weight_bottom_(0.75) {
				this->read(root);
			}
			
			virtual ~basic_cell() throw() {}
			
			template <class XMLNode>
			basic_cell& read(XMLNode* root) {
				if (!root) throw std::runtime_error("cannot find <a:tc> tag");
				
				this->xread_style(root->first_node(LITERAL("a:tcPr")));
				
				node_ptr pos = root->first_node(LITERAL("a:txBody"));
				for (node_ptr child = pos->first_node(); child; child = child->next_sibling()) {
					if (string_type(child->name()) != LITERAL("a:p")) continue;
					value_type elem;
					elem.read(child);
					v_.push_back(elem);
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			bool empty() const { return v_.empty(); }
			size_type size() const { return v_.size(); }
			const value_type& at(size_type pos) const { return v_.at(pos); }
			const value_type& operator[](size_type pos) const { return v_[pos]; }
			const_iterator begin() const { return v_.begin(); }
			const_iterator end() const { return v_.end(); }
			
			// additional data.
			double width() const { return width_; }
			size_type rgb() const { return fill_; }
			size_type valign() const { return valign_; }
			double weight_left() const { return weight_left_; }
			double weight_right() const { return weight_right_; }
			double weight_top() const { return weight_top_; }
			double weight_bottom() const { return weight_bottom_; }
			
			/* ------------------------------------------------------------- */
			//  Access methods (set).
			/* ------------------------------------------------------------- */
			void width(double cp) { width_ = cp; }
			void rgb(size_type cp) { fill_ = cp; }
			
		private:
			typedef rapidxml::xml_node<CharT>* node_ptr;
			typedef rapidxml::xml_attribute<CharT>* attr_ptr;
			
			container v_;
			double width_;
			size_type fill_;
			size_type valign_;
			double weight_left_;
			double weight_right_;
			double weight_top_;
			double weight_bottom_;
			
			template <class XMLNode>
			basic_cell& xread_style(XMLNode* root) {
				if (!root) throw std::runtime_error("cannot find <a:tcPr> tag");
				
				/*
				node_ptr pos = root->first_node(LITERAL("w:tcW"));
				if (pos) {
					attr_ptr attr = pos->first_attribute(LITERAL("w:w"));
					if (attr && attr->value_size() > 0) {
						width_ = clx::lexical_cast<double>(attr->value());
					}
				}
				*/
				
				attr_ptr attr = root->first_attribute(LITERAL("anchor"));
				if (attr && attr->value_size() > 0) {
					valign_ = faml::officex::getvalign(attr->value());
				}
				
				node_ptr pos = root->first_node(LITERAL("a:solidFill"));
				if (pos) fill_ = faml::officex::getrgb(pos);
				
				node_ptr pos_l = root->first_node(LITERAL("a:lnL"));
				if (pos_l) {
					attr_ptr attr_l = pos_l->first_attribute(LITERAL("w"));
					if (attr_l && attr_l->value_size() > 0) weight_left_ = clx::lexical_cast<double>(attr_l->value());
				}
				
				node_ptr pos_r = root->first_node(LITERAL("a:lnR"));
				if (pos_r) {
					attr_ptr attr_r = pos_r->first_attribute(LITERAL("w"));
					if (attr_r && attr_r->value_size() > 0) weight_right_ = clx::lexical_cast<double>(attr_r->value());
				}
				
				node_ptr pos_t = root->first_node(LITERAL("a:lnT"));
				if (pos_t) {
					attr_ptr attr_t = pos_t->first_attribute(LITERAL("w"));
					if (attr_t && attr_t->value_size() > 0) weight_top_ = clx::lexical_cast<double>(attr_t->value());
				}
				
				node_ptr pos_b = root->first_node(LITERAL("a:lnB"));
				if (pos_b) {
					attr_ptr attr_b = pos_b->first_attribute(LITERAL("w"));
					if (attr_b && attr_b->value_size() > 0) weight_bottom_ = clx::lexical_cast<double>(attr_b->value());
				}
				
				return *this;
			}
		};
		
		/* ----------------------------------------------------------------- */
		//  basic_table
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_table {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef basic_cell<CharT, Traits> value_type;
			typedef std::pair<double, std::vector<value_type> > row_type;
			typedef std::vector<row_type> container;
			typedef typename container::const_iterator const_iterator;
			
			basic_table() : origin_(), width_(0.0), height_(0.0), v_() {}
			
			template <class XMLNode>
			basic_table(XMLNode* root) : origin_(),  width_(0.0), height_(0.0), v_() {
				this->read(root);
			}
			
			virtual ~basic_table() throw() {}
			
			template <class XMLNode>
			basic_table& read(XMLNode* root) {
				if (!root) throw std::runtime_error("cannot find <p:graphicFrame> tag");
				
				this->xread_form(root->first_node(LITERAL("p:xfrm")));
				
				node_ptr p1 = root->first_node(LITERAL("a:graphic"));
				if (!p1) throw std::runtime_error("cannot find <a:graphic> tag");
				node_ptr p2 = p1->first_node(LITERAL("a:graphicData"));
				if (!p2) throw std::runtime_error("cannot find <a:graphicData> tag");
				node_ptr p3 = p2->first_node(LITERAL("a:tbl"));
				
				node_ptr pos = p3->first_node(LITERAL("a:tblGrid"));
				if (!pos) throw std::runtime_error("cannot find <a:tblGrid> tag");
				std::vector<double> widths;
				for (node_ptr child = pos->first_node(); child; child = child->next_sibling()) {
					if (string_type(child->name()) != LITERAL("a:gridCol")) continue;
					attr_ptr attr = child->first_attribute(LITERAL("w"));
					if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find w attribute");
					widths.push_back(clx::lexical_cast<double>(attr->value()));
				}
				
				for (node_ptr child = p3->first_node(); child; child = child->next_sibling()) {
					if (string_type(child->name()) != LITERAL("a:tr")) continue;
					row_type elem;
					this->xread_rowdata(child, elem, widths);
					v_.push_back(elem);
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			const coordinate& origin() const { return origin_; }
			bool empty() const { return v_.empty(); }
			size_type size() const { return v_.size(); }
			const row_type& at(size_type pos) const { return v_.at(pos); }
			const row_type& operator[](size_type pos) const { return v_[pos]; }
			const_iterator begin() const { return v_.begin(); }
			const_iterator end() const { return v_.end(); }
			
			/* ------------------------------------------------------------- */
			//  Access methods (set).
			/* ------------------------------------------------------------- */
			void background(size_type cp) { background_ = cp; }
			
		private:
			typedef rapidxml::xml_node<CharT>* node_ptr;
			typedef rapidxml::xml_attribute<CharT>* attr_ptr;
			
			coordinate origin_;
			double width_;
			double height_;
			container v_;
			size_type background_;
			
			/* ------------------------------------------------------------- */
			//  xread_form
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_table& xread_form(XMLNode* root) {
				if (!root) throw std::runtime_error("cannot find <a:xfrm> tag");
				
				node_ptr pos = root->first_node(LITERAL("a:off"));
				if (!pos) throw std::runtime_error("cannot find <a:off> tag");
				
				attr_ptr attr = pos->first_attribute(LITERAL("x"));
				if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find x attribute");
				double x = clx::lexical_cast<double>(attr->value());
				
				attr = pos->first_attribute(LITERAL("y"));
				if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find y attribute");
				double y = clx::lexical_cast<double>(attr->value());
				origin_ = coordinate(x, y);
				
				pos = root->first_node(LITERAL("a:ext"));
				if (!pos) throw std::runtime_error("cannot find <a:ext>");
				
				attr = pos->first_attribute(LITERAL("cx"));
				if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find cx attribute");
				width_ = clx::lexical_cast<double>(attr->value());
				
				attr = pos->first_attribute(LITERAL("cy"));
				if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find cy attribute");
				height_ = clx::lexical_cast<double>(attr->value());
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_rowdata
			/* ------------------------------------------------------------- */
			template <class XMLNode, class Container, class WidthArray>
			basic_table& xread_rowdata(XMLNode* root, Container& dest, const WidthArray& widths) {
				if (!root) throw std::runtime_error("cannot find <a:tr> tag");
				
				double w = width_ / widths.size();
				bool eqw = true;
				for (size_type i = 1; i < widths.size(); ++i) {
					if (widths.at(i) != widths.at(i - 1)) {
						eqw = false;
						break;
					}
				}
				
				attr_ptr attr = root->first_attribute(LITERAL("h"));
				if (attr && attr->value_size() > 0) dest.first = clx::lexical_cast<double>(attr->value());
				
				size_type i = 0;
				for (node_ptr child = root->first_node(); child; child = child->next_sibling()) {
					if (string_type(child->name()) != LITERAL("a:tc")) continue;
					value_type elem;
					if (i >= widths.size()) throw std::runtime_error("width list is too small");
					if (!eqw) elem.width(widths.at(i));
					else elem.width(w);
					elem.rgb(background_);
					elem.read(child);
					dest.second.push_back(elem);
					++i;
				}
				
				return *this;
			}
		};
	}
}

#endif // FAML_PPTX_TABLE_H
