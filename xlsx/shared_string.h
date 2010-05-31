/* ------------------------------------------------------------------------- */
/*
 *  xslx/shared_string.h
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
#ifndef FAML_XLSX_SHARED_STRING_H
#define FAML_XLSX_SHARED_STRING_H

#include <cassert>
#include <stdexcept>
#include <string>
#include <vector>
#include "rapidxml/rapidxml.hpp"
#include "clx/lexical_cast.h"
#include "clx/literal.h"
#include "clx/utility.h"
#include "../piece.h"
#include "../color.h"

namespace faml {
	namespace xlsx {
		/* ----------------------------------------------------------------- */
		//  basic_shared_string
		/* ----------------------------------------------------------------- */
		template <
			class CharT = char,
			class Traits = std::char_traits<CharT>
		>
		class basic_shared_string {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef faml::basic_piece<CharT, Traits> value_type;
			typedef std::vector<value_type> paragraph_type;
			typedef std::vector<paragraph_type> container;
			
			basic_shared_string() : v_() {}
			
			template <class InStream>
			basic_shared_string(InStream& in) :
				v_() { this->read(in); }
			
			virtual ~basic_shared_string() throw() {}
			
			/* ------------------------------------------------------------- */
			/*
			 *  read
			 *
			 *  Main operation of the class. Opens the xl/shredStrings.xml
			 *  and read (shared) strings.
			 */
			/* ------------------------------------------------------------- */
			template <class InStream>
			basic_shared_string& read(InStream& in) {
				std::vector<char_type> s;
				clx::read(in, s);
				s.push_back(0); // make null terminated string.
				
				//rapidxml::xml_document<char_type> doc; // <- ??? compile error!!
				rapidxml::xml_document<char> doc;
				doc.parse<0>(reinterpret_cast<char_type*>(&s.at(0)));
				
				// parser OpenXML.
				typedef rapidxml::xml_node<char_type>* node_ptr;
				typedef rapidxml::xml_attribute<char_type>* attr_ptr;
				
				node_ptr root = doc.first_node(LITERAL("sst"));
				if (!root) throw std::runtime_error("cannot find <sst> tag");
				
				for (node_ptr child = root->first_node(); child; child = child->next_sibling()) {
					node_ptr p = child->first_node(LITERAL("t"));
					paragraph_type elem;
					if (p && p->value_size() > 0) {
						value_type s;
						s.data(string_type(p->value()));
						elem.push_back(s);
					}
					else this->xread_paragraph(child, elem);
					v_.push_back(elem);
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			bool empty() const { return v_.empty(); }
			size_type size() const { return v_.size(); }
			const paragraph_type& at(size_type pos) const { return v_.at(pos); }
			const paragraph_type& operator[](size_type pos) const { return v_[pos]; }
			
		private:
			typedef rapidxml::xml_node<CharT>* node_ptr;
			typedef rapidxml::xml_attribute<CharT>* attr_ptr;
			typedef faml::pdf::color color_type;
			
			container v_;
			
			/* ------------------------------------------------------------- */
			//  xread_paragraph
			/* ------------------------------------------------------------- */
			template <class XMLNode, class SST>
			basic_shared_string& xread_paragraph(XMLNode* root, SST& dest) {
				if (!root) throw std::runtime_error("cannot find <si>");
				
				for (node_ptr child = root->first_node(); child; child = child->next_sibling()) {
					if (string_type(child->name()) != LITERAL("r")) continue;
					value_type elem;
					
					// 1. style
					node_ptr pos = child->first_node(LITERAL("rPr"));
					if (pos) {
						node_ptr tmp = pos->first_node(LITERAL("rFont"));
						if (tmp) {
							attr_ptr attr = tmp->first_attribute(LITERAL("val"));
							if (attr && attr->value_size() > 0) elem.name(attr->value());
						}
						
						size_type deco = 0;
						tmp = pos->first_node(LITERAL("sz"));
						if (tmp) {
							attr_ptr attr = tmp->first_attribute(LITERAL("val"));
							if (attr && attr->value_size() > 0) elem.size(clx::lexical_cast<double>(attr->value()));
						}
						else elem.size(-1);
						
						tmp = pos->first_node(LITERAL("color"));
						if (tmp) {
							attr_ptr attr = tmp->first_attribute(LITERAL("rgb"));
							if (attr && attr->value_size() > 0) {
								size_type rgb = clx::lexical_cast<size_type>(attr->value(), std::ios::hex);
								elem.fill(color_type(rgb & 0x00ffffff));
							}
						}
						
						tmp = pos->first_node(LITERAL("b"));
						if (tmp) deco |= 0x01;
						tmp = pos->first_node(LITERAL("i"));
						if (tmp) deco |= 0x02;
						tmp = pos->first_node(LITERAL("u"));
						if (tmp) deco |= 0x04;
						tmp = pos->first_node(LITERAL("strike"));
						if (tmp) deco |= 0x08;
						elem.decorate(deco);
					}
					
					// 2. data
					pos = child->first_node(LITERAL("t"));
					if (pos && pos->value_size() > 0) elem.data(string_type(pos->value()));
					dest.push_back(elem);
				}
				
				return *this;
			}
		};
	}
}

#endif // FAML_XLSX_SHARED_STRING_H
