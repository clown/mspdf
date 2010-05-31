/* ------------------------------------------------------------------------- */
/*
 *  officex/txbox.h
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
#ifndef FAML_OFFICEX_TXBOX_H
#define FAML_OFFICEX_TXBOX_H

#include <stdexcept>
#include <string>
#include <vector>
#include "rapidxml/rapidxml.hpp"
#include "clx/lexical_cast.h"
#include "clx/literal.h"
#include "clx/utility.h"
#include "color.h"
#include "font.h"
#include "theme.h"
#include "txstyle.h"

namespace faml {
	namespace officex {
		/* ----------------------------------------------------------------- */
		/*
		 *  basic_piece
		 *
		 *  Piece is a minimum unit of a paragraph. A piece has only
		 *  string data while a paragraph has text, image picture,
		 *  and some other shape data.
		 */
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_piece {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef faml::officex::basic_font<CharT, Traits> font_type;
			
			enum { none = 0x00, bold = 0x01, italic = 0x02,
				underline = 0x04, strike = 0x08, hyperlink = 0x10 };
			
			basic_piece() :
				data_(), font_(), decorate_(0), scale_(100), baseline_(0), ascii_(false) {}
			
			basic_piece(const string_type& s) :
				data_(s), font_(), decorate_(0), scale_(100), baseline_(0), ascii_(false) {}
			
			virtual ~basic_piece() throw() {}
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			const string_type& data() const { return data_; }
			const font_type& font() const { return font_; }
			size_type decorate() const { return decorate_; }
			size_type scale() const { return scale_; }
			int baseline() const { return baseline_; }
			bool ascii() const { return ascii_; }
			
			/* ------------------------------------------------------------- */
			//  Access methods (set).
			/* ------------------------------------------------------------- */
			void data(const string_type& cp) { data_ = cp; }
			void font(const font_type& cp) { font_ = cp; }
			void decorate(size_type cp) { decorate_ = cp; }
			void scale(size_type cp) { scale_ = cp; }
			void baseline(int cp) { baseline_ = cp; }
			void ascii(bool cp) { ascii_ = cp; }
			
		private:
			string_type data_;
			font_type font_;
			size_type decorate_;
			size_type scale_;
			int baseline_;
			bool ascii_;
		};
		
		/* ----------------------------------------------------------------- */
		//  basic_paragraph
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_paragraph {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef basic_piece<CharT, Traits> value_type;
			typedef std::vector<value_type> container;
			typedef typename container::const_iterator const_iterator;
			typedef basic_font<CharT, Traits> font_type;
			typedef basic_theme<CharT, Traits> theme_type;
			typedef basic_txstyle<CharT, Traits> style_type;
			typedef std::map<string_type, string_type> color_map;
			
			enum { normal = 0, pagenum };
			
			explicit basic_paragraph(const color_map& clr = color_map()) :
				type_(0), v_(), font_(LITERAL("Arial"), 18.0, 0),
				align_(0), valign_(0), level_(0), indent_(0.0), tab_(914400.0),
				baseline_(-1.0), before_(0.0), after_(0.0),
				bullet_(), buclr_(), decorate_(0),
				theme_(NULL), style_(), clr_(clr) {}
			
			template <class XMLNode>
			basic_paragraph(XMLNode* root, const color_map& clr = color_map()) :
				type_(0), v_(), font_(LITERAL("Arial"), 18.0, 0),
				align_(0), valign_(0), level_(0), indent_(0.0), tab_(914400.0),
				bullet_(), buclr_(), decorate_(0),
				theme_(NULL), style_(), clr_(clr) {
				this->read(root);
			}
			
			virtual ~basic_paragraph() throw() {}
			
			template <class XMLNode>
			basic_paragraph& read(XMLNode* root) {
				if (!root) throw std::runtime_error("cannot find <a:p> tag");
				
				if (!style_.empty()) this->xassign(root->first_node(LITERAL("a:pPr")));
				return this->xread(root);
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
			
			size_type type() const { return type_; }
			const font_type& font() const { return font_; }
			size_type align() const { return align_; }
			size_type valign() const { return valign_; }
			size_type level() const { return level_; }
			double indent() const { return indent_; }
			double baseline() const { return baseline_; }
			double before() const { return before_; }
			double after() const { return after_; }
			size_type butype() const { return butype_; }
			const string_type& bullet() const { return bullet_; }
			size_type buclr() const { return buclr_; }
			double busize() const { return busize_; }
			size_type decorate() const { return decorate_; }
			
			/* ------------------------------------------------------------- */
			//  Access methods (set).
			/* ------------------------------------------------------------- */
			void theme(const theme_type* cp) { theme_ = cp; }
			void align(size_type cp) { align_ = cp; }
			
			template <class Container>
			void style(const Container& cp) {
				style_.assign(cp.begin(), cp.end());
			}
			
		private:
			typedef rapidxml::xml_node<CharT>* node_ptr;
			typedef rapidxml::xml_attribute<CharT>* attr_ptr;
			
			size_type type_;
			container v_;
			font_type font_;
			size_type align_;
			size_type valign_;
			size_type level_;
			double indent_;
			double tab_;
			double baseline_;
			double before_;
			double after_;
			string_type bullet_;
			size_type buclr_;
			double busize_;
			size_type butype_;
			size_type decorate_;
			
			// optional data.
			const theme_type* theme_;
			std::vector<style_type> style_;
			color_map clr_;
			
			/* ------------------------------------------------------------- */
			//  xassign
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_paragraph& xassign(XMLNode* root) {
				if (root) {
					attr_ptr attr = root->first_attribute(LITERAL("lvl"));
					if (attr && attr->value_size() > 0) level_ = clx::lexical_cast<size_type>(attr->value());
				}
				
				if (level_ >= style_.size()) return *this;
				font_ = style_.at(level_).font();
				align_ = style_.at(level_).align();
				valign_ = style_.at(level_).valign();
				indent_ = style_.at(level_).indent();
				butype_ = style_.at(level_).butype();
				bullet_ = style_.at(level_).bullet();
				buclr_ = style_.at(level_).buclr();
				busize_ = style_.at(level_).busize();
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_newline
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_paragraph& xread_newline(XMLNode* root) {
				if (!root) throw std::runtime_error("cannot find <a:br/a:endParaRPr>");
				
				string_type code(LITERAL("\n"));
				font_type f(font_);
				
				if (string_type(root->name()) == LITERAL("a:endParaRPr")) {
					attr_ptr attr = root->first_attribute(LITERAL("sz"));
					if (attr && attr->value_size() > 0) {
						f.size(clx::lexical_cast<double>(attr->value()) / 100.0);
					}
					
					if (v_.empty()) {
						value_type elem(code);
						elem.font(f);
						v_.push_back(elem);
					}
				}
				else {
					if (v_.empty()) {
						value_type elem(code);
						elem.font(f);
						v_.push_back(elem);
					}
					else {
						string_type tmp(v_.back().data());
						tmp += code;
						v_.back().data(tmp);
					}
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_paragraph& xread(XMLNode* root) {
				if (!root) throw std::runtime_error("cannot find <a:p> tag");
				this->xread_style(root->first_node(LITERAL("a:pPr")));
				
				for (node_ptr child = root->first_node(); child; child = child->next_sibling()) {
					if (string_type(child->name()) == LITERAL("a:br") ||
						string_type(child->name()) == LITERAL("a:endParaRPr")) {
						this->xread_newline(child);
						continue;
					}
					if (string_type(child->name()) != LITERAL("a:r")) continue;
					
					value_type elem;
					font_type f(font_);
					size_type deco = 0;
					
					node_ptr pos = child->first_node(LITERAL("a:rPr"));
					if (pos) {
						attr_ptr attr = pos->first_attribute(LITERAL("lang"));
						if (attr && attr->value_size() > 0) {
							string_type tmp(attr->value());
							if (tmp == LITERAL("en-US")) elem.ascii(true);
						}
						
						attr = pos->first_attribute(LITERAL("sz"));
						if (!attr) attr = pos->first_attribute(LITERAL("szCs"));
						if (attr && attr->value_size() > 0) {
							f.size(clx::lexical_cast<double>(attr->value()) / 100.0);
							//font_.size(clx::lexical_cast<double>(attr->value()) / 100.0);
						}
						
						attr = pos->first_attribute(LITERAL("baseline"));
						if (attr && attr->value_size() > 0) elem.baseline(clx::lexical_cast<int>(attr->value()));
						attr = pos->first_attribute(LITERAL("b"));
						if (attr) deco |= 0x01;
						attr = pos->first_attribute(LITERAL("i"));
						if (attr) deco |= 0x02;
						attr = pos->first_attribute(LITERAL("u"));
						if (attr && attr->value_size() > 0) {
							deco |= 0x04;
							if (string_type(attr->value()) == LITERAL("dbl")) deco |= 0x100;
						}
						attr = pos->first_attribute(LITERAL("strike"));
						if (attr && attr->value_size() > 0) {
							deco |= 0x08;
							if (string_type(attr->value()) == LITERAL("dbl")) deco |= 0x100;
						}
						node_ptr tmp = pos->first_node(LITERAL("a:hlinkClick"));
						if (tmp) deco |= 0x10;
						
						tmp = pos->first_node(LITERAL("a:solidFill"));
						if (tmp) {
							if (theme_) f.rgb(getrgb(tmp, theme_->palette(), clr_));
							else f.rgb(getrgb(tmp));
							//if (theme_) font_.rgb(getrgb(tmp, theme_->palette()));
							//else font_.rgb(getrgb(tmp));
						}
						
						// TODO: エンボス以外の効果があるのかを調査．
						tmp = pos->first_node(LITERAL("a:effectDag"));
						if (tmp) deco |= 0x2000;
						
						tmp = pos->first_node(LITERAL("a:latin"));
						if (tmp) {
							attr = tmp->first_attribute(LITERAL("typeface"));
							if (attr && attr->value_size() > 0) f.latin(string_type(attr->value()));
						}
						
						tmp = pos->first_node(LITERAL("a:ea"));
						if (tmp) {
							attr = tmp->first_attribute(LITERAL("typeface"));
							if (attr && attr->value_size() > 0) f.japan(string_type(attr->value()));
						}
					}
					
					elem.font(f);
					elem.decorate(deco);
					
					// get data
					pos = child->first_node(LITERAL("a:t"));
					if (pos && pos->value_size() > 0) {
						elem.data(string_type(pos->value()));
					}
					v_.push_back(elem);
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_paragraph& xread_style(XMLNode* root) {
				if (root) {
					if (root->first_node(LITERAL("a:buNone"))) butype_ = 0;
					
					// align
					attr_ptr attr = root->first_attribute(LITERAL("algn"));
					if (attr && attr->value_size() > 0) align_ = getalign(attr->value());
					
					// indent
					attr = root->first_attribute(LITERAL("indent"));
					if (attr && attr->value_size() > 0) {
						double tmp = clx::lexical_cast<double>(attr->value());
						if (tmp > 0.0) indent_ = tmp;
					}
					
					// default tab size
					attr = root->first_attribute(LITERAL("defTabSz"));
					if (attr && attr->value_size() > 0) tab_ = clx::lexical_cast<double>(attr->value());
					
					// baseline
					node_ptr pos = root->first_node(LITERAL("a:lnSpc"));
					if (pos) pos = pos->first_node(LITERAL("a:spcPct"));
					if (pos) {
						attr = pos->first_attribute(LITERAL("val"));
						if (attr && attr->value_size() > 0) {
							try {
								int per = clx::lexical_cast<int>(attr->value());
								baseline_ = 1.0 + per / (1000.0 * 100.0);
							}
							catch (clx::bad_lexical_cast& e) {
								clx::logger::warn(WARNF("failed to get baseline: %s", attr->value()));
							}
						}
					}
					
					// before spacing
					pos = root->first_node(LITERAL("a:spcBef"));
					if (pos) pos = pos->first_node(LITERAL("a:spcPct"));
					if (pos) {
						attr = pos->first_attribute(LITERAL("val"));
						if (attr && attr->value_size() > 0) {
							try {
								int per = clx::lexical_cast<int>(attr->value());
								before_ = per / (1000.0 * 100.0);
							}
							catch (clx::bad_lexical_cast& e) {
								clx::logger::warn(WARNF("failed to get before spacing: %s", attr->value()));
							}
						}
					}
					
					// after spacing
					pos = root->first_node(LITERAL("a:spcAft"));
					if (pos) pos = pos->first_node(LITERAL("a:spcPct"));
					if (pos) {
						attr = pos->first_attribute(LITERAL("val"));
						if (attr && attr->value_size() > 0) {
							try {
								int per = clx::lexical_cast<int>(attr->value());
								after_ = per / (1000.0 * 100.0);
							}
							catch (clx::bad_lexical_cast& e) {
								clx::logger::warn(WARNF("failed to get after spacing: %s", attr->value()));
							}
						}
					}
				}
				
				return *this;
			}
		};
		
		/* ----------------------------------------------------------------- */
		//  basic_txbox
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_txbox {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef basic_paragraph<CharT, Traits> paragraph;
			typedef std::vector<paragraph> container;
			typedef typename container::value_type value_type;
			typedef typename container::const_iterator const_iterator;
			typedef basic_theme<CharT, Traits> theme_type;
			typedef basic_txstyle<CharT, Traits> style_type;
			typedef std::map<string_type, string_type> color_map;
			
			basic_txbox() :
				v_(), valign_(0), wrap_(true), clr_() {}
			
			template <class XMLNode>
			explicit basic_txbox(XMLNode* root) :
				v_(), valign_(0), wrap_(true),
				theme_(NULL), style_(), clr_() {
				this->read(root);
			}
			
			virtual ~basic_txbox() throw() {}
			
			template <class XMLNode>
			basic_txbox& read(XMLNode* root) {
				if (!root) throw std::runtime_error("cannot find <*:txBody> tag");
				
				this->xread_style(root->first_node(LITERAL("a:bodyPr")));
				for (XMLNode* child = root->first_node(); child; child = child->next_sibling()) {
					if (string_type(child->name()) != LITERAL("a:p")) continue;
					value_type elem(clr_);
					if (theme_) elem.theme(theme_);
					if (!style_.empty()) elem.style(style_);
					elem.read(child);
					v_.push_back(elem);
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			size_type valign() const { return valign_; }
			bool wrap() const { return wrap_; }
			bool empty() const { return v_.empty(); }
			size_type size() const { return v_.size(); }
			const value_type& at(size_type pos) const { return v_.at(pos); }
			const value_type& operator[](size_type pos) const { return v_[pos]; }
			const_iterator begin() const { return v_.begin(); }
			const_iterator end() const { return v_.end(); }
			
			/* ------------------------------------------------------------- */
			//  Access methods (set).
			/* ------------------------------------------------------------- */
			void clear() { v_.clear(); }
			void theme(const theme_type* cp) { theme_ = cp; }
			
			template <class Container>
			void style(const Container& cp) {
				style_.assign(cp.begin(), cp.end());
			}
			
			void clrmap(const color_map& cp) {
				clr_ = cp;
			}
			
		private:
			typedef rapidxml::xml_node<CharT>* node_ptr;
			typedef rapidxml::xml_attribute<CharT>* attr_ptr;
			
			container v_;
			size_type valign_;
			bool wrap_;
			
			const theme_type* theme_;
			std::vector<style_type> style_;
			color_map clr_;
			
			template <class XMLNode>
			basic_txbox& xread_style(XMLNode* root) {
				if (!root) return *this;
				
				// valign
				attr_ptr attr = root->first_attribute(LITERAL("anchor"));
				if (attr && attr->value_size() > 0) {
					valign_ = getvalign(attr->value());
				}
				
				attr = root->first_attribute(LITERAL("wrap"));
				if (attr && attr->value_size() > 0) {
					if (string_type(attr->value()) == LITERAL("none")) wrap_ = false;
				}
				
				return *this;
			}
		};
	}
}

#endif // FAML_OFFICEX_TXBOX_H
