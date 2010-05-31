/* ------------------------------------------------------------------------- */
/*
 *  pptx/layout.h
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
 *  Last-modified: Sun 14 Jun 2009 23:07:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PPTX_LAYOUT_H
#define FAML_PPTX_LAYOUT_H

#include <stdexcept>
#include <string>
#include <map>
#include "rapidxml/rapidxml.hpp"
#include "clx/literal.h"
#include "clx/utility.h"
#include "../officex/shape.h"
#include "../officex/reference.h"
#include "master.h"

namespace faml {
	namespace pptx {
		/* ----------------------------------------------------------------- */
		//  basic_layout
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_layout {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef faml::officex::basic_shape<CharT, Traits> value_type;
			typedef std::vector<value_type> container;
			typedef typename container::const_iterator const_iterator;
			typedef value_type shstyle_type;
			typedef std::map<size_type, shstyle_type> shstyle_map;
			typedef faml::officex::basic_txstyle<CharT, Traits> txstyle_type;
			typedef std::vector<txstyle_type> txstyle_container;
			typedef std::map<size_type, txstyle_container> txstyle_map;
			typedef std::map<string_type, string_type> color_map;
			
			basic_layout() : path_(), v_(), master_(), sh_(), tx_() {}
			
			template <class InStream, class DocManager>
			basic_layout(InStream& in, DocManager& dm) :
				path_(), v_(), sh_(), tx_() {
				this->read(in, dm);
			}
			
			/* ------------------------------------------------------------- */
			//  read
			/* ------------------------------------------------------------- */
			template <class InStream, class DocManager>
			basic_layout& read(InStream& in, DocManager& dm) {
				path_ = in.path();
				
				std::vector<char_type> s;
				clx::read(in, s);
				s.push_back(0);
				
				//rapidxml::xml_document<char_type> doc; // <- ??? compile error!!
				rapidxml::xml_document<char> doc;
				doc.parse<0>(reinterpret_cast<char_type*>(&s.at(0)));
				
				node_ptr root = doc.first_node(LITERAL("p:sldLayout"));
				if (!root) throw std::runtime_error("cannot find <p:sldLayout>");
				
				this->xread_master(dm);
				this->xassign_master(root, dm);
				
				node_ptr pos = root->first_node(LITERAL("p:cSld"));
				if (!pos) throw std::runtime_error("cannot find <p:cSld>");
				this->xread(pos->first_node(LITERAL("p:spTree")), dm);
				
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
			const shstyle_map& shstyles() const { return sh_; }
			const txstyle_map& txstyles() const { return tx_; }
			const color_map& clrmap() const { return master_.clrmap(); }
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			shstyle_map& shstyles() { return sh_; }
			txstyle_map& txstyles() { return tx_; }
			color_map& clrmap() { return master_.clrmap(); }
			
		private:
			typedef rapidxml::xml_node<CharT>* node_ptr;
			typedef rapidxml::xml_attribute<CharT>* attr_ptr;
			typedef basic_master<CharT, Traits> master_type;
			
			string_type path_;
			container v_;
			master_type master_;
			shstyle_map sh_;
			txstyle_map tx_;
			
			/* ------------------------------------------------------------- */
			//  xread
			/* ------------------------------------------------------------- */
			template <class XMLNode, class DocManager>
			basic_layout& xread(XMLNode* root, DocManager& dm) {
				if (!root) throw std::runtime_error("cannnot find <p:spTree>");
				
				std::map<string_type, string_type> ref;
				std::basic_stringstream<CharT, Traits> ss;
				ss << LITERAL("ppt/slideLayouts/_rels");
				ss << path_.substr(path_.find_last_of(LITERAL("/")));
				ss << LITERAL(".rels");
				typename DocManager::storage_type::iterator fp = dm.storage().find(ss.str());
				if (fp != dm.storage().end()) faml::officex::read_reference(*fp, ref);
				
				for (node_ptr child = root->first_node(); child; child = child->next_sibling()) {
					if (string_type(child->name()) == LITERAL("p:grpSp")) {
						this->xread_shape_group(child, dm, ref);
						continue;
					}
					
					if (string_type(child->name()) != LITERAL("p:sp") &&
						string_type(child->name()) != LITERAL("p:cxnSp") &&
						string_type(child->name()) != LITERAL("p:pic")) {
						continue;
					}
					
					node_ptr p1 = child->first_node(LITERAL("p:nvSpPr"));
					if (!p1) p1 = child->first_node(LITERAL("p:nvPicPr"));
					if (!p1) p1 = child->first_node(LITERAL("p:cNvCxnSpPr"));
					if (!p1) throw std::runtime_error("cannot find <p:nvSpPr> tag (in layout)");
					node_ptr p2 = p1->first_node(LITERAL("p:nvPr"));
					if (!p2) throw std::runtime_error("cannot find <p:nvPr> tag (in layout)");
					node_ptr pos = p2->first_node(LITERAL("p:ph"));
					if (!pos) this->xread_shape(child, dm, ref);
					else {
						size_type idx = 0;
						attr_ptr attr = pos->first_attribute(LITERAL("idx"));
						if (attr && attr->value_size() > 0) idx = clx::lexical_cast<size_type>(attr->value());
						this->xread_shstyle(child, idx, dm);
						this->xread_txstyle(child->first_node(LITERAL("p:txBody")), idx, dm);
					}
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_shape_tree
			/* ------------------------------------------------------------- */
			template <class XMLNode, class DocManager, class Container>
			basic_layout& xread_shape_tree(XMLNode* root, DocManager& dm, Container& ref) {
				if (!root) throw std::runtime_error("cannot find <p:spTree> tag");
				
				for (node_ptr child = root->first_node(); child; child = child->next_sibling()) {
					if (string_type(child->name()) == LITERAL("p:grpSp")) {
						this->xread_shape_group(child, dm, ref);
						continue;
					}
					
					if (string_type(child->name()) != LITERAL("p:sp") &&
						string_type(child->name()) != LITERAL("p:cxnSp") &&
						string_type(child->name()) != LITERAL("p:pic")) {
						continue;
					}
					
					this->xread_shape(child, dm, ref);
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_shape_group
			/* ------------------------------------------------------------- */
			template <class XMLNode, class DocManager, class Container>
			basic_layout& xread_shape_group(XMLNode* root, DocManager& dm, Container& ref) {
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
				
				this->xread_shape_tree(root, dm, ref);
				
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
			//  xread_shape
			/* ------------------------------------------------------------- */
			template <class XMLNode, class DocManager, class Container>
			basic_layout& xread_shape(XMLNode* root, DocManager& dm, Container& ref) {
				if (!root) throw std::runtime_error("cannot find <p:sp>");
				
				value_type elem(master_.clrmap());
				if (dm.theme()) elem.theme(dm.theme().get());
				elem.read(root, LITERAL("p"));
				if (!elem.reference().empty()) {
					if (ref.find(elem.reference()) == ref.end()) {
						throw std::runtime_error("cannot find reference");
					}
					std::basic_stringstream<CharT, Traits> ss;
					ss << LITERAL("ppt/media/") << ref[elem.reference()];
					elem.reference(ss.str());
				}
				v_.push_back(elem);
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_shstyle
			/* ------------------------------------------------------------- */
			template <class XMLNode, class DocManager>
			basic_layout& xread_shstyle(XMLNode* root, size_type idx, DocManager& dm) {
				if (!root) throw std::runtime_error("cannot find <p:sp>");
				
				shstyle_type elem;
				typename shstyle_map::const_iterator it = master_.shapes().find(idx);
				if (it != master_.shapes().end()) {
					elem = it->second;
					elem.texts().clear();
				}
				
				if (dm.theme()) elem.theme(dm.theme().get());
				elem.read(root, LITERAL("p"));
				sh_[idx] = elem;
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_txstyle
			/* ------------------------------------------------------------- */
			template <class XMLNode, class DocManager>
			basic_layout& xread_txstyle(XMLNode* root, size_type idx, DocManager& dm) {
				if (!root) return *this;
				node_ptr pos = root->first_node(LITERAL("a:lstStyle"));
				if (!pos || !pos->first_node()) return *this;
				
				const txstyle_container* base = NULL;
				if (idx == 0) base = &master_.title_style();
				else if (idx == 1) base = &master_.body_style();
				else base = &master_.other_style();
				
				txstyle_container v;
				size_type i = 0;
				for (node_ptr child = pos->first_node(); child; child = child->next_sibling()) {
					txstyle_type elem;
					if (i < base->size()) elem = base->at(i);
					if (dm.theme()) elem.theme(dm.theme().get());
					elem.read(child);
					v.push_back(elem);
					++i;
				}
				tx_[idx] = v;
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_master
			/* ------------------------------------------------------------- */
			template <class DocManager>
			basic_layout& xread_master(DocManager& dm) {
				std::basic_stringstream<CharT, Traits> ss;
				ss << LITERAL("ppt/slideLayouts/_rels")
					<< path_.substr(path_.find_last_of(LITERAL("/")))
					<< LITERAL(".rels");
				typename DocManager::storage_type::iterator pos = dm.storage().find(ss.str());
				if (pos == dm.storage().end()) throw std::runtime_error("cannot find layout rels");
				
				std::map<string_type, string_type> ref;
				faml::officex::read_reference(*pos, ref);
				
				string_type m;
				typedef typename std::map<string_type, string_type>::iterator iter;
				for (iter it = ref.begin(); it != ref.end(); ++it) {
					if (it->second.compare(0, 11, LITERAL("slideMaster")) == 0) {
						m = it->second;
						break;
					}
				}
				if (m.empty()) throw std::runtime_error("cannof find master reference");
				
				ss.str(LITERAL(""));
				ss << LITERAL("ppt/slideMasters/") << m;
				
				pos = dm.storage().find(ss.str());
				if (pos == dm.storage().end()) throw std::runtime_error("cannot find master");
				
				master_.read(*pos, dm);
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xassign_master
			/* ------------------------------------------------------------- */
			template <class XMLNode, class DocManager>
			basic_layout& xassign_master(XMLNode* root, DocManager& dm) {
				size_type assign = 1;
				attr_ptr attr = root->first_attribute(LITERAL("showMasterSp"));
				if (attr && attr->value_size() > 0) {
					assign = clx::lexical_cast<size_type>(attr->value());
				}
				if (assign == 0) return *this;
				
				if (!master_.empty()) {
					v_.assign(master_.begin(), master_.end());
				}
				
				return *this;
			}
		};
	}
}

#endif // FAML_PPTX_LAYOUT_H
