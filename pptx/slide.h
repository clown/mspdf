/* ------------------------------------------------------------------------- */
/*
 *  pptx/slide.h
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
 *  Last-modified: Tue 23 Jun 2009 20:57:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PPTX_SLIDE_H
#define FAML_PPTX_SLIDE_H

#include <stdexcept>
#include <string>
#include <vector>
#include "rapidxml/rapidxml.hpp"
#include "clx/lexical_cast.h"
#include "clx/literal.h"
#include "clx/utility.h"
#include "../officex/shape.h"
#include "../xlsx/chart.h"
#include "table.h"
#include "layout.h"
#include "master.h"

namespace faml {
	namespace pptx {
		/* ----------------------------------------------------------------- */
		//  basic_slide
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_slide {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef faml::officex::basic_shape<CharT, Traits> value_type;
			typedef std::vector<value_type> container;
			typedef typename container::const_iterator const_iterator;
			typedef typename container::iterator iterator;
			typedef faml::officex::basic_theme<CharT, Traits> theme_type;
			typedef basic_layout<CharT, Traits> layout_type;
			typedef basic_master<CharT, Traits> master_type;
			
			typedef basic_table<CharT, Traits> table_type;
			typedef std::vector<table_type> table_container;
			
			typedef faml::xlsx::basic_chart<CharT, Traits> chart_type;
			typedef std::vector<chart_type> chart_container;
			
			basic_slide() :
				v_(), background_(), bgimage_(), theme_(NULL), layout_() {}
			
			template <class Ch, class Tr>
			basic_slide(std::basic_istream<Ch, Tr>& in) :
				v_(), background_(), bgimage_(), theme_(NULL), layout_() {
				this->read(in);
			}
			
			virtual ~basic_slide() throw() {}
			
			template <class Ch, class Tr>
			basic_slide& read(std::basic_istream<Ch, Tr>& in) {
				std::vector<char_type> s;
				clx::read(in, s);
				s.push_back(0); // make null terminated string.
				
				//rapidxml::xml_document<char_type> doc; // <- ??? compile error!!
				rapidxml::xml_document<char> doc;
				doc.parse<0>(reinterpret_cast<char_type*>(&s.at(0)));
				
				node_ptr root = doc.first_node(LITERAL("p:sld"));
				if (!root) throw std::runtime_error("cannot find <p:sld> (root) tag");
				node_ptr parent = root->first_node("p:cSld");
				if (!parent) throw std::runtime_error("cannot find <p:cSld> tag");
				
				bgimage_ = master_.bgimage();
				node_ptr pos = parent->first_node(LITERAL("p:bg"));
				if (pos) this->xread_background(pos);
				else {
					background_ = master_.rgbs();
					angle_ = master_.angle();
				}
				
				this->xread_drawing(parent->first_node("p:spTree"));
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			const std::vector<size_type> rgbs() const { return background_; }
			size_type rgb(size_type pos = 0) const { return background_.at(pos); }
			const string_type& bgimage() const { return bgimage_; }
			double angle() const { return angle_; }
			bool empty() const { return v_.empty(); }
			size_type size() const { return v_.size(); }
			const value_type& at(size_type pos) const { return v_.at(pos); }
			const value_type& operator[](size_type pos) const { return v_[pos]; }
			const_iterator begin() const { return v_.begin(); }
			const_iterator end() const { return v_.end(); }
			iterator begin() { return v_.begin(); }
			iterator end() { return v_.end(); }
			
			const layout_type& layout() const { return layout_; }
			layout_type& layout() { return layout_; }
			const table_container& tbls() const { return table_; }
			table_container& tbls() { return table_; }
			const chart_container& charts() const { return chart_; }
			chart_container& charts() { return chart_; }
			
			/* ------------------------------------------------------------- */
			//  Access methods (set).
			/* ------------------------------------------------------------- */
			void theme(const theme_type* cp) { theme_ = cp; }
			void master(const master_type& cp) { master_ = cp; }
			void layout(const layout_type& cp) { layout_ = cp; }
			
		private:
			typedef rapidxml::xml_node<CharT>* node_ptr;
			typedef rapidxml::xml_attribute<CharT>* attr_ptr;
			
			container v_;
			table_container table_;
			chart_container chart_;
			std::vector<size_type> background_;
			string_type bgimage_;
			double angle_;
			
			const theme_type* theme_;
			master_type master_;
			layout_type layout_;
			
			/* ------------------------------------------------------------- */
			//  xread_background
			/* ------------------------------------------------------------- */
			basic_slide& xread_background(node_ptr root) {
				if (!root) throw std::runtime_error("cannot find <p:bg> tag");
				
				node_ptr pos = root->first_node(LITERAL("p:bgRef"));
				if (!pos) {
					node_ptr parent = root->first_node(LITERAL("p:bgPr"));
					if (!parent) return *this;
					//pos = parent->first_node(LITERAL("a:blipFill"));
					//if (pos) this->xread_bgimage(pos, dm);
					pos = parent->first_node(LITERAL("a:gradFill"));
					if (pos) return this->xread_grad(pos);
					pos = parent->first_node(LITERAL("a:solidFill"));
					if (!pos) return *this;
					//if (!pos) throw std::runtime_error("cannot find <a:solidFill> tag");
				}
				
				if (theme_) background_.push_back(faml::officex::getrgb(pos, theme_->palette(), this->layout().clrmap()));
				else background_.push_back(faml::officex::getrgb(pos));
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_grad
			/* ------------------------------------------------------------- */
			basic_slide& xread_grad(node_ptr root) {
				if (!root) throw std::runtime_error("cannot find <a:gradFill>");
				node_ptr pos = root->first_node(LITERAL("a:gsLst"));
				if (!pos) throw std::runtime_error("cannot find <a:gsLst>");
				for (node_ptr child = pos->first_node(); child; child = child->next_sibling()) {
					if (string_type(child->name()) != LITERAL("a:gs")) continue;
					size_type tmp = 0xff000000;
					if (theme_) tmp = faml::officex::getrgb(child, theme_->palette(), this->layout().clrmap());
					else tmp = faml::officex::getrgb(child);
					if (tmp > 0xffffff) tmp = 0xffffff;
					background_.push_back(tmp);
				}
				
				angle_ = 0.0;
				pos = root->first_node(LITERAL("a:lin"));
				if (!pos) return *this;
				attr_ptr attr = pos->first_attribute(LITERAL("ang"));
				if (attr && attr->value_size() > 0) {
					angle_ = faml::officex::deg60k(clx::lexical_cast<double>(attr->value()));
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_shape
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_slide& xread_shape(XMLNode* root) {
				if (!root) return *this;
				
				if (string_type(root->name()) == LITERAL("p:graphicFrame")) {
					node_ptr p1 = root->first_node(LITERAL("a:graphic"));
					if (!p1) return *this;
					node_ptr p2 = p1->first_node(LITERAL("a:graphicData"));
					if (!p2) return *this;
					node_ptr pos = p2->first_node();
					if (!pos) return *this;
					if (string_type(pos->name()) == LITERAL("a:tbl")) this->xread_table(root);
					else if (string_type(pos->name()) == LITERAL("c:chart")) this->xread_chart(root);
					return *this;
				}
				
				value_type elem(this->layout().clrmap());
				size_type idx = this->xassign_style(root, elem);
				size_type type = this->xgettype(root);
				if (layout_.txstyles().find(idx) != layout_.txstyles().end()) {
					elem.style(layout_.txstyles()[idx]);
				}
				else if (type == 1) elem.style(master_.title_style());
				else if (type == 2) elem.style(master_.body_style());
				else elem.style(master_.other_style());
				if (theme_) elem.theme(theme_);
				//elem.background(master_.rgb());
				elem.read(root, LITERAL("p"));
				v_.push_back(elem);
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_drawing
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_slide& xread_drawing(XMLNode* root) {
				if (!root) throw std::runtime_error("cannot find <p:spTree> tag");
				
				for (node_ptr child = root->first_node(); child; child = child->next_sibling()) {
					if (string_type(child->name()) == LITERAL("p:grpSp")) {
						this->xread_group_shape(child);
						continue;
					}
					
					if (string_type(child->name()) != LITERAL("p:sp") &&
						string_type(child->name()) != LITERAL("p:cxnSp") &&
						string_type(child->name()) != LITERAL("p:pic") &&
						string_type(child->name()) != LITERAL("p:graphicFrame")) {
						continue;
					}
					
					this->xread_shape(child);
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_group_shape
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_slide& xread_group_shape(XMLNode* root) {
				if (!root) throw std::runtime_error("cannot find <p:grpSp>");
				
				size_type first = v_.size();
				node_ptr p1 = root->first_node(LITERAL("p:grpSpPr"));
				if (!p1) throw std::runtime_error("cannot find <p:grpSpPr>");
				node_ptr pos = p1->first_node(LITERAL("a:xfrm"));
				if (!pos) throw std::runtime_error("cannot find <a:xfrm>");
				
				// <a:off>
				node_ptr tmp = pos->first_node(LITERAL("a:off"));
				if (!tmp) throw std::runtime_error("cannot find <a:off>");
				attr_ptr attr = tmp->first_attribute(LITERAL("x"));
				if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find <a:off x>");
				double x = clx::lexical_cast<double>(attr->value());
				attr = tmp->first_attribute(LITERAL("y"));
				if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find <a:off y>");
				double y = clx::lexical_cast<double>(attr->value());
				
				// <a:ext>
				tmp = pos->first_node(LITERAL("a:ext"));
				if (!tmp) throw std::runtime_error("cannot find <a:ext>");
				attr = tmp->first_attribute(LITERAL("cx"));
				if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find <a:ext cx>");
				double cx = clx::lexical_cast<double>(attr->value());
				attr = tmp->first_attribute(LITERAL("cy"));
				if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find <a:ext cy>");
				double cy = clx::lexical_cast<double>(attr->value());
				
				// <a:chOff>
				tmp = pos->first_node(LITERAL("a:chOff"));
				if (!tmp) throw std::runtime_error("cannot find <a:chOff>");
				attr = tmp->first_attribute(LITERAL("x"));
				if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find <a:chOff x>");
				double chx = clx::lexical_cast<double>(attr->value());
				attr = tmp->first_attribute(LITERAL("y"));
				if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find <a:chOff y>");
				double chy = clx::lexical_cast<double>(attr->value());
				
				// <a:chExt>
				tmp = pos->first_node(LITERAL("a:chExt"));
				if (!tmp) throw std::runtime_error("cannot find <a:chExt>");
				attr = tmp->first_attribute(LITERAL("cx"));
				if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find <a:chExt cx>");
				double chcx = clx::lexical_cast<double>(attr->value());
				if (chcx <= 0.0) chcx = 1;
				attr = tmp->first_attribute(LITERAL("cy"));
				if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find <a:chExt cy>");
				double chcy = clx::lexical_cast<double>(attr->value());
				if (chcy <= 0.0) chcy = 1;
				
				this->xread_drawing(root);
				
				double scalex = cx / chcx;
				double scaley = cy / chcy;
				for (size_type i = first; i < v_.size(); ++i) {
					double dx = v_.at(i).origin().x() - chx;
					double dy = v_.at(i).origin().y() - chy;
					coordinate o(x + dx * scalex, y + dy * scaley);
					v_.at(i).origin(o);
					
					double w = v_.at(i).width() * scalex;
					v_.at(i).width(w);
					
					double h = v_.at(i).height() * scaley;
					v_.at(i).height(h);
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_table
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_slide& xread_table(XMLNode* root) {
				if (!root) return *this;
				
				table_type elem;
				elem.background(master_.rgb());
				elem.read(root);
				table_.push_back(elem);
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_chart
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_slide& xread_chart(XMLNode* root) {
				if (!root) return *this;
				
				value_type elem(this->layout().clrmap());
				elem.background(master_.rgb());
				
				node_ptr parent = root->first_node(LITERAL("p:xfrm"));
				if (!parent) return *this;
				node_ptr pos = parent->first_node(LITERAL("a:off"));
				if (!pos) return *this;
				attr_ptr attr = pos->first_attribute(LITERAL("x"));
				if (!attr || attr->value_size() == 0) return *this;
				double x = clx::lexical_cast<double>(attr->value());
				attr = pos->first_attribute(LITERAL("y"));
				if (!attr || attr->value_size() == 0) return *this;
				double y = clx::lexical_cast<double>(attr->value());
				elem.origin(coordinate(x, y));
				
				pos = parent->first_node(LITERAL("a:ext"));
				if (!pos) return *this;
				attr = pos->first_attribute(LITERAL("cx"));
				if (!attr || attr->value_size() == 0) return *this;
				elem.width(clx::lexical_cast<double>(attr->value()));
				attr = pos->first_attribute(LITERAL("cy"));
				if (!attr || attr->value_size() == 0) return *this;
				elem.height(clx::lexical_cast<double>(attr->value()));
				
				node_ptr p1 = root->first_node(LITERAL("a:graphic"));
				if (!p1) return *this;
				node_ptr p2 = p1->first_node(LITERAL("a:graphicData"));
				if (!p2) return *this;
				pos = p2->first_node(LITERAL("c:chart"));
				if (!pos) return *this;
				attr = pos->first_attribute(LITERAL("r:id"));
				if (!attr || attr->value_size() == 0) return *this;
				elem.reference(string_type(attr->value()));
				v_.push_back(elem);
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xassign_style
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			size_type xassign_style(XMLNode* root, value_type& dest) {
				size_type idx = 0;
				if (!root || string_type(root->name()) != LITERAL("p:sp")) return 0;
				
				// get shape placeholder index.
				node_ptr p1 = root->first_node(LITERAL("p:nvSpPr"));
				if (!p1) throw std::runtime_error("cannot find <p:nvSpPr> tag");
				node_ptr p2 = p1->first_node(LITERAL("p:nvPr"));
				if (!p2) throw std::runtime_error("cannot find <p:nvPr> tag");
				node_ptr pos = p2->first_node(LITERAL("p:ph"));
				if (pos) {
					attr_ptr attr = pos->first_attribute(LITERAL("idx"));
					if (attr && attr->value_size() > 0) idx = clx::lexical_cast<size_type>(attr->value());
					typename layout_type::shstyle_map::const_iterator it;
					it = layout_.shstyles().find(idx);
					if (it != layout_.shstyles().end()) {
						dest = it->second;
						dest.texts().clear();
					}
				}
				
				return idx;
			}
			
			/* ------------------------------------------------------------- */
			//  xgettype
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			size_type xgettype(XMLNode* root) {
				if (!root) throw std::runtime_error("cannot find shape tag");
				node_ptr p1 = root->first_node(LITERAL("p:nvSpPr"));
				if (!p1 || !p1->first_node()) return 0;
				node_ptr p2 = p1->first_node(LITERAL("p:nvPr"));
				if (!p2 || !p2->first_node()) return 0;
				node_ptr pos = p2->first_node(LITERAL("p:ph"));
				if (!pos) return 0;
				
				/*
				 * What is txstyle type is determined ?
				 * We currently determine the type based on the ph index.
				 */
				size_type idx = 0;
				size_type dest = 0;
				attr_ptr attr = pos->first_attribute(LITERAL("idx"));
				if (attr && attr->value_size() > 0) idx = clx::lexical_cast<size_type>(attr->value());
				if (idx == 0) dest = 1;
				else if (idx == 1) dest = 2;
				
				return dest;
			}
		};
	}
}

#endif // FAML_PPTX_SLIDE_H
