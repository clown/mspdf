/* ------------------------------------------------------------------------- */
/*
 *  xslx/drawing.h
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
#ifndef FAML_XLSX_DRAWING_H
#define FAML_XLSX_DRAWING_H

#include <stdexcept>
#include <string>
#include <vector>
#include "rapidxml/rapidxml.hpp"
#include "clx/lexical_cast.h"
#include "clx/literal.h"
#include "clx/utility.h"
#include "../coordinate.h"
#include "../officex/shape.h"

namespace faml {
	namespace xlsx {
		using faml::pdf::coordinate;
		
		/* ----------------------------------------------------------------- */
		//  basic_shape_container
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_shape_container {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef faml::officex::basic_shape<CharT, Traits> value_type;
			typedef std::vector<value_type> container;
			typedef typename container::const_iterator const_iterator;
			typedef faml::officex::basic_theme<CharT, Traits> theme_type;
			typedef std::map<size_type, double> size_map;
			
			basic_shape_container() :
				v_(), path_(), widths_(NULL), heights_(NULL) {}
			
			template <class InStream, class DocManager>
			basic_shape_container(InStream& in, DocManager& dm) :
				v_(), path_(), widths_(NULL), heights_(NULL) {
				this->read(in, dm);
			}
			
			template <class InStream, class DocManager>
			basic_shape_container& read(InStream& in, DocManager& dm) {
				path_ = in.path();
				
				std::vector<char_type> s;
				clx::read(in, s);
				s.push_back(0); // make null terminated string.
				
				//rapidxml::xml_document<char_type> doc; // <- ??? compile error!!
				rapidxml::xml_document<char> doc;
				doc.parse<0>(reinterpret_cast<char_type*>(&s.at(0)));
				
				std::map<string_type, string_type> rels;
				this->xread_rels(rels, dm);
				
				node_ptr root = doc.first_node(LITERAL("xdr:wsDr"));
				if (!root) throw std::runtime_error("cannot find <xdr:wsDr> (root) tag");
				
				for (node_ptr child = root->first_node(); child; child = child->next_sibling()) {
					if (string_type(child->name()) != LITERAL("xdr:twoCellAnchor")) continue;
					value_type elem;
					this->xread_form(child, elem);
					
					// shape
					node_ptr pos = child->first_node(LITERAL("xdr:sp"));
					if (!pos) pos = child->first_node(LITERAL("xdr:pic"));
					if (pos) {
						if (dm.theme()) elem.theme(dm.theme().get());
						elem.read(pos, LITERAL("xdr"));
						this->xread_form(child, elem); // temporary code.
						
						if (!elem.reference().empty()) {
							typename std::map<string_type, string_type>::iterator it = rels.find(elem.reference());
							if (it != rels.end()) {
								std::basic_stringstream<CharT, Traits> ss;
								ss << LITERAL("xl/media/") << it->second;
								elem.reference(ss.str());
							}
						}
						v_.push_back(elem);
						continue;
					}
					
					// graphic data (chart, and more ... ?)
					pos = child->first_node(LITERAL("xdr:graphicFrame"));
					if (pos) {
						this->xread_reference(pos, elem);
						if (!elem.reference().empty()) {
							typename std::map<string_type, string_type>::iterator it = rels.find(elem.reference());
							if (it != rels.end()) elem.reference(it->second);
						}
						v_.push_back(elem);
						continue;
					}
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
			value_type& at(size_type pos) { return v_.at(pos); }
			value_type& operator[](size_type pos) { return v_[pos]; }
			const_iterator begin() const { return v_.begin(); }
			const_iterator end() const { return v_.end(); }
			
			/* ------------------------------------------------------------- */
			//  Access methods (set).
			/* ------------------------------------------------------------- */
			void widths(const size_map& cp) { widths_ = &cp; }
			void heights(const size_map& cp) { heights_ = &cp; }
			
		private:
			typedef rapidxml::xml_node<CharT>* node_ptr;
			typedef rapidxml::xml_attribute<CharT>* attr_ptr;
			
			container v_;
			string_type path_;
			const size_map* widths_;
			const size_map* heights_;
			
			/* ------------------------------------------------------------- */
			//  xread_form
			/* ------------------------------------------------------------- */
			template <class XMLNode, class Type>
			basic_shape_container& xread_form(XMLNode* root, Type& dest) {
				if (!root) throw std::runtime_error("cannot find <xdr:twoCellAnchor> tag");
				
				double x0, y0;
				this->xread_coordinate(root->first_node(LITERAL("xdr:from")), x0, y0);
				
				double x1, y1;
				this->xread_coordinate(root->first_node(LITERAL("xdr:to")), x1, y1);
				
				dest.origin(coordinate(x0, y0));
				dest.width(x1 - x0);
				dest.height(y1 - y0);
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_coordinate
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_shape_container& xread_coordinate(XMLNode* root, double& x, double& y) {
				if (!root) throw std::runtime_error("cannot find <xdr:from/to> tag");
				
				// x-axis
				node_ptr pos = root->first_node(LITERAL("xdr:col"));
				if (!pos || pos->value_size() == 0) throw std::runtime_error("cannot find <xdr:col> tag");
				size_type idx = clx::lexical_cast<size_type>(pos->value());
				
				x = 0.0;
				for (size_type i = 0; i < idx; ++i) {
					if (widths_ && widths_->find(i + 1) != widths_->end()) x += (widths_->find(i + 1))->second;
					else x += 8.0;
				}
				
				x *= 6.0 * 12700; // convert xls unit to emu.
				//x *= (11.0 * 0.5 * 12700); // convert xls unit to emu.
				pos = root->first_node(LITERAL("xdr:colOff"));
				if (!pos || pos->value_size() == 0) throw std::runtime_error("cannot find <xdr:colOff> tag");
				x += clx::lexical_cast<double>(pos->value());
				
				// y-axis
				pos = root->first_node(LITERAL("xdr:row"));
				if (!pos || pos->value_size() == 0) throw std::runtime_error("cannot find <xdr:row> tag");
				idx = clx::lexical_cast<size_type>(pos->value());
				
				y = 0.0;
				for (size_type i = 0; i < idx; ++i) {
					if (heights_ && heights_->find(i + 1) != heights_->end()) y += (heights_->find(i + 1))->second;
					else y += 13.5;
				}
				y *= 12700; // convert pt to emu.
				pos = root->first_node(LITERAL("xdr:rowOff"));
				if (!pos || pos->value_size() == 0) throw std::runtime_error("cannot find <xdr:rowOff> tag");
				y += clx::lexical_cast<double>(pos->value());
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_reference
			/* ------------------------------------------------------------- */
			template <class XMLNode, class Type>
			basic_shape_container& xread_reference(XMLNode* root, Type& dest) {
				if (!root) throw std::runtime_error("cannot find <xdr:graphicFrame> tag");
				
				node_ptr p1 = root->first_node(LITERAL("a:graphic"));
				if (!p1) throw std::runtime_error("cannot find <a:graphic> tag");
				node_ptr p2 = p1->first_node(LITERAL("a:graphicData"));
				if (!p2) throw std::runtime_error("cannot find <a:graphicData> tag");
				node_ptr pos = p2->first_node(LITERAL("c:chart"));
				if (!pos) throw std::runtime_error("cannot find <c:chart> tag");
				
				attr_ptr attr = pos->first_attribute(LITERAL("r:id"));
				if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find r:id attribute");
				
				dest.reference(string_type(attr->value()));
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_rels
			/* ------------------------------------------------------------- */
			template <class Container, class DocManager>
			basic_shape_container& xread_rels(Container& dest, DocManager& dm) {
				std::basic_stringstream<CharT, Traits> ss;
				ss << LITERAL("xl/drawings/_rels");
				ss << path_.substr(path_.find_last_of(LITERAL("/")));
				ss << LITERAL(".rels");
				typename DocManager::storage_type::iterator fp = dm.storage().find(ss.str());
				if (fp != dm.storage().end()) faml::officex::read_reference(*fp, dest);
				
				return *this;
			}
		};
	}
}

#endif // FAML_XLSX_DRAWING_H
