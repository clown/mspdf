/* ------------------------------------------------------------------------- */
/*
 *  xslx/worksheet.h
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
#ifndef FAML_XLSX_WORKSHEET_H
#define FAML_XLSX_WORKSHEET_H

#include <stdexcept>
#include <string>
#include <vector>
#include "rapidxml/rapidxml.hpp"
#include "clx/lexical_cast.h"
#include "clx/literal.h"
#include "clx/utility.h"
#include "cell.h"
#include "utility.h"
#include "drawing.h"
#include "chart.h"

namespace faml {
	namespace xlsx {
		/* ----------------------------------------------------------------- */
		/*
		 *  basic_widthlist
		 *
		 *  basic_widthlist is the helper class when constructing
		 *  basic_worksheet. Users does not usually use the class.
		 */
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_widthlist {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef std::map<size_type, double> width_map;
			typedef std::map<size_type, bool> hidden_map;
			typedef width_map::const_iterator const_iterator;
			
			basic_widthlist() : v_(), hidden_() {}
			
			template <class XMLNode>
			basic_widthlist(XMLNode* root) :
				v_(), hidden_() {
				this->read(root);
			}
			
			template <class XMLNode>
			basic_widthlist& read(XMLNode* root) {
				typedef rapidxml::xml_attribute<CharT>* attr_ptr;
				
				if (!root) return *this;
				for (XMLNode* child = root->first_node(LITERAL("col"));
					child; child = child->next_sibling()) {
					size_type l = 0;
					attr_ptr attr = child->first_attribute(LITERAL("min"));
					if (!attr || attr->value_size() == 0) continue;
					l = clx::lexical_cast<size_type>(attr->value());
					
					size_type u = 0;
					attr = child->first_attribute(LITERAL("max"));
					if (!attr || attr->value_size() == 0) continue;
					u = clx::lexical_cast<size_type>(attr->value());
					
					double w = 0.0;
					attr = child->first_attribute(LITERAL("width"));
					if (!attr || attr->value_size() == 0) continue;
					w = clx::lexical_cast<double>(attr->value());
					
					bool hidden = false;
					attr = child->first_attribute(LITERAL("hidden"));
					if (attr && attr->value_size() > 0) {
						int val = clx::lexical_cast<int>(attr->value());
						if (val > 0) hidden = true;
					}
					
					for (size_type i = l; i <= u; ++i) {
						v_[i] = w;
						hidden_[i] = hidden;
					}
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			double operator[](size_type pos) { return v_[pos]; }
			bool empty() const { return v_.empty(); }
			size_type size() const { return v_.size(); }
			bool exist(size_type pos) const { return v_.find(pos) != v_.end(); }
			const_iterator begin() const { return v_.begin(); }
			const_iterator end() const { return v_.end(); }
			
			bool is_hidden(size_type pos) const {
				typename hidden_map::const_iterator it = hidden_.find(pos);
				if (it == hidden_.end()) return false;
				return it->second;
			}
			
		private:
			width_map v_;
			hidden_map hidden_;
		};
		
		/* ----------------------------------------------------------------- */
		/*
		 *  basic_mergelist
		 *
		 *  basic_mergelist is the helper class when constructing
		 *  basic_worksheet. Users does not usually use the class.
		 */
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_mergelist {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef std::pair<size_type, size_type> subvalue;
			typedef std::pair<subvalue, subvalue> value_type;
			typedef std::vector<value_type> container;
			
			enum {
				none		= 0x000,
				row_beg		= 0x010,
				row_merge	= 0x020,
				row_end		= 0x040,
				col_beg		= 0x100,
				col_merge	= 0x200,
				col_end		= 0x400
			};
			
			basic_mergelist() : v_() {}
			
			template <class XMLNode>
			basic_mergelist(XMLNode* root) :
				v_() {
				this->read(root);
			}
			
			template <class XMLNode>
			basic_mergelist& read(XMLNode* root) {
				typedef rapidxml::xml_attribute<CharT>* attr_ptr;
				
				if (!root) return *this;
				for (XMLNode* child = root->first_node(LITERAL("mergeCell"));
					child; child = child->next_sibling()) {
					attr_ptr attr = child->first_attribute(LITERAL("ref"));
					if (!attr || attr->value_size() == 0) continue;
					
					string_type s(attr->value());
					size_type pos = s.find(LITERAL(':'));
					if (pos == string_type::npos) continue;
					
					string_type s1(s.substr(0, pos));
					subvalue from = getindex(s1);
					
					string_type s2(s.substr(pos + 1));
					subvalue to = getindex(s2);
					
					v_.push_back(std::make_pair(from, to));
				}
				
				return *this;
			}
			
			size_type is_merged(size_type row, size_type column) {
				size_type dest = 0;
				for (size_type i = 0; i < v_.size(); ++i) {
					subvalue from = v_.at(i).first;
					subvalue to = v_.at(i).second;
					
					if (from.first <= row && row <= to.first &&
						from.second <= column && column <= to.second) {
						if (row == from.first && row == to.first) dest &= 0xf0f;
						else if (row == from.first) dest |= row_beg;
						else if (row == to.first) dest |= row_end;
						else if (from.first < row && row < to.first) dest |= row_merge;
						
						if (column == from.second && column == to.second) dest &= 0x0ff;
						else if (column == from.second) dest |= col_beg;
						else if (column == to.second) dest |= col_end;
						if (from.second < column && column < to.second) dest |= col_merge;
						
						break;
					}
				}
				
				return dest;
			}
			
		private:
			container v_;
		};
		
		/* ----------------------------------------------------------------- */
		//  basic_worksheet
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_worksheet {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef cell value_type;
			typedef std::vector<value_type> subcontainer;
			typedef std::vector<subcontainer> container;
			typedef container::const_iterator const_iterator;
			typedef basic_shape_container<CharT, Traits> shape_container;
			typedef std::map<size_type, double> size_map;
			typedef basic_chart<CharT, Traits> chart_type;
			typedef std::vector<chart_type> chart_container;
			
			basic_worksheet() : v_(), draws_() {}
			
			template <class Ch, class Tr>
			basic_worksheet(std::basic_istream<Ch, Tr>& in) :
				v_(), draws_(), reference_() {
				this->read(in);
			}
			
			template <class Ch, class Tr>
			basic_worksheet& read(std::basic_istream<Ch, Tr>& in) {
				std::vector<char_type> s;
				clx::read(in, s);
				s.push_back(0); // make null terminated string.
				
				//rapidxml::xml_document<char_type> doc; // <- ??? compile error!!
				rapidxml::xml_document<char> doc;
				doc.parse<0>(reinterpret_cast<char_type*>(&s.at(0)));
				
				return this->xread_data(doc.first_node(LITERAL("worksheet")));
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
			
			const string_type& reference() const { return reference_; }
			double wdefault() const { return width_; }
			double hdefault() const { return height_; }
			const size_map& widths() const { return widths_; }
			size_map& widths() { return widths_; }
			const size_map& heights() const { return heights_; }
			size_map& heights() { return heights_; }
			
			/* ------------------------------------------------------------- */
			//  Additional data.
			/* ------------------------------------------------------------- */
			const container& data() const { return v_; }
			container& data() { return v_; }
			const shape_container& drawings() const { return draws_; }
			shape_container& drawings() { return draws_; }
			const chart_container& charts() const { return charts_; }
			chart_container& charts() { return charts_; }
			
		private:
			typedef basic_mergelist<CharT, Traits> mergelist;
			typedef basic_widthlist<CharT, Traits> widthlist;
			typedef rapidxml::xml_node<CharT>* node_ptr;
			typedef rapidxml::xml_attribute<CharT>* attr_ptr;
			
			container v_;
			shape_container draws_;
			chart_container charts_;
			double width_;
			double height_;
			widthlist ws_; // will deprecated in the future.
			size_map widths_;
			size_map heights_;
			string_type reference_;
			
			/* ------------------------------------------------------------- */
			//  xread_data
			/* ------------------------------------------------------------- */
			basic_worksheet& xread_data(node_ptr root) {
				if (!root) throw std::runtime_error("cannot find <worksheet> (root) tag");
				
				// 1. width and height
				width_ = 8.0,
				height_ = 13.5;
				attr_ptr attr = root->first_attribute(LITERAL("defaultColWidth"));
				if (attr) width_ = clx::lexical_cast<double>(attr->value());
				attr = root->first_attribute(LITERAL("defaultRowHeight"));
				if (attr) height_ = clx::lexical_cast<double>(attr->value());
				ws_.read(root->first_node(LITERAL("cols")));
				widths_.insert(ws_.begin(), ws_.end());
				
				// 2. merge list
				mergelist merge(root->first_node(LITERAL("mergeCells")));
				
				node_ptr pos = root->first_node(LITERAL("sheetData"));
				if (!pos) return *this;
				
				size_type i = 1;
				for (node_ptr child = pos->first_node(LITERAL("row")); child; child = child->next_sibling()) {
					subcontainer elem;
					double h = height_;
					size_type index = 0;
					attr = child->first_attribute(LITERAL("r"));
					if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find r attribute");
					index = clx::lexical_cast<size_type>(attr->value());
					
					attr = child->first_attribute(LITERAL("ht"));
					if (attr && attr->value_size() > 0) {
						h = clx::lexical_cast<double>(attr->value());
						heights_[index] = h;
					}
					this->xread_rowdata(child, elem, merge, h);
					
					while (i < index) {
						subcontainer dummy;
						v_.push_back(dummy);
						++i;
					}
					v_.push_back(elem);
					++i;
				}
				
				pos = root->first_node(LITERAL("drawing"));
				if (pos) {
					attr = pos->first_attribute(LITERAL("r:id"));
					if (attr && attr->value_size()) reference_ = string_type(attr->value());
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_rowdata
			/* ------------------------------------------------------------- */
			template <class Container>
			basic_worksheet& xread_rowdata(node_ptr root, Container& dest, mergelist merge, double h) {
				if (!root) throw std::runtime_error("cannot find <row> tag");
				size_type i = 1;
				for (node_ptr child = root->first_node();
					child; child = child->next_sibling(), ++i) {
					if (string_type(child->name()) != LITERAL("c")) continue;
					value_type elem;
					attr_ptr attr = child->first_attribute(LITERAL("r"));
					if (!attr || attr->value_size() == 0) {
						throw std::runtime_error("cannot find r (row & col index) attribute");
					}
					std::pair<size_type, size_type> idx = getindex(attr->value());
					
					while (i < idx.second) {
						value_type dummy;
						if (widths_.find(i) != widths_.end()) dummy.width(widths_[i]);
						else dummy.width(width_);
						dummy.height(h);
						size_type m = merge.is_merged(idx.first, i);
						if (ws_.is_hidden(i)) m |= value_type::hidden;
						dummy.type(m);
						dest.push_back(dummy);
						++i;
					}
					
					attr = child->first_attribute(LITERAL("s"));
					if (attr && attr->value_size() > 0) elem.style(clx::lexical_cast<size_type>(attr->value()));
					else elem.style(0);
					
					node_ptr pos = child->first_node(LITERAL("v"));
					//if (!pos) std::runtime_error("cannot find <v> tag (cell data)");
					
					size_type m = merge.is_merged(idx.first, idx.second);
					if (ws_.is_hidden(idx.second)) m |= value_type::hidden;
					if (!pos || pos->value_size() == 0) elem.type(value_type::empty | m);
					else {
						attr = child->first_attribute(LITERAL("t"));
						if (attr && string_type(attr->value()) == LITERAL("s")) {
							elem.type(value_type::sst | m);
							elem.data(clx::lexical_cast<double>(pos->value()));
						}
						else {
							node_ptr sib = child->first_node(LITERAL("f"));
							if (sib && sib->value_size() == 0) elem.type(value_type::empty | m);
							else {
								elem.type(value_type::value | m);
								elem.data(clx::lexical_cast<double>(pos->value()));
							}
						}
					}
					
					if (widths_.find(idx.second) != widths_.end()) elem.width(widths_[idx.second]);
					else elem.width(width_);
					elem.height(h);
					dest.push_back(elem);
				}
				
				attr_ptr attr = root->first_attribute(LITERAL("r"));
				if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find <row r>");
				size_type row = clx::lexical_cast<size_type>(attr->value());
				attr = root->first_attribute(LITERAL("spans"));
				size_type idx = 0;
				if (attr && attr->value_size() > 0) {
					string_type tmp(attr->value());
					tmp.erase(0, 2);
					idx = clx::lexical_cast<size_type>(tmp);
				}
				
				while (i <= idx) {
					value_type dummy;
					if (widths_.find(i) != widths_.end()) dummy.width(widths_[i]);
					else dummy.width(width_);
					dummy.height(h);
					size_type m = merge.is_merged(row, i);
					if (ws_.is_hidden(i)) m |= value_type::hidden;
					dummy.type(m);
					dest.push_back(dummy);
					++i;
				}
				
				return *this;
			}
		};
	}
}

#endif // FAML_XLSX_WORKSHEET_H
