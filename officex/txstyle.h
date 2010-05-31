/* ------------------------------------------------------------------------- */
/*
 *  officex/style.h
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
 *  Last-modified: Sat 13 Jun 2009 00:38:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_OFFICEX_TXSTYLE_H
#define FAML_OFFICEX_TXSTYLE_H

#include <string>
#include "rapidxml/rapidxml.hpp"
#include "clx/literal.h"
#include "clx/lexical_cast.h"
#include "color.h"
#include "utility.h"
#include "font.h"
#include "theme.h"
#include "bullet.h"

namespace faml {
	namespace officex {
		/* ----------------------------------------------------------------- */
		/*
		 *  basic_txstyle
		 *
		 *  Properties about text style in the current PowerPoint file.
		 */
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_txstyle {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef basic_font<CharT, Traits> font_type;
			typedef basic_theme<CharT, Traits> theme_type;
			typedef std::map<string_type, string_type> color_map;
			
			explicit basic_txstyle(const color_map& clr = color_map()) :
				font_(), align_(0), valign_(0), indent_(0.0),
				butype_(0), bullet_(), buclr_(0), busize_(0.8),
				theme_(NULL), clr_(clr) {}
			
			template <class XMLNode>
			basic_txstyle(XMLNode* root, const color_map& clr = color_map()) :
				font_(), align_(0), valign_(0), indent_(0.0),
				butype_(0), bullet_(), buclr_(0), busize_(0.8),
				theme_(NULL), clr_(clr) {
				this->read(root);
			}
			
			virtual ~basic_txstyle() throw() {}
			
			template <class XMLNode>
			basic_txstyle& read(XMLNode* root) {
				typedef rapidxml::xml_node<CharT>* node_ptr;
				typedef rapidxml::xml_attribute<CharT>* attr_ptr;
				if (!root) throw std::runtime_error("cannot find root tag");
				
				if (font_.latin().empty()) {
					if (theme_) font_.latin(theme_->latin(LITERAL("major")));
					else font_.latin(LITERAL("Arial"));
				}
				
				if (font_.japan().empty()) {
					if (theme_) font_.japan(theme_->japan(LITERAL("major")));
					else font_.japan(LITERAL("ＭＳ ゴシック"));
				}
				
				// 1. left margin
				attr_ptr attr = root->first_attribute(LITERAL("marL"));
				if (attr && attr->value_size() > 0) indent_ = clx::lexical_cast<double>(attr->value());
				
				// 2. align
				attr = root->first_attribute(LITERAL("algn"));
				if (attr && attr->value_size() > 0) align_ = getalign(attr->value());
				
				// 3. font size and font color
				node_ptr pos = root->first_node(LITERAL("a:defRPr"));
				if (pos) {
					attr = pos->first_attribute(LITERAL("sz"));
					if (attr && attr->value_size() > 0) font_.size(clx::lexical_cast<double>(attr->value()) / 100.0);
					
					size_type c = 0;
					if (pos->first_node(LITERAL("a:solidFill"))) {
						if (theme_) c = getrgb(pos->first_node(LITERAL("a:solidFill")), theme_->palette(), clr_);
						else c = getrgb(pos->first_node(LITERAL("a:solidFill")));
					}
					else if (theme_) {
						c = theme_->palette().at(0);
					}
					font_.rgb(c);
					
					node_ptr tmp = pos->first_node(LITERAL("a:latin"));
					if (tmp) {
						attr = tmp->first_attribute(LITERAL("typeface"));
						if (attr && attr->value_size() > 0) {
							string_type f(attr->value());
							if (f != LITERAL("+mj-lt") && f != LITERAL("+mn-lt")) font_.latin(f);
						}
					}
					
					tmp = pos->first_node(LITERAL("a:ea"));
					if (tmp) {
						attr = tmp->first_attribute(LITERAL("typeface"));
						if (attr && attr->value_size() > 0) {
							string_type f(attr->value());
							if (f != LITERAL("+mj-ea") && f != LITERAL("+mn-ea")) font_.japan(f);
						}
					}
				}
				
				// 5. bullet
				pos = root->first_node(LITERAL("a:buNone"));
				if (pos) {
					bullet_.erase();
					butype_ = 0;
				}
				else butype_ = 1;
				
				string_type f;
				pos = root->first_node(LITERAL("a:buFont"));
				if (pos) {
					attr = pos->first_attribute(LITERAL("typeface"));
					if (attr && attr->value_size() > 0) {
						f = string_type(attr->value());
					}
				}
				
				pos = root->first_node(LITERAL("a:buChar"));
				if (pos) {
					attr = pos->first_attribute(LITERAL("char"));
					if (attr && attr->value_size() > 0) {
						if (f.find("Wingdings") != string_type::npos) {
							bullet_ = getbullet(string_type(attr->value()));
						}
						else bullet_ = string_type(attr->value());
						butype_ = 1;
					}
				}
				else {
					bullet_.erase();
					butype_ = 0;
				}
				
				pos = root->first_node(LITERAL("a:buClr"));
				if (pos) {
					if (theme_) buclr_ = getrgb(pos, theme_->palette(), clr_);
					else buclr_ = getrgb(pos);
				}
				else if (theme_) buclr_ = theme_->palette().at(0);
				
				pos = root->first_node(LITERAL("a:buSzPct"));
				if (pos) {
					attr = pos->first_attribute(LITERAL("val"));
					if (attr && attr->value_size() > 0) {
						busize_ = clx::lexical_cast<double>(attr->value()) / 1000.0 / 100.0;
					}
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			const font_type& font() const { return font_; }
			size_type align() const { return align_; }
			size_type valign() const { return valign_; }
			double indent() const { return indent_; }
			size_type butype() const { return butype_; }
			size_type buclr() const { return buclr_; }
			double busize() const { return busize_; }
			const string_type& bullet() const { return bullet_; }
			
			/* ------------------------------------------------------------- */
			//  Access methods (set).
			/* ------------------------------------------------------------- */
			void font(const font_type& cp) { font_ = cp; }
			void align(size_type cp) { align_ = cp; }
			void valign(size_type cp) { valign_ = cp; }
			void indent(double cp) { indent_ = cp; }
			void butype(size_type cp) { butype_ = cp; }
			void bullet(const string_type& cp) { bullet_ = cp; }
			void buclr(size_type cp) { buclr_ = cp; }
			void busize(double cp) { busize_ = cp; }
			void theme(const theme_type* cp) { theme_ = cp; }
			
		private:
			font_type font_;
			size_type align_;
			size_type valign_;
			double indent_;
			size_type butype_;
			string_type bullet_;
			size_type buclr_;
			double busize_;
			const theme_type* theme_;
			color_map clr_;
		};
	}
}

#endif // FAML_OFFICEX_TXSTYLE_H
