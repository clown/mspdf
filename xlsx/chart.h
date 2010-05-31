/* ------------------------------------------------------------------------- */
/*
 *  xslx/chart.h
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
 *  Last-modified: Mon 22 Jun 2009 14:37:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_XLSX_CHART_H
#define FAML_XLSX_CHART_H

#include <stdexcept>
#include <string>
#include <vector>
#include "rapidxml/rapidxml.hpp"
#include "clx/lexical_cast.h"
#include "clx/literal.h"
#include "clx/utility.h"
#include "clx/tokenizer.h"
#include "../axis.h"
#include "../officex/theme.h"
#include "../officex/color.h"
#include "cell.h"
#include "shared_string.h"

namespace faml {
	namespace xlsx {
		/* ----------------------------------------------------------------- */
		//  basic_chart_element
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_chart_element {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef double value_type;
			typedef std::vector<value_type> container;
			typedef typename container::const_iterator const_iterator;
			typedef std::vector<std::vector<cell> > table_type;
			typedef basic_shared_string<CharT, Traits> sst_type;
			
			basic_chart_element() : name_(), data_(), ref_(NULL), sst_(NULL) {}
			
			template <class XMLNode>
			basic_chart_element(XMLNode* root) :
				name_(), data_(), ref_(NULL), sst_(NULL) {
				this->read(root);
			}
			
			virtual ~basic_chart_element() throw() {}
			
			template <class XMLNode>
			basic_chart_element& read(XMLNode* root) {
				if (!root) throw std::runtime_error("cannot find <c:ser> tag");
				
				this->xread_name(root->first_node(LITERAL("c:tx")));
				node_ptr pos = root->first_node(LITERAL("c:val"));
				if (!pos) pos = root->first_node(LITERAL("c:yVal"));
				this->xread_value(pos);
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			const string_type& name() const { return name_; }
			bool empty() const { return data_.empty(); }
			size_type size() const { return data_.size(); }
			const value_type& at(size_type pos) const { return data_.at(pos); }
			const value_type& operator[](size_type pos) const { return data_[pos]; }
			const_iterator begin() const { return data_.begin(); }
			const_iterator end() const { return data_.end(); }
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			void name(const string_type& cp) { name_ = cp; }
			void reference(const table_type& cp) { ref_ = &cp; }
			void sst(const sst_type& cp) { sst_ = &cp; }
			
		private:
			typedef rapidxml::xml_node<CharT>* node_ptr;
			typedef rapidxml::xml_attribute<CharT>* attr_ptr;
			
			string_type name_;
			container data_;
			const table_type* ref_;
			const sst_type* sst_;
			
			/* ------------------------------------------------------------- */
			//  xread_name
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_chart_element& xread_name(XMLNode* root) {
				if (!root) return *this;
				
				node_ptr p1 = root->first_node(LITERAL("c:strRef"));
				if (!p1) return *this;
				node_ptr p2 = p1->first_node(LITERAL("c:strCache"));
				//if (!p2) return *this;
				if (!p2) return this->xread_name_from_reference(root);
				node_ptr p3 = p2->first_node(LITERAL("c:pt"));
				if (!p3) return *this;
				node_ptr pos = p3->first_node(LITERAL("c:v"));
				if (!pos || pos->value_size() == 0) return *this;
				name_ = string_type(pos->value());
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_value
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_chart_element& xread_value(XMLNode* root) {
				if (!root) throw std::runtime_error("cannot find <c:val> tag");
				
				node_ptr p1 = root->first_node(LITERAL("c:numRef"));
				if (!p1) throw std::runtime_error("cannot find <c:numRef> tag");
				node_ptr p2 = p1->first_node(LITERAL("c:numCache"));
				if (!p2) throw std::runtime_error("cannot find <c:numCache> tag");
				
				node_ptr pos = p2->first_node(LITERAL("c:ptCount"));
				if (!pos) throw std::runtime_error("cannot find <c:ptCount> tag");
				attr_ptr attr = pos->first_attribute(LITERAL("val"));
				if (!attr && attr->value_size() == 0) throw std::runtime_error("cannot find val attribute");
				size_type count = clx::lexical_cast<size_type>(attr->value());
				if (count == 0) return this->xread_from_reference(root);
				data_.resize(count, 0);
				
				for (node_ptr child = p2->first_node(); child; child = child->next_sibling()) {
					if (string_type(child->name()) != LITERAL("c:pt")) continue;
					attr = child->first_attribute(LITERAL("idx"));
					if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find idx attribute");
					size_type idx = clx::lexical_cast<size_type>(attr->value());
					pos = child->first_node(LITERAL("c:v"));
					if (pos && pos->value_size() > 0) data_.at(idx) = clx::lexical_cast<value_type>(pos->value());
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_name_from_reference
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_chart_element& xread_name_from_reference(XMLNode* root) {
				if (!root) throw std::runtime_error("cannot find <c:tx>");
				if (!sst_) return *this;
				
				node_ptr parent = root->first_node(LITERAL("c:strRef"));
				if (!parent) throw std::runtime_error("cannot find <c:strRef>");
				node_ptr pos = parent->first_node(LITERAL("c:f"));
				if (!pos || pos->value_size() == 0) throw std::runtime_error("cannot find <c:f>");
				string_type expr(pos->value());
				
				clx::char_separator<CharT, Traits> sep(LITERAL("$:"));
				clx::basic_tokenizer<
					clx::char_separator<CharT, Traits>,
					std::basic_string<CharT, Traits> > tok(expr, sep);
				if (tok.size() < 3) return *this;
				std::pair<size_type, size_type> index = getindex(tok.at(1) + tok.at(2));
				if (!ref_) throw std::runtime_error("cannot have data");
				
				size_type data = ref_->at(index.first - 1).at(index.second - 1).data();
				name_.erase();
				if ((ref_->at(index.first - 1).at(index.second - 1).type() & cell::sst)) {
					for (size_type i = 0; i < sst_->at(data).size(); ++i) {
						name_ += sst_->at(data).at(i).data();
					}
				}
				else name_ = clx::lexical_cast<string_type>(data);
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_from_reference
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_chart_element& xread_from_reference(XMLNode* root) {
				if (!root) throw std::runtime_error("cannot find <c:val>");
				
				node_ptr parent = root->first_node(LITERAL("c:numRef"));
				if (!parent) throw std::runtime_error("cannot find <c:numRef>");
				node_ptr pos = parent->first_node(LITERAL("c:f"));
				if (!pos || pos->value_size() == 0) throw std::runtime_error("cannot find <c:f>");
				string_type expr(pos->value());
				
				data_.clear();
				clx::char_separator<CharT, Traits> sep(LITERAL("$:"));
				clx::basic_tokenizer<
					clx::char_separator<CharT, Traits>,
					std::basic_string<CharT, Traits> > tok(expr, sep);
				if (tok.size() < 5) throw std::runtime_error("bad chart expression");
				//if (tok.size() < 5) return *this;
				std::pair<size_type, size_type> from = getindex(tok.at(1) + tok.at(2));
				std::pair<size_type, size_type> to = getindex(tok.at(3) + tok.at(4));
				if (!ref_) throw std::runtime_error("cannot have data");
				if (from.first == to.first) {
					size_type row = from.first - 1;
					//for (size_type i = from.second - 1; i < to.second - 1; ++i) {
					for (size_type i = from.second - 1; i < to.second; ++i) {
						data_.push_back(ref_->at(row).at(i).data());
					}
				}
				else if (from.second == to.second) {
					size_type col = from.second - 1;
					//for (size_type i = from.first - 1; i < to.first - 1; ++i) {
					for (size_type i = from.first - 1; i < to.first; ++i) {
						data_.push_back(ref_->at(i).at(col).data());
					}
				}
				else throw std::runtime_error("bad chart range");
				return *this;
			}
		};
		
		/* ----------------------------------------------------------------- */
		//  basic_chart
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_chart {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef basic_chart_element<CharT, Traits> element_type;
			typedef faml::axis axis_type;
			typedef std::vector<element_type> container;
			typedef typename container::const_iterator const_iterator;
			typedef std::vector<string_type> label_container;
			typedef std::pair<bool, size_type> rgb_type;
			typedef faml::officex::basic_theme<CharT, Traits> theme_type;
			typedef std::vector<std::vector<cell> > table_type;
			typedef basic_shared_string<CharT, Traits> sst_type;
			
			enum {
				// chart type
				unknown			= 0x00000000,
				bar				= 0x00000001,
				line			= 0x00000002,
				pie				= 0x00000003,
				area			= 0x00000004,
				scatter			= 0x00000005,
				bar3d_box		= 0x00000006,
				bar3d_cylinder	= 0x00000007,
				bar3d_cone		= 0x00000008,
				bar3d_pyramid	= 0x00000009,
				row3d_box		= 0x0000000a,
				row3d_cylinder	= 0x0000000b,
				row3d_cone		= 0x0000000c,
				row3d_pyramid	= 0x0000000d,
				pie3d			= 0x00000012,
				pie3d_explosion	= 0x00000013,
				//options
				inner			= 0x00010000,
				marked			= 0x00020000,
				grid			= 0x00040000,
				subgrid			= 0x00080000
			};
			
			basic_chart() :
				type_(0), option_(0), v_(), label_(),
				border_(rgb_type(false, 0xffffff)),
				background_(rgb_type(false, 0xffffff)),
				plot_(rgb_type(false, 0xffffff)),
				bgplot_(rgb_type(false, 0xffffff)),
				fgleg_(rgb_type(false, 0xffffff)),
				bgleg_(rgb_type(false, 0xffffff)),
				legpos_(0x012),
				valax_(),
				theme_(NULL), ref_(NULL), sst_(NULL) {}
			
			template <class Ch, class Tr>
			basic_chart(std::basic_istream<Ch, Tr>& in) :
				type_(0), option_(0), v_(), label_(),
				border_(rgb_type(false, 0xffffff)),
				background_(rgb_type(false, 0xffffff)),
				plot_(rgb_type(false, 0xffffff)),
				bgplot_(rgb_type(false, 0xffffff)),
				fgleg_(rgb_type(false, 0xffffff)),
				bgleg_(rgb_type(false, 0xffffff)),
				legpos_(0x012),
				valax_(),
				theme_(NULL), ref_(NULL), sst_(NULL) {
				this->read(in);
			}
			
			virtual ~basic_chart() throw() {}
			
			template <class Ch, class Tr>
			basic_chart& read(std::basic_istream<Ch, Tr>& in) {
				std::vector<char_type> s;
				clx::read(in, s);
				s.push_back(0);
				
				//rapidxml::xml_document<char_type> doc; // <- ??? compile error!!
				rapidxml::xml_document<char> doc;
				doc.parse<0>(reinterpret_cast<char_type*>(&s.at(0)));
				
				node_ptr root = doc.first_node(LITERAL("c:chartSpace"));
				if (!root) throw std::runtime_error("cannot find <c:chartSpace> (root) tag");
				this->xread_style(root);
				
				node_ptr p1 = root->first_node(LITERAL("c:chart"));
				if (!p1) throw std::runtime_error("cannot find <c:chart> tag");
				node_ptr pos = p1->first_node(LITERAL("c:plotArea"));
				if (!pos) throw std::runtime_error("cannot find <c:plotArea> tag");
				this->xread_layout(pos);
				this->xread_axis(pos);
				this->xread_legend(pos);
				
				node_ptr child = pos->first_node();
				while (child) {
					string_type s(child->name());
					if (s.find(LITERAL("Chart")) != string_type::npos) break;
					child = child->next_sibling();
				}
				if (!child) return *this;
				
				this->xread_type(child);
				this->xread_data(child);
				this->xread_label(child, v_.back());
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			size_type type() const { return type_; }
			size_type option() const { return option_; }
			const label_container& labels() const { return label_; }
			bool empty() const { return v_.empty(); }
			size_type size() const { return v_.size(); }
			const element_type& at(size_type pos) const { return v_.at(pos); }
			const element_type& operator[](size_type pos) const { return v_[pos]; }
			const_iterator begin() const { return v_.begin(); }
			const_iterator end() const { return v_.end(); }
			
			const rgb_type& border() const { return border_; }
			const rgb_type& background() const { return background_; }
			const rgb_type& plot_border() const { return plot_; }
			const rgb_type& plot_area() const { return bgplot_; }
			const rgb_type& legend_border() const { return fgleg_; }
			const rgb_type& legend_area() const { return bgleg_; }
			size_type legend_position() const { return legpos_; }
			
			const axis_type& value_axis() const { return valax_; }
			
			/* ------------------------------------------------------------- */
			//  Access methods (set).
			/* ------------------------------------------------------------- */
			void theme(const theme_type* cp) { theme_ = cp; }
			void reference(const table_type& cp) { ref_ = &cp; }
			void sst(const sst_type& cp) { sst_ = &cp; }
			
		private:
			typedef rapidxml::xml_node<CharT>* node_ptr;
			typedef rapidxml::xml_attribute<CharT>* attr_ptr;
			
			size_type type_;
			size_type option_;
			container v_;
			label_container label_;
			rgb_type border_;
			rgb_type background_;
			rgb_type plot_;
			rgb_type bgplot_;
			rgb_type fgleg_;
			rgb_type bgleg_;
			size_type legpos_;
			axis_type valax_;
			
			const theme_type* theme_;
			const table_type* ref_;
			const sst_type* sst_;
			
			/* ------------------------------------------------------------- */
			//  xread_type
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_chart& xread_type(XMLNode* root) {
				// 2D
				if (string_type(root->name()) == LITERAL("c:barChart")) {
					type_ = 0;
					node_ptr tmp = root->first_node(LITERAL("c:barDir"));
					if (tmp) {
						attr_ptr attr = tmp->first_attribute(LITERAL("val"));
						if (attr && attr->value_size() > 0) {
							if (string_type(attr->value()) == LITERAL("col")) type_ = 0;
							else if (string_type(attr->value()) == LITERAL("bar")) type_ = 1;
						}
					}
				}
				else if (string_type(root->name()) == LITERAL("c:lineChart")) {
					type_ = 2;
					option_ |= marked;
				}
				else if (string_type(root->name()) == LITERAL("c:pieChart")) type_ = 3;
				else if (string_type(root->name()) == LITERAL("c:areaChart")) type_ = 4;
				else if (string_type(root->name()) == LITERAL("c:scatterChart")) type_ = 5;
				
				// bar3Dchart
				else if (string_type(root->name()) == LITERAL("c:bar3DChart")) {
					type_ = 0;
					node_ptr tmp1 = root->first_node(LITERAL("c:barDir"));
					node_ptr tmp2 = root->first_node(LITERAL("c:shape"));
					if (tmp1 && tmp2) {
						attr_ptr attr1 = tmp1->first_attribute(LITERAL("val"));
						attr_ptr attr2 = tmp2->first_attribute(LITERAL("val"));
						if (attr1 && attr2 && attr1->value_size() > 0 && attr2->value_size() > 0) {
							if (string_type(attr1->value()) == LITERAL("col")) {
								if (string_type(attr2->value()) == LITERAL("box")) type_ = 6;
								else if (string_type(attr2->value()) == LITERAL("cylinder")) type_ = 7;
								else if (string_type(attr2->value()) == LITERAL("cone")) type_ = 8;
								else if (string_type(attr2->value()) == LITERAL("pyramid")) type_ = 9;
							}
							else if (string_type(attr1->value()) == LITERAL("bar")) {
								if (string_type(attr2->value()) == LITERAL("box")) type_ = 10;
								else if (string_type(attr2->value()) == LITERAL("cylinder")) type_ = 11;
								else if (string_type(attr2->value()) == LITERAL("cone")) type_ = 12;
								else if (string_type(attr2->value()) == LITERAL("pyramid")) type_ = 13;
							}
						}
					}
				}
				
				// line3Dchart
				else if (string_type(root->name()) == LITERAL("c:line3DChart")) type_ = 0;
				
				// pie3Dchart
				else if (string_type(root->name()) == LITERAL("c:pie3DChart")) {
					type_ = 0;
					node_ptr tmp1 = root->first_node(LITERAL("c:ser"));
					node_ptr tmp2 = tmp1->first_node(LITERAL("c:explosion"));
					if (!tmp2) {
						type_ = 18;
					}
					else{
						type_ = 19;
					}
				}
				else if (string_type(root->name()) == LITERAL("c:area3DChart")) type_ = 0;
				else if (string_type(root->name()) == LITERAL("c:scatter3DChart")) type_ = 0;
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_layout
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_chart& xread_layout(XMLNode* root) {
				if (!root) throw std::runtime_error("cannot find <c:plotArea>");
				
				node_ptr p1 = root->first_node(LITERAL("c:layout"));
				if (!p1) return *this;
				
				node_ptr p2 = p1->first_node(LITERAL("c:manualLayout"));
				if (!p2) return *this;
				
				node_ptr pos = p2->first_node(LITERAL("c:layoutTarget"));
				if (pos) {
					attr_ptr attr = pos->first_attribute(LITERAL("val"));
					if (attr && attr->value_size() > 0) {
						string_type val(attr->value());
						if (val == LITERAL("inner")) option_ |= inner;
					}
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_data
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_chart& xread_data(XMLNode* root) {
				if (!root) throw std::runtime_error("cannot find <c:barChart> tag");
				
				size_type i = 1;
				for (node_ptr child = root->first_node(); child; child = child->next_sibling()) {
					if (string_type(child->name()) != LITERAL("c:ser")) continue;
					element_type elem;
					if (ref_ != NULL) elem.reference(*ref_);
					if (sst_ != NULL) elem.sst(*sst_);
					elem.read(child);
					if (elem.name().empty() && !(type_ == 0x03 || type_ == 0x12 || type_ == 0x13)) {
						std::basic_stringstream<CharT, Traits> ss;
						ss << LITERAL("系列") << i;
						elem.name(ss.str());
					}
					v_.push_back(elem);
					++i;
					
					node_ptr pos = child->first_node(LITERAL("c:marker"));
					if (pos) {
						node_ptr tmp = pos->first_node(LITERAL("c:symbol"));
						if (tmp) {
							attr_ptr attr = tmp->first_attribute(LITERAL("val"));
							if (attr && attr->value_size() > 0) {
								string_type val(attr->value());
								if (val == LITERAL("none")) option_ &= ~marked;
							}
						}
					}
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_label
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_chart& xread_label(XMLNode* root, const element_type& v) {
				if (!root) throw std::runtime_error("cannot find <c:***Chart> tag");
				
				node_ptr p1 = root->first_node(LITERAL("c:ser"));
				if (!p1) throw std::runtime_error("cannot find <c:ser> tag (for getting label)");
				node_ptr p2 = p1->first_node(LITERAL("c:cat"));
				if (!p2) return this->xread_default_label(root, v);
				node_ptr p3 = p2->first_node(LITERAL("c:strRef"));
				if (!p3) return this->xread_default_label(root, v);
				node_ptr p4 = p3->first_node(LITERAL("c:strCache"));
				if (!p3) return this->xread_label_from_reference(root, v);
				node_ptr pos = p4->first_node(LITERAL("c:ptCount"));
				if (!pos) throw std::runtime_error("cannot find <c:ptCount> tag");
				attr_ptr attr = pos->first_attribute(LITERAL("val"));
				if (!attr && attr->value_size() == 0) throw std::runtime_error("cannot find val attribute");
				size_type n = clx::lexical_cast<size_type>(attr->value());
				if (n == 0) return this->xread_label_from_reference(root, v);
				label_.resize(n, string_type());
				
				for (node_ptr child = p4->first_node(); child; child = child->next_sibling()) {
					if (string_type(child->name()) != LITERAL("c:pt")) continue;
					attr = child->first_attribute(LITERAL("idx"));
					if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find idx attribute");
					size_type idx = clx::lexical_cast<size_type>(attr->value());
					pos = child->first_node(LITERAL("c:v"));
					if (pos && pos->value_size() > 0) label_.at(idx) = string_type(pos->value());
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_label
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_chart& xread_label_from_reference(XMLNode* root, const element_type& v) {
				if (!root) throw std::runtime_error("cannot find <c:***Chart> tag");
				if (!ref_ || !sst_) return this->xread_default_label(root, v);
				
				node_ptr p1 = root->first_node(LITERAL("c:ser"));
				if (!p1) throw std::runtime_error("cannot find <c:ser> tag (for getting label)");
				node_ptr p2 = p1->first_node(LITERAL("c:cat"));
				if (!p2) return this->xread_default_label(root, v);
				node_ptr p3 = p2->first_node(LITERAL("c:strRef"));
				if (!p3) return this->xread_default_label(root, v);
				
				node_ptr pos = p3->first_node(LITERAL("c:f"));
				if (!pos || pos->value_size() == 0) return this->xread_default_label(root, v);
				string_type expr(pos->value());
				
				clx::char_separator<CharT, Traits> sep(LITERAL("$:"));
				clx::basic_tokenizer<
					clx::char_separator<CharT, Traits>,
					std::basic_string<CharT, Traits> > tok(expr, sep);
				if (tok.size() < 5) return this->xread_default_label(root, v);
				
				label_.clear();
				std::pair<size_type, size_type> from = getindex(tok.at(1) + tok.at(2));
				std::pair<size_type, size_type> to = getindex(tok.at(3) + tok.at(4));
				if (from.first == to.first) {
					size_type row = from.first - 1;
					//for (size_type i = from.second - 1; i < to.second - 1; ++i) {
					for (size_type i = from.second - 1; i < to.second; ++i) {
						string_type val;
						if ((ref_->at(row).at(i).type() & cell::sst)) {
							size_type data = ref_->at(row).at(i).data();
							for (size_type j = 0; j < sst_->at(data).size(); ++j) {
								val += sst_->at(data).at(j).data();
							}
						}
						else val = clx::lexical_cast<string_type>(ref_->at(row).at(i).data());
						label_.push_back(val);
					}
				}
				else if (from.second == to.second) {
					size_type col = from.second - 1;
					//for (size_type i = from.first - 1; i < to.first - 1; ++i) {
					for (size_type i = from.first - 1; i < to.first; ++i) {
						string_type val;
						if ((ref_->at(i).at(col).type() & cell::sst)) {
							size_type data = ref_->at(i).at(col).data();
							for (size_type j = 0; j < sst_->at(data).size(); ++j) {
								val += sst_->at(data).at(j).data();
							}
						}
						else val = clx::lexical_cast<string_type>(ref_->at(i).at(col).data());
						label_.push_back(val);
					}
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_default_label
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_chart& xread_default_label(XMLNode* root, const element_type& v) {
				label_.clear();
				for (size_type i = 0; i < v.size(); ++i) {
					label_.push_back(clx::lexical_cast<string_type>(i + 1));
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_style
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_chart& xread_style(XMLNode* root) {
				if (!root) throw std::runtime_error("cannot find <c:chartSpace>");
				
				node_ptr pos = root->first_node(LITERAL("c:spPr"));
				if (pos) {
					node_ptr child = pos->first_node(LITERAL("a:solidFill"));
					if (child) {
						size_type clr = 0;
						if (theme_) clr = faml::officex::getrgb(child, theme_->palette());
						else clr = faml::officex::getrgb(child);
						if (clr != static_cast<size_type>(-1)) {
							background_.first = true;
							background_.second = clr;
						}
					}
					
					node_ptr tmp = pos->first_node(LITERAL("a:ln"));
					if (tmp) {
						child = tmp->first_node(LITERAL("a:solidFill"));
						if (child) {
							size_type clr = 0;
							if (theme_) clr = faml::officex::getrgb(child, theme_->palette());
							else clr = faml::officex::getrgb(child);
							if (clr != static_cast<size_type>(-1)) {
								border_.first = true;
								border_.second = clr;
							}
						}
					}
				}
				
				node_ptr p1 = root->first_node(LITERAL("c:chart"));
				if (!p1) throw std::runtime_error("cannot find <c:chart>");
				node_ptr p2 = p1->first_node(LITERAL("c:plotArea"));
				if (!p2) throw std::runtime_error("cannot find <c:plotArea>");
				pos = p2->first_node(LITERAL("c:spPr"));
				if (pos) {
					node_ptr child = pos->first_node(LITERAL("a:solidFill"));
					if (child) {
						size_type clr = 0;
						if (theme_) clr = faml::officex::getrgb(child, theme_->palette());
						else clr = faml::officex::getrgb(child);
						if (clr != static_cast<size_type>(-1)) {
							bgplot_.first = true;
							bgplot_.second = clr;
						}
						
						node_ptr tmp = pos->first_node(LITERAL("a:ln"));
						if (tmp) {
							child = tmp->first_node(LITERAL("a:solidFill"));
							if (child) {
								size_type clr = 0;
								if (theme_) clr = faml::officex::getrgb(child, theme_->palette());
								else clr = faml::officex::getrgb(child);
								if (clr != static_cast<size_type>(-1)) {
									plot_.first = true;
									plot_.second = clr;
								}
							}
						}
					}
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_axis
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_chart& xread_axis(XMLNode* root) {
				if (!root) throw std::runtime_error("cannot find <c:plotArea>");
				node_ptr pos = root->first_node(LITERAL("c:valAx"));
				if (!pos) return *this;
				
				node_ptr child = pos->first_node(LITERAL("c:majorUnit"));
				if (child) {
					attr_ptr attr = child->first_attribute(LITERAL("val"));
					if (attr && attr->value_size() > 0) valax_.unit(clx::lexical_cast<double>(attr->value()));
				}
				else valax_.unit(-1.0);
				
				child = pos->first_node(LITERAL("c:majorGridlines"));
				if (child) option_ |= grid;
				
				child = pos->first_node(LITERAL("c:scaling"));
				if (!child) return *this;
				node_ptr p2 = child->first_node(LITERAL("c:orientation"));
				if (p2) {
					attr_ptr attr = p2->first_attribute(LITERAL("val"));
					if (attr && attr->value_size() > 0) {
						string_type val(attr->value());
						if (val == LITERAL("minMax")) valax_.type(1);
						else if (val == LITERAL("maxMin")) valax_.type(2);
						else valax_.type(1); // currently implemented
					}
				}
				
				p2 = child->first_node(LITERAL("c:min"));
				if (p2) {
					attr_ptr attr = p2->first_attribute(LITERAL("val"));
					if (attr && attr->value_size() > 0) valax_.min(clx::lexical_cast<double>(attr->value()));
				}
				
				p2 = child->first_node(LITERAL("c:max"));
				if (p2) {
					attr_ptr attr = p2->first_attribute(LITERAL("val"));
					if (attr && attr->value_size() > 0) valax_.max(clx::lexical_cast<double>(attr->value()));
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_legend
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_chart& xread_legend(XMLNode* root) {
				if (!root) throw std::runtime_error("cannof find <c:plotArea>");
				
				node_ptr p1 = root->first_node(LITERAL("c:legend"));
				if (!p1) return *this;
				
				node_ptr p2 = p1->first_node(LITERAL("c:legendPos"));
				if (p2) {
					int align = 0x02; // right
					int valign = 0x10; // center
					attr_ptr attr = p2->first_attribute(LITERAL("val"));
					if (attr && attr->value_size() > 0) {
						align = faml::officex::getalign(attr->value());
					}
					legpos_ = (valign | align);
				}
				
				node_ptr pos = p1->first_node(LITERAL("c:spPr"));
				if (pos) {
					node_ptr child = pos->first_node(LITERAL("a:solidFill"));
					if (child) {
						size_type clr = 0;
						if (theme_) clr = faml::officex::getrgb(child, theme_->palette());
						else clr = faml::officex::getrgb(child);
						if (clr != static_cast<size_type>(-1)) {
							bgleg_.first = true;
							bgleg_.second = clr;
						}
						
						node_ptr tmp = pos->first_node(LITERAL("a:ln"));
						if (tmp) {
							child = tmp->first_node(LITERAL("a:solidFill"));
							if (child) {
								size_type clr = 0;
								if (theme_) clr = faml::officex::getrgb(child, theme_->palette());
								else clr = faml::officex::getrgb(child);
								if (clr != static_cast<size_type>(-1)) {
									fgleg_.first = true;
									bgleg_.second = clr;
								}
							}
						}
					}
				}
				
				return *this;
			}
		};
	}
}

#endif // FAML_XLSX_CHART_H
