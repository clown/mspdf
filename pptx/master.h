/* ------------------------------------------------------------------------- */
/*
 *  pptx/master.h
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
 *  Last-modified: Sat 13 Jun 2009 02:51:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PPTX_MASTER_H
#define FAML_PPTX_MASTER_H

#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include "rapidxml/rapidxml.hpp"
#include "clx/literal.h"
#include "clx/utility.h"
#include "../officex/unit.h"
#include "../officex/color.h"
#include "../officex/shape.h"
#include "../officex/txstyle.h"

namespace faml {
	namespace pptx {
		/* ----------------------------------------------------------------- */
		//  basic_master
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_master {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef faml::officex::basic_shape<CharT, Traits> shape_type;
			typedef shape_type value_type;
			typedef std::vector<value_type> container;
			typedef typename container::const_iterator const_iterator;
			typedef typename container::iterator iterator;
			typedef faml::officex::basic_txstyle<CharT, Traits> txstyle;
			typedef faml::officex::basic_theme<CharT, Traits> theme_type;
			typedef std::map<size_type, shape_type> shape_map;
			typedef std::vector<txstyle> txstyle_container;
			typedef std::map<string_type, string_type> color_map;
			
			basic_master() :
				path_(), background_(), angle_(-1.0), v_(),
				sh_(), title_(), body_(), other_() {}
			
			template <class InStream, class DocManager>
			basic_master(InStream& in, DocManager& dm) :
				path_(), background_(), angle_(-1.0), v_(),
				sh_(), title_(), body_(), other_() {
				this->read(in, dm);
			}
			
			virtual ~basic_master() throw() {}
			
			template <class InStream, class DocManager>
			basic_master& read(InStream& in, DocManager& dm) {
				path_ = in.path();
				std::vector<char_type> s;
				clx::read(in, s);
				s.push_back(0);
				
				//rapidxml::xml_document<char_type> doc; // <- ??? compile error!!
				rapidxml::xml_document<char> doc;
				doc.parse<0>(reinterpret_cast<char_type*>(&s.at(0)));
				
				node_ptr root = doc.first_node(LITERAL("p:sldMaster"));
				if (!root) throw std::runtime_error("cannot find <p:sldMaster> (root) tag");
				
				// 1. color map.
				node_ptr pos = root->first_node(LITERAL("p:clrMap"));
				if (pos) this->xread_clrmap(pos, dm);
				
				// 2. background color.
				pos = root->first_node(LITERAL("p:cSld"));
				if (!pos) throw std::runtime_error("cannot find <p:cSld> tag");
				this->xread_background(pos->first_node(LITERAL("p:bg")), dm);
				
				// 3. master shapes.
				this->xread_shapes(pos->first_node(LITERAL("p:spTree")), dm);
				
				// 4. text styles.
				this->xread_default_styles(other_, dm);
				pos = root->first_node(LITERAL("p:txStyles"));
				if (!pos) throw std::runtime_error("cannot find <p:txStyles> tag");
				this->xread_styles(pos->first_node(LITERAL("p:titleStyle")), title_, dm);
				this->xread_styles(pos->first_node(LITERAL("p:bodyStyle")), body_, dm);
				node_ptr child = pos->first_node(LITERAL("p:otherStyle"));
				if (child) this->xread_styles(child, other_, dm);
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			const std::vector<size_type> rgbs() const { return background_; }
			const string_type& bgimage() const { return bgimage_; }
			size_type rgb(size_type pos = 0) const { return background_.at(pos); }
			double angle() const { return angle_; }
			bool empty() const { return v_.empty(); }
			size_type size() const { return v_.size(); }
			const value_type& at(size_type pos) const { return v_.at(pos); }
			const value_type& operator[](size_type pos) const { return v_[pos]; }
			const_iterator begin() const { return v_.begin(); }
			const_iterator end() const { return v_.end(); }
			iterator begin() { return v_.begin(); }
			iterator end() { return v_.end(); }
			
			const shape_map& shapes() const { return sh_; }
			shape_map& shapes() { return sh_; }
			const txstyle_container& title_style() const { return title_; }
			const txstyle_container& body_style() const { return body_; }
			const txstyle_container& other_style() const { return other_; }
			const color_map& clrmap() const { return clr_; }
			color_map& clrmap() { return clr_; }
			
		private:
			typedef rapidxml::xml_node<CharT>* node_ptr;
			typedef rapidxml::xml_attribute<CharT>* attr_ptr;
			
			string_type path_;
			std::vector<size_type> background_;
			string_type bgimage_;
			double angle_;
			container v_;
			shape_map sh_;
			txstyle_container title_;
			txstyle_container body_;
			txstyle_container other_;
			color_map clr_;
			
			/* ------------------------------------------------------------- */
			//  xread_default_styles
			/* ------------------------------------------------------------- */
			template <class Container, class DocManager>
			basic_master& xread_default_styles(Container& dest, DocManager& dm) {
				typename DocManager::storage_type::iterator pos = dm.storage().find(LITERAL("ppt/presentation.xml"));
				if (pos == dm.storage().end()) return *this;
				
				std::vector<char_type> s;
				clx::read(*pos, s);
				s.push_back(0);
				
				//rapidxml::xml_document<char_type> doc; // <- ??? compile error!!
				rapidxml::xml_document<char> doc;
				doc.parse<0>(reinterpret_cast<char_type*>(&s.at(0)));
				
				node_ptr root = doc.first_node(LITERAL("p:presentation"));
				if (!root) return *this;
				
				node_ptr p = root->first_node(LITERAL("p:defaultTextStyle"));
				if (p) return this->xread_styles(p, dest, dm);
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_styles
			/* ------------------------------------------------------------- */
			template <class Container, class DocManager>
			basic_master& xread_styles(node_ptr root, Container& dest, DocManager& dm) {
				if (!root) std::runtime_error("cannot find <p:xxxStyle> tag");
				for (node_ptr child = root->first_node(); child; child = child->next_sibling()) {
					if (string_type(child->name()) == LITERAL("a:defPPr")) continue;
					txstyle elem;
					if (dm.theme()) elem.theme(dm.theme().get());
					elem.read(child);
					dest.push_back(elem);
				}
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_background
			/* ------------------------------------------------------------- */
			template <class DocManager>
			basic_master& xread_background(node_ptr root, DocManager& dm) {
				//if (!root) throw std::runtime_error("cannot find <p:bg> tag");
				if (!root) {
					clx::logger::debug(DEBUGF("warning: background info is not found"));
					background_.push_back(0xffffff);
					return *this;
				}
				
				node_ptr pos = root->first_node(LITERAL("p:bgRef"));
				if (!pos) {
					node_ptr parent = root->first_node(LITERAL("p:bgPr"));
					if (!parent) return *this;
					pos = parent->first_node(LITERAL("a:blipFill"));
					if (pos) this->xread_bgimage(pos, dm);
					pos = parent->first_node(LITERAL("a:gradFill"));
					if (pos) return this->xread_grad(pos, dm);
					pos = parent->first_node(LITERAL("a:solidFill"));
					if (!pos) return *this;
					//if (!pos) throw std::runtime_error("cannot find <a:solidFill> tag");
				}
				
				if (dm.theme()) background_.push_back(faml::officex::getrgb(pos, dm.theme()->palette(), clr_));
				else background_.push_back(faml::officex::getrgb(pos));
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_bgimage
			/* ------------------------------------------------------------- */
			template <class DocManager>
			basic_master& xread_bgimage(node_ptr root, DocManager& dm) {
				if (!root) throw std::runtime_error("cannot find <a:blipFill>");
				
				node_ptr pos = root->first_node(LITERAL("a:blip"));
				if (!pos) return *this;
				attr_ptr attr = pos->first_attribute(LITERAL("r:embed"));
				if (!attr || attr->value_size() == 0) return *this;
				string_type s(attr->value());
				
				std::map<string_type, string_type> ref;
				std::basic_stringstream<CharT, Traits> ss;
				ss << LITERAL("ppt/slideMasters/_rels");
				ss << path_.substr(path_.find_last_of(LITERAL("/")));
				ss << LITERAL(".rels");
				typename DocManager::storage_type::iterator fp = dm.storage().find(ss.str());
				if (fp != dm.storage().end()) {
					faml::officex::read_reference(*fp, ref);
				}
				
				if (ref.find(s) != ref.end()) bgimage_ = LITERAL("ppt/media/") + ref[s];
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_clrmap
			/* ------------------------------------------------------------- */
			template <class DocManager>
			basic_master& xread_clrmap(node_ptr root, DocManager& dm) {
				if (!root) throw std::runtime_error("cannot find <p:clrMap>");
				for (attr_ptr attr = root->first_attribute(); attr; attr = attr->next_attribute()) {
					if (string_type(attr->name()) == LITERAL("xmlns:p")) continue;
					if (!attr || attr->value_size() == 0) continue;
					string_type key(attr->name());
					string_type value(attr->value());
					clr_[key] = value;
				}
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_grad
			/* ------------------------------------------------------------- */
			template <class DocManager>
			basic_master& xread_grad(node_ptr root, DocManager& dm) {
				if (!root) throw std::runtime_error("cannot find <a:gradFill>");
				node_ptr pos = root->first_node(LITERAL("a:gsLst"));
				if (!pos) throw std::runtime_error("cannot find <a:gsLst>");
				for (node_ptr child = pos->first_node(); child; child = child->next_sibling()) {
					if (string_type(child->name()) != LITERAL("a:gs")) continue;
					size_type tmp = 0xff000000;
					if (dm.theme()) tmp = faml::officex::getrgb(child, dm.theme()->palette(), clr_);
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
			//  xread_shapes
			/* ------------------------------------------------------------- */
			template <class DocManager>
			basic_master& xread_shapes(node_ptr root, DocManager& dm) {
				if (!root) throw std::runtime_error("cannot find <p:spTree> tag");
				
				std::map<string_type, string_type> ref;
				std::basic_stringstream<CharT, Traits> ss;
				ss << LITERAL("ppt/slideMasters/_rels");
				ss << path_.substr(path_.find_last_of(LITERAL("/")));
				ss << LITERAL(".rels");
				typename DocManager::storage_type::iterator fp = dm.storage().find(ss.str());
				if (fp != dm.storage().end()) {
					faml::officex::read_reference(*fp, ref);
				}
				
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
					
					// get shape placeholder index.
					node_ptr p1 = child->first_node(LITERAL("p:nvSpPr"));
					if (!p1) p1 = child->first_node(LITERAL("p:nvPicPr"));
					if (!p1) p1 = child->first_node(LITERAL("p:cNvCxnSpPr"));
					if (!p1) throw std::runtime_error("cannot find <p:nvSpPr> tag");
					node_ptr p2 = p1->first_node(LITERAL("p:nvPr"));
					if (!p2) throw std::runtime_error("cannot find <p:nvPr> tag");
					node_ptr pos = p2->first_node(LITERAL("p:ph"));
					if (!pos) this->xread_master_shape(child, dm, ref);
					else {
						size_type idx = 0;
						attr_ptr attr = pos->first_attribute(LITERAL("idx"));
						if (attr && attr->value_size() > 0) idx = clx::lexical_cast<size_type>(attr->value());
						
						shape_type elem(clr_);
						//elem.background(this->rgb());
						//elem.border(this->rgb());
						if (dm.theme()) elem.theme(dm.theme().get());
						elem.read(child, LITERAL("p"));
						
						sh_[idx] = elem;
					}
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_shape_tree
			/* ------------------------------------------------------------- */
			template <class XMLNode, class DocManager, class Container>
			basic_master& xread_shape_tree(XMLNode* root, DocManager& dm, Container& ref) {
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
					
					this->xread_master_shape(child, dm, ref);
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_shape_group
			/* ------------------------------------------------------------- */
			template <class XMLNode, class DocManager, class Container>
			basic_master& xread_shape_group(XMLNode* root, DocManager& dm, Container& ref) {
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
			//  xread_master_shape
			/* ------------------------------------------------------------- */
			template <class DocManager, class Container>
			basic_master& xread_master_shape(node_ptr root, DocManager& dm, Container& ref) {
				value_type elem(clr_);
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
		};
	}
}

#endif // FAML_PPTX_MASTER_H
