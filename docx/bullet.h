/* ------------------------------------------------------------------------- */
/*
 *  docx/bullet.h
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
#ifndef FAML_DOCX_BULLET_H
#define FAML_DOCX_BULLET_H

#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include "rapidxml/rapidxml.hpp"
#include "clx/lexical_cast.h"
#include "clx/literal.h"
#include "clx/utility.h"
#include "../officex/bullet.h"

namespace faml {
	namespace docx {
		/* ----------------------------------------------------------------- */
		//  getbullet
		/* ----------------------------------------------------------------- */
		inline std::string getbullet(const std::string& name, const std::string& fmt) {
			std::string dest;
			if (name == "decimal") dest = "1";
			if (name == "decimalFullWidth") dest = "1";
			else if (name == "decimalEnclosedCircle") dest = "①";
			else if (name == "lowerRoman") dest = "ⅰ";
			else if (name == "upperRoman") dest = "Ⅰ";
			else if (name == "aiueoFullWidth") dest = "ア";
			else if (name == "upperLetter") dest = "A";
			else if (name == "lowerLetter") dest = "a";
			else if (name == "bullet") {
				if (fmt.size() < 3) return fmt;
				
				dest = "・";
				size_t c0 = static_cast<size_t>(fmt.at(0)) & 0xff;
				size_t c1 = static_cast<size_t>(fmt.at(1)) & 0xff;
				size_t c2 = static_cast<size_t>(fmt.at(2)) & 0xff;
				if (c0 != 0xef) return fmt;
				
				if (c1 == 0x81 && c2 == 0xac) dest = "●";
				else if (c1 == 0x82 && c2 == 0xa1) dest = "○";
				else if (c1 == 0x82 && c2 == 0xa7) dest = "・";
				else if (c1 == 0x82 && c2 == 0x92) dest = "・";
				else if (c1 == 0x82 && c2 == 0x9f) dest = "・";
				else if (c1 == 0x81 && c2 == 0xae) dest = "■";
				else if (c1 == 0x81 && c2 == 0xaf) dest = "□";
				else if (c1 == 0x81 && c2 == 0xb5) dest = "◆";
				//else if (c1 == 0x82 && c2 == 0x9f) dest = "◇";
				else if (c1 == 0x83 && c2 == 0xbc) dest = "－"; // "✓"
				else if (c1 == 0x83 && c2 == 0x98) dest = "≫"; // "➢"
				else if (c1 == 0xbc && c2 == 0x8a) dest = "＊"; // "＊"
			}
			
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  basic_bullet
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_bullet {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			
			struct value_type {
				size_type type;
				string_type mark;
				string_type format;
				double indent;
				double margin;
			};
			
			typedef std::vector<value_type> subcontainer;
			typedef std::map<size_type, subcontainer> container;
			typedef typename container::const_iterator const_iterator;
			
			enum { none = 0, ul = 1, ol = 2 };
			
			basic_bullet() : v_() {}
			
			template <class Ch, class Tr>
			basic_bullet(std::basic_istream<Ch, Tr>& in) :
				v_() {
				this->read(in);
			}
			
			virtual ~basic_bullet() throw() {}
			
			template <class Ch, class Tr>
			basic_bullet& read(std::basic_istream<Ch, Tr>& in) {
				std::vector<char_type> s;
				clx::read(in, s);
				s.push_back(0); // make null terminated string.
				
				//rapidxml::xml_document<char_type> doc; // <- ??? compile error!!
				rapidxml::xml_document<char> doc;
				doc.parse<0>(reinterpret_cast<char_type*>(&s.at(0)));
				node_ptr root = doc.first_node(LITERAL("w:numbering"));
				if (!root) throw std::runtime_error("cannot find <w:numbering> (root) tag");
				
				container tmp;
				for (node_ptr child = root->first_node(); child; child = child->next_sibling()) {
					if (string_type(child->name()) != LITERAL("w:abstractNum")) continue;
					subcontainer elem;
					this->xread_data(child, elem);
					attr_ptr attr = child->first_attribute(LITERAL("w:abstractNumId"));
					if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find w:abstractNumId");
					tmp[clx::lexical_cast<size_type>(attr->value())] = elem;
				}
				
				for (node_ptr child = root->first_node(LITERAL("w:num")); child; child = child->next_sibling()) {
					if (string_type(child->name()) != LITERAL("w:num")) continue;
					attr_ptr attr = child->first_attribute(LITERAL("w:numId"));
					if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find w:numId");
					size_type index = clx::lexical_cast<size_type>(attr->value());
					
					node_ptr pos = child->first_node(LITERAL("w:abstractNumId"));
					if (!pos) throw std::runtime_error("cannot find <w:abstractNumId> tag");
					attr = pos->first_attribute(LITERAL("w:val"));
					if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find w:val");
					size_type ref = clx::lexical_cast<size_type>(attr->value());
					v_[index] = tmp[ref];
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			bool empty() const { return v_.empty(); }
			size_type size() const { return v_.size(); }
			subcontainer& operator[](size_type pos) { return v_[pos]; }
			const_iterator find(size_type pos) const { return v_.find(pos); }
			const_iterator begin() const { return v_.begin(); }
			const_iterator end() const { return v_.end(); }
			
		private:
			typedef rapidxml::xml_node<CharT>* node_ptr;
			typedef rapidxml::xml_attribute<CharT>* attr_ptr;
			
			container v_;
			
			template <class XMLNode, class Container>
			basic_bullet& xread_data(XMLNode* root, Container& dest) {
				if (!root) throw std::runtime_error("cannot find <w:abstractNum> tag");
				
				for (node_ptr child = root->first_node(); child; child = child->next_sibling()) {
					if (string_type(child->name()) != LITERAL("w:lvl")) continue;
					
					node_ptr pos = child->first_node(LITERAL("w:numFmt"));
					if (!pos) throw std::runtime_error("cannot find <w:numFmt> tag");
					attr_ptr attr = pos->first_attribute(LITERAL("w:val"));
					if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find w:val attribute");
					string_type name(attr->value());
					
					pos = child->first_node(LITERAL("w:lvlText"));
					if (!pos) throw std::runtime_error("cannot find <w:lvlText> tag");
					attr = pos->first_attribute(LITERAL("w:val"));
					if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find w:val attribute");
					string_type fmt(attr->value());
					
					string_type mk = getbullet(name, fmt);
					value_type elem;
					elem.mark = mk;
					if (name == LITERAL("bullet")) elem.type = ul;
					else {
						elem.type = ol;
						size_type i = fmt.find(LITERAL("%"));
						if (i != string_type::npos && i + 1 < fmt.size()) fmt.at(i + 1) = LITERAL('s');
						elem.format = fmt;
					}
					
					elem.indent = 0.0;
					pos = child->first_node(LITERAL("w:pPr"));
					if (pos) {
						node_ptr tmp = pos->first_node(LITERAL("w:ind"));
						if (tmp) {
							attr = tmp->first_attribute(LITERAL("w:left"));
							if (attr && attr->value_size() > 0) {
								elem.indent = clx::lexical_cast<double>(attr->value());
							}
							attr = tmp->first_attribute(LITERAL("w:hanging"));
							if (attr && attr->value_size() > 0) {
								elem.margin = clx::lexical_cast<double>(attr->value());
							}
						}
					}
					
					dest.push_back(elem);
				}
				
				return *this;
			}
		};
	}
}

#endif // FAML_DOCX_BULLET_H
