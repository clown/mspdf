/* ------------------------------------------------------------------------- */
/*
 *  docx/style.h
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
 *  Last-modified: Tue 16 Jun 2009 14:50:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_DOCX_STYLE_H
#define FAML_DOCX_STYLE_H

#include <stdexcept>
#include <string>
#include <map>
#include "rapidxml/rapidxml.hpp"
#include "clx/literal.h"
#include "clx/lexical_cast.h"
#include "clx/utility.h"
#include "pstyle.h"

namespace faml {
	namespace docx {
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_style {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef std::map<string_type, pstyle> pstyle_map;
			
			basic_style() :
				latin_(LITERAL("Century")), japan_(LITERAL("ＭＳ 明朝")),
				size_(10.5), p_() {}
			
			template <class Ch, class Tr>
			basic_style(std::basic_istream<Ch, Tr>& in) :
				latin_(LITERAL("Century")), japan_(LITERAL("ＭＳ 明朝")),
				size_(10.5), p_() {
				this->read(in);
			}
			
			virtual ~basic_style() throw() {}
			
			template <class Ch, class Tr>
			basic_style& read(std::basic_istream<Ch, Tr>& in) {
				std::vector<char_type> s;
				clx::read(in, s);
				s.push_back(0); // make null terminated string.
				
				//rapidxml::xml_document<char_type> doc; // <- ??? compile error!!
				rapidxml::xml_document<char> doc;
				doc.parse<0>(reinterpret_cast<char_type*>(&s.at(0)));
				node_ptr root = doc.first_node(LITERAL("w:styles"));
				if (!root) throw std::runtime_error("cannot find <w:styles> (root) tag");
				
				this->xread_defaults(root->first_node(LITERAL("w:docDefaults")));
				this->xread_styles(root);
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			double size() const { return size_; }
			const string_type& latin() const { return latin_; }
			const string_type& japan() const { return japan_; }
			pstyle_map& paragraph() { return p_; }
			pstyle& paragraph(const string_type& name) { return p_[name]; }
			
		private:
			typedef rapidxml::xml_node<CharT>* node_ptr;
			typedef rapidxml::xml_attribute<CharT>* attr_ptr;
			
			string_type latin_;
			string_type japan_;
			double size_;
			pstyle_map p_;
			
			/* ------------------------------------------------------------- */
			//  xread_defaults
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_style& xread_defaults(XMLNode* root) {
				if (!root) throw std::runtime_error("cannot find <w:docDefaults> tag");
				
				node_ptr p1 = root->first_node(LITERAL("w:rPrDefault"));
				if (!p1) throw std::runtime_error("cannot find <w:rPrDefault> tag");
				node_ptr p2 = p1->first_node(LITERAL("w:rPr"));
				if (!p2) throw std::runtime_error("cannot find <w:rPr> tag");
				
				node_ptr pos = p2->first_node(LITERAL("w:rFonts"));
				if (pos) {
					attr_ptr attr = pos->first_attribute(LITERAL("w:ascii"));
					if (attr && attr->value_size() > 0) latin_ = string_type(attr->value());
					attr = pos->first_attribute(LITERAL("w:eastAsia"));
					if (attr && attr->value_size() > 0) japan_ = string_type(attr->value());
				}
				
				pos = p2->first_node(LITERAL("w:sz"));
				if (pos) {
					attr_ptr attr = pos->first_attribute(LITERAL("w:val"));
					if (attr && attr->value_size()) size_ = clx::lexical_cast<double>(attr->value()) / 2.0;
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_styles
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_style& xread_styles(XMLNode* root) {
				for (node_ptr child = root->first_node(); child; child = child->next_sibling()) {
					if (string_type(child->name()) != LITERAL("w:style")) continue;
					attr_ptr attr = child->first_attribute(LITERAL("w:type"));
					if (!attr || attr->value_size() == 0 ||
						string_type(attr->value()) != LITERAL("paragraph")) continue;
					
					attr = child->first_attribute(LITERAL("w:styleId"));
					if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find styleId attribute");
					string_type id(attr->value());
					
					attr = child->first_attribute(LITERAL("w:default"));
					if (attr) this->xread_pdefault(child, id);
					else continue;
					read_pstyle(child->first_node(LITERAL("w:pPr")), p_[id]);
					read_pstyle(child->first_node(LITERAL("w:rPr")), p_[id]);
				}
				
				for (node_ptr child = root->first_node(); child; child = child->next_sibling()) {
					if (string_type(child->name()) != LITERAL("w:style")) continue;
					attr_ptr attr = child->first_attribute(LITERAL("w:type"));
					if (!attr || attr->value_size() == 0 ||
						string_type(attr->value()) != LITERAL("paragraph")) continue;
					
					attr = child->first_attribute(LITERAL("w:styleId"));
					if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find styleId attribute");
					string_type id(attr->value());
					
					attr = child->first_attribute(LITERAL("w:default"));
					if (!attr) this->xread_pbase(child, id);
					else continue;
					read_pstyle(child->first_node(LITERAL("w:pPr")), p_[id]);
					read_pstyle(child->first_node(LITERAL("w:rPr")), p_[id]);
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_pdefault
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_style& xread_pdefault(XMLNode* root, const string_type& id) {
				node_ptr pos = root->first_node(LITERAL("w:name"));
				if (!pos) throw std::runtime_error("cannot find <w:name> tag");
				
				attr_ptr attr = pos->first_attribute(LITERAL("w:val"));
				if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find val attribute");
				p_[id] = init_pstyle(attr->value());
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_pbase
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_style& xread_pbase(XMLNode* root, const string_type& id) {
				node_ptr pos = root->first_node(LITERAL("w:basedOn"));
				//if (!pos) throw std::runtime_error("cannot find <w:basedOn> tag");
				if (!pos) return *this;
				
				attr_ptr attr = pos->first_attribute(LITERAL("w:val"));
				if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find val attribute");
				string_type base(attr->value());
				if (p_.find(base) == p_.end()) throw std::runtime_error("cannot find base style");
				p_[id] = p_[base];
				
				return *this;
			}
		};
	}
}

#endif // FAML_DOCX_STYLE_H
