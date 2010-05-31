/* ------------------------------------------------------------------------- */
/*
 *  docx/document.h
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
 *  Last-modified: Mon 15 Jun 2009 22:24:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_DOCX_DOCUMENT_H
#define FAML_DOCX_DOCUMENT_H

#include <stdexcept>
#include <string>
#include <vector>
#include "rapidxml/rapidxml.hpp"
#include "clx/literal.h"
#include "clx/lexical_cast.h"
#include "clx/shared_ptr.h"
#include "clx/unzip.h"
#include "../officex/theme.h"
#include "../officex/reference.h"
#include "../xlsx/chart.h"
#include "bullet.h"
#include "paragraph.h"
#include "container.h"
#include "table.h"
#include "style.h"

namespace faml {
	namespace docx {
		/* ----------------------------------------------------------------- */
		//  basic_document
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_document {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef std::vector<size_type> container;
			typedef typename container::value_type value_type;
			typedef typename container::const_iterator const_iterator;
			typedef basic_paragraph<CharT, Traits> paragraph;
			typedef basic_table<CharT, Traits> txtable;
			typedef faml::xlsx::basic_chart<CharT, Traits> chart_type;
			typedef basic_container<1, CharT, Traits> header_type;
			typedef basic_container<2, CharT, Traits> footer_type;
			
			enum { text = 1, table = 2 };
			
			basic_document() :
				types_(), texts_(), tbls_(),
				header_(), footer_(),
				width_(0.0), height_(0.0),
				margin_top_(0.0), margin_bottom_(0.0),
				margin_left_(0.0), margin_right_(0.0),
				margin_header_(0.0), margin_footer_(0.0),
				in_(), style_(), bullet_(), theme_() {}
			
			basic_document(const string_type& path) :
				types_(), texts_(), tbls_(),
				header_(), footer_(),
				width_(0.0), height_(0.0),
				margin_top_(0.0), margin_bottom_(0.0),
				margin_left_(0.0), margin_right_(0.0),
				margin_header_(0.0), margin_footer_(0.0),
				in_(), style_(), bullet_(), theme_() {
				this->read(path);
			}
			
			virtual ~basic_document() throw() {}
			
			basic_document& read(const string_type& path) {
				if (!in_.open(path)) throw std::runtime_error("cannot find input file");
				
				// 1. theme1.xml
				typename storage_type::iterator pos = in_.find(LITERAL("word/theme/theme1.xml"));
				if (pos != in_.end()) theme_ = theme_ptr(new theme_type(*pos));
				
				// 2. styles.xml
				pos = in_.find(LITERAL("word/styles.xml"));
				if (pos != in_.end()) style_.read(*pos);
				
				// 3. numbering.xml
				pos = in_.find(LITERAL("word/numbering.xml"));
				if (pos != in_.end()) bullet_.read(*pos);
				
				// 4. document.xml
				pos = in_.find(LITERAL("word/document.xml"));
				if (pos == in_.end()) throw std::runtime_error("cannot find document.xml");
				this->xread(*pos, theme_);
				
				// 5. header1.xml
				pos = in_.find(LITERAL("word/header1.xml"));
				if (pos != in_.end()) header_.read(*pos, *this);
				
				// 6. footer1.xml
				pos = in_.find(LITERAL("word/footer1.xml"));
				if (pos != in_.end()) footer_.read(*pos, *this);
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			bool empty() const { return types_.empty(); }
			size_type size() const { return types_.size(); }
			size_type at(size_type pos) const { return types_.at(pos); }
			size_type operator[](size_type pos) const { return types_[pos]; }
			const_iterator begin() const { return types_.begin(); }
			const_iterator end() const { return types_.end(); }
			const std::vector<paragraph>& texts() const { return texts_; }
			const std::vector<txtable>& tables() const { return tbls_; }
			const header_type& header() const { return header_; }
			const footer_type& footer() const { return footer_; }
			double width() const { return width_; }
			double height() const { return height_; }
			double margin_top() const { return margin_top_; }
			double margin_bottom() const { return margin_bottom_; }
			double margin_left() const { return margin_left_; }
			double margin_right() const { return margin_right_; }
			double margin_header() const { return margin_header_; }
			double margin_footer() const { return margin_footer_; }
			
		public:
		//private: // future works
			typedef clx::basic_unzip<CharT, Traits> storage_type;
			typedef faml::officex::basic_theme<CharT, Traits> theme_type;
			typedef clx::shared_ptr<theme_type> theme_ptr;
			typedef basic_style<CharT, Traits> style_type;
			typedef basic_bullet<CharT, Traits> bullet_type;
			
			storage_type& storage() { return in_; }
			theme_ptr& theme() { return theme_; }
			const theme_ptr& theme() const { return theme_; }
			const style_type& style() const { return style_; }
			const bullet_type bullet() const { return bullet_; }
			
		private:
			typedef rapidxml::xml_node<CharT>* node_ptr;
			typedef rapidxml::xml_attribute<CharT>* attr_ptr;
			
			std::vector<size_type> types_;
			std::vector<paragraph> texts_;
			std::vector<txtable> tbls_;
			header_type header_;
			footer_type footer_;
			
			double width_;
			double height_;
			double margin_top_;
			double margin_bottom_;
			double margin_left_;
			double margin_right_;
			double margin_header_;
			double margin_footer_;
			
			storage_type in_;
			style_type style_;
			bullet_type bullet_;
			theme_ptr theme_;
			
			/* ------------------------------------------------------------- */
			//  xread
			/* ------------------------------------------------------------- */
			template <class Ch, class Tr>
			basic_document& xread(std::basic_istream<Ch, Tr>& in, const theme_ptr& theme) {
				std::vector<char_type> s;
				clx::read(in, s);
				s.push_back(0); // make null terminated string.
				
				//rapidxml::xml_document<char_type> doc; // <- ??? compile error!!
				rapidxml::xml_document<char> doc;
				doc.parse<0>(reinterpret_cast<char_type*>(&s.at(0)));
				node_ptr root = doc.first_node(LITERAL("w:document"));
				if (!root) throw std::runtime_error("cannot find <w:document> (root) tag");
				node_ptr parent = root->first_node(LITERAL("w:body"));
				if (!parent) throw std::runtime_error("cannot find <w:body> tag");
				
				std::map<string_type, string_type> ref;
				this->xread_reference(ref);
				
				for (node_ptr child = parent->first_node(); child; child = child->next_sibling()) {
					if (string_type(child->name()) == LITERAL("w:p")) {
						types_.push_back(text);
						paragraph elem;
						elem.style(style_);
						elem.bullets(bullet_);
						elem.read(child);
						
						std::basic_stringstream<CharT, Traits> ss;
						typedef typename paragraph::image_container::iterator iter;
						for (iter it = elem.images().begin(); it != elem.images().end(); ++it) {
							if (ref.find(it->reference()) == ref.end()) {
								throw std::runtime_error("cannot find reference");
							}
							
							string_type tmp = ref[it->reference()];
							if (tmp.compare(0, 5, LITERAL("chart")) == 0) {
								it->reference(tmp);
								this->xread_chart(tmp, elem);
								continue;
							}
							
							ss.str(LITERAL(""));
							ss << LITERAL("word/media/") << ref[it->reference()];
							it->reference(ss.str());
						}
						
						texts_.push_back(elem);
					}
					else if (string_type(child->name()) == LITERAL("w:tbl")) {
						types_.push_back(table);
						txtable elem;
						elem.read(child, *this);
						tbls_.push_back(elem);
					}
				}
				
				node_ptr pos = parent->first_node(LITERAL("w:sectPr"));
				if (!pos) throw std::runtime_error("cannot find <w:sectPr> tag");
				this->xread_page(pos);
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_page
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_document& xread_page(XMLNode* root) {
				node_ptr pos = root->first_node(LITERAL("w:pgSz"));
				if (!pos) throw std::runtime_error("cannot find <w:pgSz> tag");
				attr_ptr attr = pos->first_attribute(LITERAL("w:w"));
				if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find w:w attribute");
				width_ = clx::lexical_cast<double>(attr->value());
				attr = pos->first_attribute(LITERAL("w:h"));
				if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find w:h attribute");
				height_ = clx::lexical_cast<double>(attr->value());
				
				pos = root->first_node(LITERAL("w:pgMar"));
				if (pos) {
					attr = pos->first_attribute(LITERAL("w:top"));
					if (attr && attr->value_size() > 0) {
						margin_top_ = clx::lexical_cast<double>(attr->value());
					}
					
					attr = pos->first_attribute(LITERAL("w:right"));
					if (attr && attr->value_size() > 0) {
						margin_right_ = clx::lexical_cast<double>(attr->value());
					}
					
					attr = pos->first_attribute(LITERAL("w:bottom"));
					if (attr && attr->value_size() > 0) {
						margin_bottom_ = clx::lexical_cast<double>(attr->value());
					}
					
					attr = pos->first_attribute(LITERAL("w:left"));
					if (attr && attr->value_size() > 0) {
						margin_left_ = clx::lexical_cast<double>(attr->value());
					}
					
					attr = pos->first_attribute(LITERAL("w:header"));
					if (attr && attr->value_size() > 0) {
						margin_header_ = clx::lexical_cast<double>(attr->value());
					}
					
					attr = pos->first_attribute(LITERAL("w:footer"));
					if (attr && attr->value_size() > 0) {
						margin_footer_ = clx::lexical_cast<double>(attr->value());
					}
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_chart
			/* ------------------------------------------------------------- */
			template <class Type>
			basic_document& xread_chart(const string_type& src, Type& dest) {
				std::basic_stringstream<CharT, Traits> ss;
				ss << LITERAL("word/charts/") << src;
				typename storage_type::iterator pos = in_.find(ss.str());
				if (pos == in_.end()) throw std::runtime_error("cannot find chart file");
				
				chart_type ch;
				if (theme_) ch.theme(theme_.get());
				ch.read(*pos);
				dest.charts().push_back(ch);
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_reference
			/* ------------------------------------------------------------- */
			template <class Container>
			basic_document& xread_reference(Container& dest) {
				typename storage_type::iterator pos = in_.find(LITERAL("word/_rels/document.xml.rels"));
				if (pos == in_.end()) return *this;
				faml::officex::read_reference(*pos, dest);
				return *this;
			}
		};
	}
}

#endif // FAML_DOCX_DOCUMENT_H
