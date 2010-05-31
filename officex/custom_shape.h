/* ------------------------------------------------------------------------- */
/*
 *  officex/custom_shape.h
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
#ifndef FAML_OFFICEX_CUSTOM_SHAPE_H
#define FAML_OFFICEX_CUSTOM_SHAPE_H

#include <stdexcept>
#include <string>
#include "rapidxml/rapidxml.hpp"
#include "clx/lexical_cast.h"
#include "clx/literal.h"
#include "clx/utility.h"
#include "../coordinate.h"
#include "color.h"
#include "theme.h"
#include "txbox.h"

namespace faml {
	namespace officex {
		using faml::pdf::coordinate;
		
		/* ----------------------------------------------------------------- */
		//  basic_shape_path
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_shape_path {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef std::vector<coordinate> container;
			typedef container::value_type value_type;
			typedef container::const_iterator const_iterator;
			
			enum { move = 0, line, curve, close };
			
			basic_shape_path() : command_(), v_() {}
			
			template <class XMLNode>
			basic_shape_path(XMLNode* root, double maxw, double maxh) :
				command_(), v_() {
				this->read(root, maxw, maxh);
			}
			
			virtual ~basic_shape_path() throw() {}
			
			template <class XMLNode>
			basic_shape_path& read(XMLNode* root, double maxw, double maxh) {
				if (!root) throw std::runtime_error("cannot find shape path tag");
				
				string_type name = string_type(root->name());
				if (name == LITERAL("a:moveTo")) command_ = move;
				else if (name == LITERAL("a:lnTo")) command_ = line;
				else if (name == LITERAL("a:cubicBezTo")) command_ = curve;
				else if (name == LITERAL("a:close")) command_ = close;
				else throw std::runtime_error("undefined drawing command");
				
				for (node_ptr child = root->first_node(); child; child = child->next_sibling()) {
					if (string_type(child->name()) != LITERAL("a:pt")) continue;
					attr_ptr attr = child->first_attribute(LITERAL("x"));
					if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find x attribute");
					double x = clx::lexical_cast<double>(attr->value()) / maxw;
					
					attr = child->first_attribute(LITERAL("y"));
					if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find x attribute");
					double y = clx::lexical_cast<double>(attr->value()) / maxh;
					
					coordinate elem(x, y);
					v_.push_back(elem);
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			size_type command() const { return command_; }
			bool empty() const { return v_.empty(); }
			size_type size() const { return v_.size(); }
			const value_type& at(size_type pos) const { return v_.at(pos); }
			const value_type& operator[](size_type pos) const { return v_[pos]; }
			const_iterator begin() const { return v_.begin(); }
			const_iterator end() const { return v_.end(); }
			
		private:
			typedef rapidxml::xml_node<CharT>* node_ptr;
			typedef rapidxml::xml_attribute<CharT>* attr_ptr;
			
			size_type command_;
			container v_;
		};
		
		/* ----------------------------------------------------------------- */
		//  basic_shape_path_container
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_shape_path_container {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef basic_shape_path<CharT, Traits> value_type;
			typedef std::vector<value_type> container;
			typedef typename container::const_iterator const_iterator;
			
			basic_shape_path_container() : v_(), fill_(true) {}
			
			template <class XMLNode>
			basic_shape_path_container(XMLNode* root) :
				v_(), fill_(false) {
				this->read(root);
			}
			
			virtual ~basic_shape_path_container() throw() {}
			
			template <class XMLNode>
			basic_shape_path_container& read(XMLNode* root) {
				if (!root) throw std::runtime_error("cannot find <a:path> tag");
				
				double w = 21600.0;
				double h = 21600.0;
				attr_ptr attr = root->first_attribute(LITERAL("w"));
				if (attr && attr->value_size() > 0) {
					w = clx::lexical_cast<double>(attr->value());
				}
				
				attr = root->first_attribute(LITERAL("h"));
				if (attr && attr->value_size() > 0) {
					h = clx::lexical_cast<double>(attr->value());
				}
				
				attr = root->first_attribute(LITERAL("fill"));
				if (attr && attr->value_size() > 0 && string_type(attr->value()) == LITERAL("none")) fill_ = false;
				
				for (node_ptr child = root->first_node(); child; child = child->next_sibling()) {
					value_type elem;
					elem.read(child, w, h);
					if (!((elem.command() == 2 && elem.size() == 3) ||
						  (elem.command() == 3 && elem.size() == 0) ||
						  (elem.size() == 1))) {
						throw std::runtime_error("unexpected number of parameters");
					}
					v_.push_back(elem);
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
			const_iterator begin() const { return v_.begin(); }
			const_iterator end() const { return v_.end(); }
			bool is_fill() const { return fill_; }
			
		private:
			typedef rapidxml::xml_node<CharT>* node_ptr;
			typedef rapidxml::xml_attribute<CharT>* attr_ptr;
			
			container v_;
			bool fill_;
		};
	}
}

#endif // FAML_OFFICEX_CUSTOM_SHAPE_H
