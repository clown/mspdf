/* ------------------------------------------------------------------------- */
/*
 *  docx/container.h
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
 *  Last-modified: Thu 09 Jul 2009 00:47:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_DOCX_CONTAINER_H
#define FAML_DOCX_CONTAINER_H

#include <stdexcept>
#include <string>
#include <vector>
#include "paragraph.h"
#include "clx/literal.h"
#include "clx/unzip.h"
#include "../officex/reference.h"

namespace faml {
	namespace docx {
		/* ----------------------------------------------------------------- */
		/*
		 *  basic_container
		 *
		 *  basic_container class is used when parsing and converting
		 *  the body/header/footer part of the word document. The ``Which''
		 *  template parameter is used when which part of the document
		 *  is currently parsing. The value of ``Which'' parameter is as
		 *  follows: (0: body, 1: header, 2: footer).
		 */
		/* ----------------------------------------------------------------- */
		template <
			int Which,
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_container {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef basic_paragraph<CharT, Traits> value_type;
			typedef std::vector<value_type> container;
			typedef typename container::const_iterator const_iterator;
			
			basic_container() : v_() {}
			
			template <class Ch, class Tr, class DocManager>
			basic_container(std::basic_istream<Ch, Tr>& in, DocManager& dm) :
				v_() {
				this->read(in, dm);
			}
			
			template <class Ch, class Tr, class DocManager>
			basic_container& read(std::basic_istream<Ch, Tr>& in, DocManager& dm) {
				std::vector<char_type> s;
				clx::read(in, s);
				s.push_back(0); // make null terminated string.
				
				//rapidxml::xml_document<char_type> doc; // <- ??? compile error!!
				rapidxml::xml_document<char> doc;
				doc.parse<0>(reinterpret_cast<char_type*>(&s.at(0)));
				string_type tag;
				if (Which == 1) tag = LITERAL("w:hdr");
				else tag = LITERAL("w:ftr");
				node_ptr root = doc.first_node(tag.c_str());
				if (!root) throw std::runtime_error("cannot find root tag");
				
				std::map<string_type, string_type> ref;
				this->xread_reference(ref, dm);
				this->xread(root, ref, dm);
				
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
			
		private:
			typedef rapidxml::xml_node<CharT>* node_ptr;
			typedef rapidxml::xml_attribute<CharT>* attr_ptr;
			
			container v_;
			
			/* ------------------------------------------------------------- */
			/*
			 *  xread
			 *
			 *  Parsing main OpenXML file.
			 */
			/* ------------------------------------------------------------- */
			template <class XMLNode, class Container, class DocManager>
			basic_container& xread(XMLNode* root, Container& ref, DocManager& dm) {
				if (!root) throw std::runtime_error("cannot find root tag");
				
				for (node_ptr child = root->first_node(); child; child = child->next_sibling()) {
					if (string_type(child->name()) != LITERAL("w:p")) continue;
					value_type elem;
					elem.style(dm.style());
					elem.bullets(dm.bullet());
					elem.read(child);
					
					std::basic_stringstream<CharT, Traits> ss;
					typedef typename value_type::image_container::iterator image_iterator;
					for (image_iterator it = elem.images().begin(); it != elem.images().end(); ++it) {
						if (ref.find(it->reference()) == ref.end()) {
							throw std::runtime_error("cannot find reference");
						}
						ss.str(LITERAL(""));
						ss << LITERAL("word/media/") << ref[it->reference()];
						it->reference(ss.str());
					}
					
					v_.push_back(elem);
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_reference
			/* ------------------------------------------------------------- */
			template <class Container, class DocManager>
			basic_container& xread_reference(Container& dest, DocManager& dm) {
				typedef typename DocManager::storage_type::iterator storage_iterator;
				
				string_type file;
				if (Which == 1) file = LITERAL("header1");
				else file = LITERAL("footer1");
				
				std::basic_stringstream<CharT, Traits> ss;
				ss << LITERAL("word/_rels/") << file << LITERAL(".xml.rels");
				
				storage_iterator pos = dm.storage().find(ss.str());
				if (pos == dm.storage().end()) return *this;
				faml::officex::read_reference(*pos, dest);
				
				return *this;
			}
		};
	}
}

#endif // FAML_DOCX_TEMPLATE_H
