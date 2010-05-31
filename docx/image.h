/* ------------------------------------------------------------------------- */
/*
 *  docx/imaget.h
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
#ifndef FAML_DOCX_IMAGE_H
#define FAML_DOCX_IMAGE_H

#include <stdexcept>
#include <string>
#include "rapidxml/rapidxml.hpp"
#include "clx/literal.h"
#include "clx/lexical_cast.h"

namespace faml {
	namespace docx {
		using faml::pdf::coordinate;
		
		/* ----------------------------------------------------------------- */
		//  basic_image
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_image {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			
			basic_image() :
				reference_(), offset_(),
				width_(0.0), height_(0.0), position_(0) {}
			
			template <class XMLNode>
			basic_image(XMLNode* root) :
				reference_(), offset_(),
				width_(0.0), height_(0.0), position_(0) {
				this->read(root);
			}
			
			virtual ~basic_image() throw() {}
			
			template <class XMLNode>
			basic_image& read(XMLNode* root) {
				if (!root) throw std::runtime_error("cannot find <w:drawing> tag");
				
				this->xread_position(root->first_node(LITERAL("wp:anchor")));
				this->xread_size(root);
				this->xread_reference(root);
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			const string_type& reference() const { return reference_; }
			const coordinate& offset() const { return offset_; }
			double width() const { return width_; }
			double height() const { return height_; }
			size_type position() const { return position_; }
			
			/* ------------------------------------------------------------- */
			//  Access methods (set).
			/* ------------------------------------------------------------- */
			void reference(const string_type& cp) { reference_ = cp; }
			void offset(const coordinate& cp) { offset_ = cp; }
			void width(double cp) { width_ = cp; }
			void height(double cp) { height_ = cp; }
			
		private:
			typedef rapidxml::xml_node<CharT>* node_ptr;
			typedef rapidxml::xml_attribute<CharT>* attr_ptr;
			
			string_type reference_;
			coordinate offset_;
			double width_;
			double height_;
			size_type position_;
			
			template <class XMLNode>
			basic_image& xread_position(XMLNode* root) {
				if (!root) return *this;
				
				node_ptr pos = root->first_node(LITERAL("wp:positionH"));
				if (!pos) throw std::runtime_error("cannot find <wp:positionH> tag");
				node_ptr child = pos->first_node(LITERAL("wp:posOffset"));
				if (!child || child->value_size() == 0) throw std::runtime_error("cannot find <wp:posOffset> tag");
				offset_.x(faml::officex::emu(clx::lexical_cast<double>(child->value())));
				
				pos = root->first_node(LITERAL("wp:positionV"));
				if (!pos) throw std::runtime_error("cannot find <wp:positionV> tag");
				child = pos->first_node(LITERAL("wp:posOffset"));
				if (!child || child->value_size() == 0) throw std::runtime_error("cannot find <wp:posOffset> tag");
				offset_.y(faml::officex::emu(clx::lexical_cast<double>(child->value())));
				
				position_ = 1;
				return *this;
			}
			
			template <class XMLNode>
			basic_image& xread_size(XMLNode* root) {
				if (!root) throw std::runtime_error("cannot find <w:drawing> tag");
				node_ptr parent = root->first_node();
				if (!parent) throw std::runtime_error("cannot find any child-tags of <w:drawing>");
				node_ptr pos = parent->first_node(LITERAL("wp:extent"));
				if (!pos) throw std::runtime_error("cannot find <wp:extent> tag");
				
				attr_ptr attr = pos->first_attribute(LITERAL("cx"));
				if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find cx attribute");
				width_ = faml::officex::emu(clx::lexical_cast<double>(attr->value()));
				
				attr = pos->first_attribute(LITERAL("cy"));
				if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find cx attribute");
				height_ = faml::officex::emu(clx::lexical_cast<double>(attr->value()));
				return *this;
			}
			
			template <class XMLNode>
			basic_image& xread_reference(XMLNode* root) {
				if (!root) throw std::runtime_error("cannot find <w:drawing> tag");
				node_ptr parent = root->first_node();
				if (!parent) throw std::runtime_error("cannot find any child-tags of <w:drawing>");
				
				node_ptr p1 = parent->first_node(LITERAL("a:graphic"));
				if (!p1) throw std::runtime_error("cannot find <a:graphic> tag");
				node_ptr p2 = p1->first_node(LITERAL("a:graphicData"));
				if (!p2) throw std::runtime_error("cannot find <a:graphicData> tag");
				node_ptr p3 = p2->first_node(LITERAL("pic:pic"));
				if (!p3) throw std::runtime_error("cannot find <pic:pic> tag");
				node_ptr p4 = p3->first_node(LITERAL("pic:blipFill"));
				if (!p4) throw std::runtime_error("cannot find <pic:blipFill> tag");
				node_ptr pos = p4->first_node(LITERAL("a:blip"));
				if (!pos) throw std::runtime_error("cannot find <a:blip> tag");
				attr_ptr attr = pos->first_attribute(LITERAL("r:embed"));
				if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find r:embed attribute");
				reference_ = string_type(attr->value());
				
				return *this;
			}
		};
	}
}

#endif // FAML_DOCX_IMAGE_H
