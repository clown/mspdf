/* ------------------------------------------------------------------------- */
/*
 *  officex/theme.h
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
 *  Last-modified: Thu 11 Jun 2009 11:49:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_OFFICEX_THEME_H
#define FAML_OFFICEX_THEME_H

#include <stdexcept>
#include <string>
#include <vector>
#include "rapidxml/rapidxml.hpp"
#include "clx/lexical_cast.h"
#include "clx/literal.h"
#include "clx/utility.h"
#include "color.h"

namespace faml {
	namespace officex {
		/* ----------------------------------------------------------------- */
		//  basic_palette
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_palette {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef std::vector<size_type> container;
			typedef typename container::value_type value_type;
			
			basic_palette() : v_() {}
			
			template <class XMLNode>
			explicit basic_palette(XMLNode* root) :
				v_() {
				this->read(root);
			}
			
			virtual ~basic_palette() throw() {}
			
			template <class XMLNode>
			basic_palette& read(XMLNode* root) {
				if (!root) throw std::runtime_error("cannot find <a:clrScheme> tag");
				for (node_ptr child = root->first_node(); child; child = child->next_sibling()) {
					size_type rgb = getrgb(child);
					v_.push_back(rgb);
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
			const container& data() const { return v_; }
			container& data() { return v_; }
			
		private:
			typedef rapidxml::xml_node<CharT>* node_ptr;
			typedef rapidxml::xml_attribute<CharT>* attr_ptr;
			
			container v_;
		};
		
		/* ----------------------------------------------------------------- */
		//  basic_theme
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_theme {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef basic_palette<CharT, Traits> palette_type;
			typedef std::pair<string_type, string_type> font_pair;
			
			basic_theme() :
				latin_(), japan_(), palette_() {}
			
			template <class InStream>
			explicit basic_theme(InStream& in) :
				latin_(), japan_(), palette_() {
				this->read(in);
			}
			
			virtual ~basic_theme() throw() {}
			
			template <class InStream>
			basic_theme& read(InStream& in) {
				STATIC_CHECK(sizeof(CharT) == sizeof(char), currently_limited_to_char_type);
				
				std::vector<char_type> s;
				clx::read(in, s);
				s.push_back(0); // make null terminated string.
				
				//rapidxml::xml_document<char_type> doc; // <- ??? compile error!!
				rapidxml::xml_document<char> doc;
				doc.parse<0>(reinterpret_cast<char_type*>(&s.at(0)));
				
				node_ptr tmp = doc.first_node(LITERAL("a:theme"));
				if (!tmp) throw std::runtime_error("cannot find <a:theme> (root) tag");
				node_ptr root = tmp->first_node(LITERAL("a:themeElements"));
				if (!root) throw std::runtime_error("cannot find <a:themeElements> tag");
				
				// 1. color palette
				palette_.read(root->first_node(LITERAL("a:clrScheme")));
				
				// 2. major/minor fonts
				this->xread_major(root->first_node(LITERAL("a:fontScheme")));
				this->xread_minor(root->first_node(LITERAL("a:fontScheme")));
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			const font_pair& latin() const { return latin_; }
			const font_pair& japan() const { return japan_; }
			const palette_type& palette() const { return palette_; }
			
			const string_type& latin(const string_type& which) const {
				return this->xwhich(latin_, which);
			}
			
			const string_type& japan(const string_type& which) const {
				return this->xwhich(japan_, which);
			}
			
		private:
			typedef rapidxml::xml_node<CharT>* node_ptr;
			typedef rapidxml::xml_attribute<CharT>* attr_ptr;
			
			font_pair latin_;
			font_pair japan_;
			palette_type palette_;
			
			/* ------------------------------------------------------------- */
			/*
			 *  xread_major
			 *
			 *  Read Latin and Japanese major (primary) fonts.
			 */
			/* ------------------------------------------------------------- */
			basic_theme& xread_major(node_ptr root) {
				if (!root) throw std::runtime_error("cannot find <a:fontScheme> tag");
				
				node_ptr child = root->first_node(LITERAL("a:majorFont"));
				if (!child) throw std::runtime_error("cannot find <a:majorFont> tag");
				node_ptr pos = child->first_node(LITERAL("a:latin"));
				if (!pos) throw std::runtime_error("cannot find <a:latin> tag");
				attr_ptr attr = pos->first_attribute(LITERAL("typeface"));
				if (attr && attr->value_size() > 0) latin_.first = attr->value();
				
				for (pos = child->first_node(LITERAL("a:font")); pos; pos = pos->next_sibling()) {
					attr = pos->first_attribute(LITERAL("script"));
					if (attr && attr->value_size() > 0 && string_type(attr->value()) == LITERAL("Jpan")) {
						attr = pos->first_attribute(LITERAL("typeface"));
						if (attr && attr->value_size() > 0) japan_.first = attr->value();
						break;
					}
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			/*
			 *  xread_minor
			 *
			 *  Read Latin and Japanese minor (secondary) fonts.
			 */
			/* ------------------------------------------------------------- */
			basic_theme& xread_minor(node_ptr root) {
				if (!root) throw std::runtime_error("cannot find <a:fontScheme> tag");
				
				node_ptr child = root->first_node(LITERAL("a:minorFont"));
				if (!child) return *this;
				node_ptr pos = child->first_node(LITERAL("a:latin"));
				if (!pos) throw std::runtime_error("cannot find <a:latin> tag");
				attr_ptr attr = pos->first_attribute(LITERAL("typeface"));
				if (attr && attr->value_size() > 0) latin_.second = attr->value();
				for (pos = child->first_node(LITERAL("a:font")); pos; pos = pos->next_sibling()) {
					attr = pos->first_attribute(LITERAL("script"));
					if (attr && attr->value_size() > 0 && string_type(attr->value()) == LITERAL("Japan")) {
						attr = pos->first_attribute(LITERAL("typeface"));
						if (attr && attr->value_size() > 0) japan_.second = attr->value();
						break;
					}
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xwhich
			/* ------------------------------------------------------------- */
			const string_type& xwhich(const font_pair& src, const string_type& s) const {
				if (s == LITERAL("major")) return src.first;
				else if (s == LITERAL("minor")) return src.second;
				throw std::runtime_error("please set major/minor");
			}
		};
	}
}
#endif // FAML_XLSX_THEME_H
