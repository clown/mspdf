/* ------------------------------------------------------------------------- */
/*
 *  xslx/utility.h
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
#ifndef FAML_XLSX_UTILITY_H
#define FAML_XLSX_UTILITY_H

#include <string>
#include "rapidxml/rapidxml.hpp"
#include "clx/literal.h"
#include "../officex/color.h"

namespace faml {
	namespace xlsx {
		/* ----------------------------------------------------------------- */
		//  getindex
		/* ----------------------------------------------------------------- */
		template <class Ch, class Tr>
		inline std::pair<size_t, size_t> getindex(const std::basic_string<Ch, Tr>& s) {
			static const Ch lst[] = LITERAL("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
			
			std::pair<size_t, size_t> dest;
			size_t i = 0;
			for (; i < s.size(); ++i) {
				if (s.at(i) >= LITERAL('0') && s.at(i) <= LITERAL('9')) break;
				dest.second *= 26;
				for (size_t j = 0; j < sizeof(lst) / sizeof(Ch); ++j) {
					if (s.at(i) == lst[j]) {
						dest.second += j + 1;
						break;
					}
				}
			}
			
			dest.first = clx::lexical_cast<size_t>(s.substr(i));
			return dest;
		}
		
		template <class CharT>
		inline std::pair<size_t, size_t> getindex(const CharT* s) {
			std::basic_string<CharT> tmp(s);
			return getindex(tmp);
		}
		
		/* ----------------------------------------------------------------- */
		//  getrgb_from_attr
		/* ----------------------------------------------------------------- */
		template <class CharT>
		size_t getrgb_from_attr(rapidxml::xml_node<CharT>* root) {
			typedef rapidxml::xml_attribute<CharT>* attr_ptr;
			
			if (!root) return 0;
			size_t dest = 0;
			attr_ptr attr = root->first_attribute(LITERAL("rgb"));
			if (attr) dest = clx::lexical_cast<size_t>(attr->value(), std::ios::hex) & 0x00ffffff;
			else if ((attr = root->first_attribute(LITERAL("indexed"))) != NULL) {
				size_t pos = clx::lexical_cast<size_t>(attr->value());
				dest = faml::officex::indexedrgb(pos, 8);
			}
			
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  getrgb_from_attr
		/* ----------------------------------------------------------------- */
		template <class CharT, class Palette>
		size_t getrgb_from_attr(rapidxml::xml_node<CharT>* root, const Palette& v) {
			typedef rapidxml::xml_attribute<CharT>* attr_ptr;
			if (!root) return 0;
			
			size_t dest = 0;
			attr_ptr attr = root->first_attribute(LITERAL("theme"));
			if (attr) {
				size_t pos = clx::lexical_cast<size_t>(attr->value());
				if (pos == 0) pos = 1;
				else if (pos == 1) pos = 0;
				if (pos < v.size()) dest = v.at(pos);
			}
			else dest = getrgb_from_attr(root);
			
			return dest;
		}
	}
}

#endif // FAML_XLSX_UTILITY_H
