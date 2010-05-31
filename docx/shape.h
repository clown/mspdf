/* ------------------------------------------------------------------------- */
/*
 *  docx/shape.h
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
 *  Last-modified: Fri 19 Jun 2009 01:34:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_DOCX_SHAPE_H
#define FAML_DOCX_SHAPE_H

#include <stdexcept>
#include <string>
#include "rapidxml/rapidxml.hpp"
#include "clx/literal.h"
#include "clx/lexical_cast.h"
#include "clx/utility.h"
#include "clx/split.h"
#include "../coordinate.h"
#include "../officex/unit.h"
#include "style.h"
#include "bullet.h"

namespace faml {
	namespace docx {
		using faml::pdf::coordinate;
		template <class CharT, class Traits> class basic_paragraph;
		
		/* ----------------------------------------------------------------- */
		//  basic_shape
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_shape {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef std::map<size_type, size_type> adjust_map;
			typedef basic_paragraph<CharT, Traits> paragraph_type;
			typedef std::vector<paragraph_type> text_container;
			typedef basic_style<CharT, Traits> style_type;
			typedef basic_bullet<CharT, Traits> bullet_type;
			
			basic_shape() :
				origin_(), type_(0), width_(0.0), height_(0.0),
				weight_(1.0), border_type_(0), border_(-1), background_(-1), adjust_(),
				texts_(), style_(NULL), bullet_(NULL) {}
			
			template <class XMLNode>
			basic_shape(XMLNode* root) :
				origin_(), type_(0), width_(0.0), height_(0.0),
				weight_(1.0), border_type_(0), border_(-1), background_(-1), adjust_(),
				texts_(), style_(NULL), bullet_(NULL) {
				this->read(root);
			}
			
			virtual ~basic_shape() throw() {}
			
			template <class XMLNode>
			basic_shape& read(XMLNode* root) {
				if (!root) throw std::runtime_error("cannot find <w:pict> tag");
				node_ptr pos = root->first_node(LITERAL("v:rect"));
				if (pos) type_ = 1;
				else {
					this->xread_type(root->first_node(LITERAL("v:shapetype")));
					pos = root->first_node(LITERAL("v:shape"));
				}
				this->xread_form(pos);
				
				node_ptr child = pos->first_node(LITERAL("v:textbox"));
				if (child) this->xread_text(child->first_node(LITERAL("w:txbxContent")));
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			const coordinate& origin() const { return origin_; }
			size_type type() const { return type_; }
			double width() const { return width_; }
			double height() const { return height_; }
			double weight() const { return weight_; }
			size_type border() const { return border_; }
			size_type border_type() const { return border_type_; }
			size_type background() const { return background_; }
			const adjust_map& adjusts() const { return adjust_; }
			adjust_map& adjusts() { return adjust_; }
			const text_container& texts() const { return texts_; }
			
			/* ------------------------------------------------------------- */
			//  Access methods (set).
			/* ------------------------------------------------------------- */
			void style(const style_type& cp) { style_ = &cp; }
			void bullets(const bullet_type& cp) { bullet_ = &cp; }
			
		private:
			typedef rapidxml::xml_node<CharT>* node_ptr;
			typedef rapidxml::xml_attribute<CharT>* attr_ptr;
			
			coordinate origin_;
			size_type type_;
			double width_;
			double height_;
			double weight_;
			size_type border_type_;
			size_type border_;
			size_type background_;
			adjust_map adjust_;
			
			text_container texts_;
			const style_type* style_;
			const bullet_type* bullet_;
			
			/* ------------------------------------------------------------- */
			//  xread_type
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_shape& xread_type(XMLNode* root) {
				if (!root) throw std::runtime_error("cannot find <v:shapetype> tag");
				
				attr_ptr attr = root->first_attribute(LITERAL("o:spt"));
				if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find o:spt attribute");
				type_ = clx::lexical_cast<size_type>(attr->value());
				attr = root->first_attribute(LITERAL("adj"));
				if (attr && attr->value_size() > 0) {
					string_type s(attr->value());
					std::vector<string_type> v;
					clx::split_if(s, v, clx::is_any_of(LITERAL(",")));
					for (size_type i = 0; i < v.size(); ++i) {
						int value = clx::lexical_cast<int>(v.at(i));
						adjust_[i] = (value > 0) ? value : 0;
					}
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_form
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_shape& xread_form(XMLNode* root) {
				if (!root) throw std::runtime_error("cannot find <v:shape> tag");
				
				attr_ptr attr = root->first_attribute(LITERAL("strokeweight"));
				if (attr && attr->value_size() > 0) {
					weight_ = faml::officex::emu(clx::lexical_cast<double>(attr->value()));
					border_type_ = 0x001;
					border_ = 0;
				}
				
				attr = root->first_attribute(LITERAL("style"));
				if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find style attribute");
				string_type s(attr->value());
				std::vector<string_type> v;
				clx::split_if(s, v, clx::is_any_of(LITERAL(";:")));
				
				// origin, width, and height
				double x = 0.0, y = 0.0;
				for (size_type i = 0; i < v.size(); ++i) {
					if (i + 1 >= v.size()) break;
					double val = 0.0;
					if (v.at(i) == LITERAL("margin-left") ||
						v.at(i) == LITERAL("margin-top") ||
						v.at(i) == LITERAL("width") ||
						v.at(i) == LITERAL("height")) {
						if (v.at(i + 1).find(LITERAL("pt"))) {
							val = clx::lexical_cast<double>(v.at(i + 1).substr(
								0, v.at(i + 1).find(LITERAL("pt"))
							));
						}
						else if (v.at(i + 1).find("in")) {
							double tmp = clx::lexical_cast<double>(v.at(i + 1).substr(
								0, v.at(i + 1).find(LITERAL("in"))
							));
							val = faml::officex::inch(tmp);
						}
						else throw std::runtime_error("emerged unexpected unit");
					}
					
					if (v.at(i) == LITERAL("margin-left")) x = val;
					else if (v.at(i) == LITERAL("margin-top")) y = val;
					else if (v.at(i) == LITERAL("width")) width_ = val;
					else if (v.at(i) == LITERAL("height")) height_ = val;
				}
				origin_ = coordinate(x, y);
				
				// ToDo: completely implement.
				string_type type;
				node_ptr pos = root->first_node(LITERAL("v:stroke"));
				if (pos) {
					attr = pos->first_attribute(LITERAL("linestyle"));
					if (!attr) attr = pos->first_attribute(LITERAL("dashstyle"));
					if (attr && attr->value_size() > 0) type = attr->value();
				}
				
				if (type == LITERAL("double")) {
					border_type_ |= 0x010;
					weight_ /= 2.0;
				}
				else if (type == LITERAL("thickThin")) {
					border_type_ |= 0x030;
					weight_ = weight_ * 2.0 / 3.0;
				}
				else if (type == LITERAL("thinThick")) {
					border_type_ |= 0x050;
					weight_ = weight_ * 1.0 / 3.0;
				}
				else if (type == LITERAL("dotted") || type == LITERAL("shortdot")) {
					border_type_ = 0x003;
				}
				else if (type == LITERAL("dashed") || type == LITERAL("dotDash") || type == LITERAL("dotDotDash")) {
					border_type_ = 0x002;
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_text
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_shape& xread_text(XMLNode* root) {
				if (!root) throw std::runtime_error("cannot find <w:txbxContent>");
				for (node_ptr child = root->first_node(); child; child = child->next_sibling()) {
					if (string_type(child->name()) == LITERAL("w:p")) {
						paragraph_type elem;
						if (style_) elem.style(*style_);
						if (bullet_) elem.bullets(*bullet_);
						elem.read(child);
						texts_.push_back(elem);
					}
				}
				
				return *this;
			}
		};
	}
}

#endif // FAML_DOCX_SHAPE_H
