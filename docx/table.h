/* ------------------------------------------------------------------------- */
/*
 *  docx/table.h
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
#ifndef FAML_DOCX_TABLE_H
#define FAML_DOCX_TABLE_H

#include <stdexcept>
#include <string>
#include <vector>
#include "rapidxml/rapidxml.hpp"
#include "clx/literal.h"
#include "clx/lexical_cast.h"
#include "../border.h"
#include "paragraph.h"
#include "bullet.h"

namespace faml {
	namespace docx {
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
			typedef basic_paragraph<CharT, Traits> value_type;
			typedef std::vector<value_type> container;
			typedef typename container::const_iterator const_iterator;
			
			basic_cell() :
				v_(), width_(0.0), height_(0.0), fill_(0xffffff) {}
			
			template <class XMLNode, class DocManager>
			basic_cell(XMLNode* root, DocManager& dm) :
				v_(), width_(0.0), height_(0.0),
				fill_(0xffffff) {
				this->read(root, dm);
			}
			
			virtual ~basic_cell() throw() {}
			
			template <class XMLNode, class DocManager>
			basic_cell& read(XMLNode* root, DocManager& dm) {
				if (!root) throw std::runtime_error("cannot find <w:tc> tag");
				
				this->xread_style(root->first_node(LITERAL("w:tcPr")));
				for (node_ptr child = root->first_node(); child; child = child->next_sibling()) {
					if (string_type(child->name()) != LITERAL("w:p")) continue;
					value_type elem;
					elem.style(dm.style());
					elem.bullets(dm.bullet());
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
			double height() const { return height_; }
			size_type rgb() const { return fill_; }
			
			/* ------------------------------------------------------------- */
			//  Access methods (set).
			/* ------------------------------------------------------------- */
			void width(double cp) { width_ = cp; }
			void height(double cp) { height_ = cp; }
			
		private:
			typedef rapidxml::xml_node<CharT>* node_ptr;
			typedef rapidxml::xml_attribute<CharT>* attr_ptr;
			
			container v_;
			double width_;
			double height_;
			size_type fill_;
			
			template <class XMLNode>
			basic_cell& xread_style(XMLNode* root) {
				if (!root) throw std::runtime_error("cannot find <a:tcPr> tag");
				
				node_ptr pos = root->first_node(LITERAL("w:tcW"));
				if (pos) {
					attr_ptr attr = pos->first_attribute(LITERAL("w:w"));
					if (attr && attr->value_size() > 0) {
						width_ = clx::lexical_cast<double>(attr->value());
					}
				}
				
				pos = root->first_node(LITERAL("w:shd"));
				if (pos) {
					attr_ptr attr = pos->first_attribute(LITERAL("w:fill"));
					if (attr && attr->value_size() > 0) {
						fill_ = clx::lexical_cast<size_type>(attr->value(), std::ios::hex) & 0x00ffffff;
					}
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
			typedef std::vector<value_type> subcontainer;
			typedef std::vector<subcontainer> container;
			typedef typename container::const_iterator const_iterator;
			typedef basic_bullet<CharT, Traits> bullet_type;
			typedef faml::border border_type;
			
			basic_table() : v_(), top_(), bottom_(), left_(), right_() {}
			
			template <class XMLNode, class DocManager>
			basic_table(XMLNode* root, DocManager& dm) :
				v_(), top_(), bottom_(), left_(), right_() {
				this->read(root, dm);
			}
			
			virtual ~basic_table() throw() {}
			
			template <class XMLNode, class DocManager>
			basic_table& read(XMLNode* root, DocManager& dm) {
				if (!root) throw std::runtime_error("cannot find <w:tbl> tag");
				
				node_ptr pos = root->first_node(LITERAL("w:tblPr"));
				if (pos) this->xread_property(pos, dm);
				
				for (node_ptr child = root->first_node(); child; child = child->next_sibling()) {
					if (string_type(child->name()) != LITERAL("w:tr")) continue;
					subcontainer elem;
					this->xread_rowdata(child, elem, dm);
					v_.push_back(elem);
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			bool empty() const { return v_.empty(); }
			size_type size() const { return v_.size(); }
			const subcontainer& at(size_type pos) const { return v_.at(pos); }
			const subcontainer& operator[](size_type pos) const { return v_[pos]; }
			const_iterator begin() const { return v_.begin(); }
			const_iterator end() const { return v_.end(); }
			
			const border_type& top() const { return top_; }
			const border_type& bottom() const { return bottom_; }
			const border_type& left() const { return left_; }
			const border_type& right() const { return right_; }
			
		private:
			typedef rapidxml::xml_node<CharT>* node_ptr;
			typedef rapidxml::xml_attribute<CharT>* attr_ptr;
			
			container v_;
			border_type top_;
			border_type bottom_;
			border_type left_;
			border_type right_;
			
			/* ------------------------------------------------------------- */
			//  xread_rowdata
			/* ------------------------------------------------------------- */
			template <class XMLNode, class Container, class DocManager>
			basic_table& xread_rowdata(XMLNode* root, Container& dest, DocManager& dm) {
				if (!root) throw std::runtime_error("cannot find <w:tr> tag");
				
				std::vector<double> hs;
				node_ptr pos = root->first_node(LITERAL("w:trPr"));
				if (pos) {
					for (node_ptr child = pos->first_node(); child; child = child->next_sibling()) {
						if (string_type(child->name()) != LITERAL("w:trHeight")) continue;
						attr_ptr attr = child->first_attribute(LITERAL("w:val"));
						if (attr && attr->value_size() > 0) {
							hs.push_back(clx::lexical_cast<double>(attr->value()));
						}
					}
				}
				
				size_type i = 0;
				for (node_ptr child = root->first_node(); child; child = child->next_sibling()) {
					if (string_type(child->name()) != LITERAL("w:tc")) continue;
					value_type elem;
					if (i < hs.size()) elem.height(hs.at(i));
					elem.read(child, dm);
					dest.push_back(elem);
					++i;
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_property
			/* ------------------------------------------------------------- */
			template <class XMLNode, class DocManager>
			basic_table& xread_property(XMLNode* root, DocManager& dm) {
				if (!root) throw std::runtime_error("cannot find <w:tblPr>");
				
				node_ptr parent = root->first_node(LITERAL("w:tblBorders"));
				if (parent) {
					node_ptr pos = parent->first_node(LITERAL("w:top"));
					if (pos) this->xread_border(pos, top_, dm);
					
					pos = parent->first_node(LITERAL("w:left"));
					if (pos) this->xread_border(pos, left_, dm);
					
					pos = parent->first_node(LITERAL("w:bottom"));
					if (pos) this->xread_border(pos, bottom_, dm);
					
					pos = parent->first_node(LITERAL("w:right"));
					if (pos) this->xread_border(pos, right_, dm);
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_border
			/* ------------------------------------------------------------- */
			template <class XMLNode, class Type, class DocManager>
			basic_table& xread_border(XMLNode* root, Type& dest, DocManager& dm) {
				if (!root) throw std::runtime_error("cannot find border tag");
				
				attr_ptr attr = root->first_attribute(LITERAL("w:val"));
				if (attr && attr->value_size() > 0) {
					faml::officex::getborder(string_type(attr->value()), dest);
				}
				
				attr = root->first_attribute(LITERAL("w:color"));
				if (attr && attr->value_size() > 0) {
					size_type rgb = clx::lexical_cast<size_type>(attr->value(), std::ios_base::hex);
					dest.fill(color(rgb));
				}
				
				return *this;
			}
		};
	}
}

#endif // FAML_DOCX_TABLE_H
