/* ------------------------------------------------------------------------- */
/*
 *  docx/pstyle.h
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
 *  Last-modified: Wed 17 Jun 2009 01:11:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_DOCX_PSTYLE_H
#define FAML_DOCX_PSTYLE_H

#include <string>
#include <map>
#include "rapidxml/rapidxml.hpp"
#include "clx/literal.h"
#include "clx/lexical_cast.h"
#include "../officex/utility.h"
#include "styledef.h"

namespace faml {
	namespace docx {
		typedef faml::docx::styles::paragraph pstyle;
		
		/* ----------------------------------------------------------------- */
		/*
		 *  default_paragraph
		 *
		 *  Definition of the default styles. The parameters of the
		 *  paragraph style are as follow:
		 *   - align: text align.
		 *   - rgb: font color.
		 *   - decorate: font decoration (bold|italic|underline|strike).
		 *   - size: font fize.
		 *   - indent: indent size (twip unit).
		 *   - indent1st: indent size of the first line (twip unit).
		 */
		/* ----------------------------------------------------------------- */
		namespace default_paragraph {
			template <class Ch, class Tr>
			static std::map<std::basic_string<Ch, Tr>, pstyle>& initialize() {
				static std::map<std::basic_string<Ch, Tr>, pstyle> v;
				static bool done = false;
				if (done) return v;
				done = true;
				
				v[LITERAL("Normal")] = pstyle(0, 0, 0, 10.5, 0, 0);
				v[LITERAL("heading 1")] = pstyle(0, 0, 0, 10.5, 0, 0);
				v[LITERAL("Header")] = pstyle(0, 0, 0, 10.5, 0, 0);
				v[LITERAL("Footer")] = pstyle(2, 0, 0, 10.5, 0, 0);
				
				return v;
			}
		}
		
		/* ----------------------------------------------------------------- */
		//  init_pstyle
		/* ----------------------------------------------------------------- */
		template <class Ch, class Tr>
		inline pstyle init_pstyle(const std::basic_string<Ch, Tr>& name) {
			typedef std::map<std::basic_string<Ch, Tr>, pstyle> pstyle_map;
			static pstyle_map& v = default_paragraph::initialize<Ch, Tr>();
			if (v.find(name) == v.end()) return pstyle();
			return v[name];
		}
		
		template <class CharT>
		inline pstyle init_pstyle(const CharT* name) {
			std::basic_string<CharT> tmp(name);
			return init_pstyle(tmp);
		}
		
		/* ----------------------------------------------------------------- */
		//  read_pstyle
		/* ----------------------------------------------------------------- */
		template <class CharT>
		inline bool read_pstyle(rapidxml::xml_node<CharT>* root, pstyle& dest) {
			typedef std::basic_string<CharT> string_type;
			typedef rapidxml::xml_node<CharT>* node_ptr;
			typedef rapidxml::xml_attribute<CharT>* attr_ptr;
			
			if (!root) return true;
			
			// 1. text align.
			node_ptr pos = root->first_node(LITERAL("w:jc"));
			if (pos) {
				attr_ptr attr = pos->first_attribute(LITERAL("w:val"));
				if (attr && attr->value_size() > 0) {
					dest.align(faml::officex::getalign(attr->value()));
				}
			}
			
			// 2. font indent
			pos = root->first_node(LITERAL("w:ind"));
			if (pos) {
				//attr_ptr attr = pos->first_attribute(LITERAL("w:val"));
				attr_ptr attr = pos->first_attribute(LITERAL("w:left"));
				if (attr && attr->value_size() > 0) {
					dest.indent(clx::lexical_cast<double>(attr->value()));
				}
				
				attr = pos->first_attribute(LITERAL("w:firstLine"));
				if (attr && attr->value_size() > 0) {
					dest.indent1st(clx::lexical_cast<double>(attr->value()));
				}
			}
			
			// 3. font size.
			dest.size(10.5); // default
			pos = root->first_node(LITERAL("w:sz"));
			//pos = root->first_node(LITERAL("w:szCs"));
			//if (!pos) pos = root->first_node(LITERAL("w:sz"));
			if (pos) {
				attr_ptr attr = pos->first_attribute(LITERAL("w:val"));
				if (attr && attr->value_size() > 0) {
					dest.size(clx::lexical_cast<double>(attr->value()) / 2.0);
				}
			}
			
			// 4. text decoration, and font size.
			size_t deco = dest.decorate();
			pos = root->first_node(LITERAL("w:b"));
			if (pos) deco |= 0x01;
			pos = root->first_node(LITERAL("w:i"));
			if (pos) deco |= 0x02;
			pos = root->first_node(LITERAL("w:u"));
			if (pos) deco |= 0x04;
			pos = root->first_node(LITERAL("w:strike"));
			if (pos) deco |= 0x08;
			dest.decorate(deco);
			
			pos = root->first_node(LITERAL("w:w"));
			if (pos) {
				attr_ptr attr = pos->first_attribute(LITERAL("w:val"));
				if (attr && attr->value_size() > 0) {
					dest.scale(clx::lexical_cast<size_t>(attr->value()));
				}
			}
			
			// 5. font name
			pos = root->first_node(LITERAL("w:rFonts"));
			if (pos) {
				attr_ptr attr = pos->first_attribute(LITERAL("w:ascii"));
				if (attr && attr->value_size() > 0) dest.latin(string_type(attr->value()));
				attr = pos->first_attribute(LITERAL("w:eastAsia"));
				if (attr && attr->value_size() > 0) dest.japan(string_type(attr->value()));
			}
			
			return true;
		}
	}
}

#endif // FAML_DOCX_PSTYLE_H
