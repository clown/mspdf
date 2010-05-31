/* ------------------------------------------------------------------------- */
/*
 *  docx/paragraph.h
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
#ifndef FAML_DOCX_PARAGRAPH_H
#define FAML_DOCX_PARAGRAPH_H

#include <stdexcept>
#include <string>
#include <vector>
#include "rapidxml/rapidxml.hpp"
#include "clx/scanner.h"
#include "clx/lexical_cast.h"
#include "clx/literal.h"
#include "clx/utility.h"
#include "../officex/utility.h"
#include "../officex/unit.h"
#include "../officex/color.h"
#include "../officex/font.h"
#include "../officex/theme.h"
#include "../xlsx/chart.h"
#include "shape.h"
#include "image.h"
#include "style.h"
#include "bullet.h"

namespace faml {
	namespace docx {
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
			
			enum {
				none		= 0x0000,
				bold		= 0x0001,
				italic		= 0x0002,
				underline	= 0x0004,
				strike		= 0x0008,
				hyperlink	= 0x0010,
				highlight	= 0x0020,
				ruby		= 0x0040,
				overlap		= 0x0080,
				box			= 0x0100,
				doubled		= 0x1000
			};
			
			basic_piece() :
				data_(), ext_(), font_(), extf_(),
				bg_(0xff000000), decorate_(0), scale_(100),
				space_(0.0), tab_(0.0) {}
			
			basic_piece(const string_type& s) :
				data_(s), ext_(), font_(), extf_(),
				bg_(0xff000000), decorate_(0), scale_(100),
				space_(0.0), tab_(0.0) {}
			
			virtual ~basic_piece() throw() {}
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			const string_type& data() const { return data_; }
			const string_type& ext() const { return ext_; }
			const font_type& font() const { return font_; }
			const font_type& extf() const { return extf_; }
			size_type background() const { return bg_; }
			size_type decorate() const { return decorate_; }
			size_type scale() const { return scale_; }
			double space() const { return space_; }
			double tab() const { return tab_; }
			
			/* ------------------------------------------------------------- */
			//  Access methods (set).
			/* ------------------------------------------------------------- */
			void data(const string_type& cp) { data_ = cp; }
			void ext(const string_type& cp) { ext_ = cp; }
			void font(const font_type& cp) { font_ = cp; }
			void extf(const font_type& cp) { extf_ = cp; }
			void background(size_type cp) { bg_ = cp; }
			void decorate(size_type cp) { decorate_ = cp; }
			void scale(size_type cp) { scale_ = cp; }
			void space(double cp) { space_ = cp; }
			void tab(double cp) { tab_ = cp; }
			
		private:
			string_type data_;
			string_type ext_;
			font_type font_;
			font_type extf_;
			size_type bg_;
			size_type decorate_;
			size_type scale_;
			double space_;
			double tab_;
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
			typedef std::vector<size_type> container;
			typedef typename container::value_type value_type;
			typedef typename container::const_iterator const_iterator;
			typedef basic_piece<CharT, Traits> text_type;
			typedef std::vector<text_type> text_container;
			typedef basic_shape<CharT, Traits> shape_type;
			typedef std::vector<shape_type> shape_container;
			typedef basic_image<CharT, Traits> image_type;
			typedef std::vector<image_type> image_container;
			typedef faml::officex::basic_font<CharT, Traits> font_type;
			typedef faml::officex::basic_theme<CharT, Traits> theme_type;
			typedef basic_style<CharT, Traits> style_type;
			typedef basic_bullet<CharT, Traits> bullet_type;
			typedef faml::xlsx::basic_chart<CharT, Traits> chart_type;
			typedef std::vector<chart_type> chart_container;
			
			enum { text = 0, shape, image, chart };
			enum { none = 0, ul = 1, ol = 2 };
			
			basic_paragraph() :
				v_(), font_(LITERAL("Century"), 10.5, 0),
				align_(0), valign_(0), decorate_(0), scale_(100),
				indent_(0.0), indent1st_(0.0), rindent_(0.0), baseline_(-1.0),
				tpad_(0.0), bpad_(0.0),
				list_(none), level_(0), index_(0), bullet_(), bufmt_(),
				theme_(NULL), style_(), bu_(NULL) {}
			
			template <class XMLNode>
			basic_paragraph(XMLNode* root) :
				v_(), font_(LITERAL("Century"), 10.5, 0),
				align_(0), valign_(0), decorate_(0), scale_(100),
				indent_(0.0), indent1st_(0.0), rindent_(0.0), baseline_(-1.0),
				tpad_(0.0), bpad_(0.0),
				list_(none), level_(0), index_(0), bullet_(), bufmt_(),
				theme_(NULL), style_(), bu_(NULL) {
				this->read(root);
			}
			
			virtual ~basic_paragraph() throw() {}
			
			template <class XMLNode>
			basic_paragraph& read(XMLNode* root) {
				if (!root) throw std::runtime_error("cannot find <a:p> tag");
				//XMLNode* pos = root->first_node(LITERAL("a:endParaRPr"));
				//if (pos) return *this;
				
				this->xread_style(root->first_node(LITERAL("w:pPr")));
				this->xread(root);
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
			
			const font_type& font() const { return font_; }
			size_type align() const { return align_; }
			size_type valign() const { return valign_; }
			size_type decorate() const { return decorate_; }
			size_type scale() const { return scale_; }
			double indent() const { return indent_; }
			double indent1st() const { return indent1st_; }
			double rindent() const { return rindent_; }
			double baseline() const { return baseline_; }
			double top_margin() const { return tpad_; }
			double bottom_margin() const { return bpad_; }
			
			size_type list_type() const { return list_; }
			size_type list_level() const { return level_; }
			size_type list_index() const { return index_; }
			double list_margin() const { return margin_; }
			const string_type& bullet() const { return bullet_; }
			const string_type& bufmt() const { return bufmt_; }
			
			const text_container& texts() const { return text_; }
			text_container& texts() { return text_; }
			const shape_container& drawings() const { return shape_; }
			shape_container& drawings() { return shape_; }
			const image_container& images() const { return image_; }
			image_container& images() { return image_; }
			const chart_container& charts() const { return chart_; }
			chart_container& charts() { return chart_; }
			
			/* ------------------------------------------------------------- */
			//  Access methods (set).
			/* ------------------------------------------------------------- */
			void theme(const theme_type* cp) { theme_ = cp; }
			void style(const style_type& cp) { style_ = cp; }
			void bullets(const bullet_type& cp) { bu_ = &cp; }
			
		private:
			typedef rapidxml::xml_node<CharT>* node_ptr;
			typedef rapidxml::xml_attribute<CharT>* attr_ptr;
			
			container v_;
			font_type font_;
			size_type align_;
			size_type valign_;
			size_type decorate_;
			size_type scale_;
			double indent_;
			double indent1st_;
			double rindent_;
			double baseline_;
			double tpad_;
			double bpad_;
			text_container text_;
			shape_container shape_;
			image_container image_;
			chart_container chart_;
			std::vector<double> tabs_;
			
			// itemize
			size_type list_;
			size_type level_;
			size_type index_;
			string_type bullet_;
			string_type bufmt_;
			double margin_;
			
			// optional data.
			const theme_type* theme_;
			style_type style_;
			const bullet_type* bu_;
			
			basic_paragraph& xassign_style(const string_type& name) {
				typename style_type::pstyle_map::const_iterator pos = style_.paragraph().find(name);
				if (pos != style_.paragraph().end()) {
					align_ = pos->second.align();
					decorate_ = pos->second.decorate();
					scale_ = pos->second.scale();
					indent_ = pos->second.indent();
					indent1st_ = pos->second.indent1st();
					font_.rgb(pos->second.rgb());
					font_.size(pos->second.size());
					font_.latin(pos->second.latin());
					font_.japan(pos->second.japan());
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_style
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_paragraph& xread_style(XMLNode* root) {
				if (!root) return *this;
				
				// 1. assign style.
				node_ptr pos = root->first_node(LITERAL("w:pStyle"));
				if (pos) {
					attr_ptr attr = pos->first_attribute(LITERAL("w:val"));
					if (attr && attr->value_size() > 0) {
						string_type st(attr->value());
						this->xassign_style(st);
					}
				}
				
				// 1. align.
				pos = root->first_node(LITERAL("w:jc"));
				if (pos) {
					attr_ptr attr = pos->first_attribute(LITERAL("w:val"));
					if (attr && attr->value_size() > 0) align_ = faml::officex::getalign(attr->value());
				}
				
				pos = root->first_node(LITERAL("w:rPr"));
				if (pos) {
					node_ptr child = pos->first_node(LITERAL("w:sz"));
					//if (!child) child = pos->first_node(LITERAL("w:szCs"));
					if (child) {
						attr_ptr attr = child->first_attribute(LITERAL("w:val"));
						if (attr && attr->value_size() > 0) {
							font_.size(clx::lexical_cast<double>(attr->value()) / 2.0);
						}
					}
					
					child = pos->first_node(LITERAL("w:rFonts"));
					if (child) {
						attr_ptr attr = child->first_attribute(LITERAL("w:ascii"));
						if (attr && attr->value_size() > 0) font_.latin(string_type(attr->value()));
						attr = child->first_attribute(LITERAL("w:eastAsia"));
						if (attr && attr->value_size() > 0) font_.japan(string_type(attr->value()));
					}
					
					// text decoration.
					child = pos->first_node(LITERAL("w:b"));
					if (child) decorate_ |= 0x01;
					child = pos->first_node(LITERAL("w:i"));
					if (child) decorate_ |= 0x02;
					child = pos->first_node(LITERAL("w:u"));
					if (child) {
						decorate_ |= 0x04;
						attr_ptr attr = child->first_attribute(LITERAL("w:val"));
						if (attr && attr->value_size() > 0 &&
							string_type(attr->value()) == LITERAL("double")) {
							decorate_ |= 0x100;
						}
					}
					
					child = pos->first_node(LITERAL("w:strike"));
					if (child) {
						decorate_ |= 0x08;
						attr_ptr attr = child->first_attribute(LITERAL("w:val"));
						if (attr && attr->value_size() > 0 &&
							string_type(attr->value()) == LITERAL("double")) {
							decorate_ |= 0x100;
						}
					}
					
					child = pos->first_node(LITERAL("w:solidFill"));
					if (child) {
						if (theme_) font_.rgb(faml::officex::getrgb(pos, theme_->palette()));
						else font_.rgb(faml::officex::getrgb(pos));
					}
				}
				
				// list
				pos = root->first_node(LITERAL("w:numPr"));
				if (pos) {
					if (!bu_) throw std::runtime_error("cannot find bullet list");
					list_ = ol;
					node_ptr tmp = pos->first_node(LITERAL("w:ilvl"));
					if (tmp) {
						attr_ptr attr = tmp->first_attribute(LITERAL("w:val"));
						if (attr && attr->value_size() > 0) {
							level_ = clx::lexical_cast<size_type>(attr->value());
						}
					}
					
					tmp = pos->first_node(LITERAL("w:numId"));
					if (!tmp) throw std::runtime_error("cannot find <w:numId> tag");
					attr_ptr attr = tmp->first_attribute(LITERAL("w:val"));
					if (!attr && attr->value_size() == 0) throw std::runtime_error("cannot find w:val");
					size_type ref = clx::lexical_cast<size_type>(attr->value());
					typename bullet_type::const_iterator it = bu_->find(ref);
					if (it == bu_->end()) throw std::runtime_error("cannot find target bullet list");
					list_ = it->second.at(level_).type;
					bullet_ = it->second.at(level_).mark;
					bufmt_ = it->second.at(level_).format;
					indent_ = it->second.at(level_).indent;
					margin_ = it->second.at(level_).margin;
				}
				
				// 2. indent
				pos = root->first_node(LITERAL("w:tabs"));
				if (pos) {
					for (node_ptr child = pos->first_node(); child; child = child->next_sibling()) {
						if (string_type(child->name()) != LITERAL("w:tab")) continue;
						attr_ptr attr = child->first_attribute(LITERAL("w:pos"));
						if (attr && attr->value_size() > 0) {
							tabs_.push_back(clx::lexical_cast<double>(attr->value()));
						}
					}
				}
				
				pos = root->first_node(LITERAL("w:ind"));
				if (pos && bullet_.empty()) {
					attr_ptr attr = pos->first_attribute(LITERAL("w:firstLine"));
					if (attr && attr->value_size() > 0) indent1st_ += clx::lexical_cast<double>(attr->value());
					attr = pos->first_attribute(LITERAL("w:left"));
					if (attr && attr->value_size() > 0) indent_ = clx::lexical_cast<double>(attr->value());
					attr = pos->first_attribute(LITERAL("w:right"));
					if (attr && attr->value_size() > 0) rindent_ = clx::lexical_cast<double>(attr->value());
					attr = pos->first_attribute(LITERAL("w:hanging"));
					if (attr && attr->value_size() > 0) indent1st_ -= clx::lexical_cast<double>(attr->value());
				}
				
				// 3. baseline
				pos = root->first_node(LITERAL("w:spacing"));
				if (pos) {
					attr_ptr attr = pos->first_attribute(LITERAL("w:line"));
					if (attr && attr->value_size() > 0) {
						// Todo: w:spacing ÇÃílÇ™ïâÇÃèÍçáÇ…ÅCÇ«Ç§èàóùÇ∑ÇÈÇÃÇ©ÅD
						int sp = clx::lexical_cast<int>(attr->value());
						//if (sp > 0) baseline_ = sp / 20.0 / font_.size();
						if (sp > 0) baseline_ = sp / 20.0;
					}
					
					attr = pos->first_attribute(LITERAL("w:before"));
					if (attr && attr->value_size() > 0) {
						tpad_ = clx::lexical_cast<double>(attr->value()) / 20.0;
					}
					
					attr = pos->first_attribute(LITERAL("w:after"));
					if (attr && attr->value_size() > 0) {
						bpad_ = clx::lexical_cast<double>(attr->value()) / 20.0;
					}
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_paragraph& xread(XMLNode* root) {
				if (!root) throw std::runtime_error("cannot find <w:p> tag");
				
				for (node_ptr child = root->first_node(); child; child = child->next_sibling()) {
					size_type deco = 0;
					node_ptr it = NULL;
					// hyper link 1.
					if (string_type(child->name()) == LITERAL("w:hyperlink")) {
						deco |= text_type::hyperlink;
						it = child->first_node(LITERAL("w:r"));
					}
					else if (string_type(child->name()) == LITERAL("w:r")) it = child;
					else continue;
					
					// newpage 1
					node_ptr pos = it->first_node(LITERAL("w:lastRenderedPageBreak"));
					if (pos) {
						if (text_.empty()) {
							text_type elem(LITERAL("\f"));
							elem.font(font_);
							text_.push_back(elem);
							v_.push_back(0);
						}
						else {
							string_type tmp(text_.back().data());
							tmp += LITERAL("\f");
							text_.back().data(tmp);
						}
					}
					
					// shape and image 2003.
					bool is_shape = false;
					pos = it->first_node(LITERAL("w:pict"));
					for (; pos; pos = pos->next_sibling()) {
						if (string_type(pos->name()) != LITERAL("w:pict")) continue;
						
						is_shape = true;
						node_ptr p1 = pos->first_node(LITERAL("v:shape"));
						if (p1) {
							node_ptr p2 = p1->first_node(LITERAL("v:imageData"));
							if (!p2) p1->first_node(LITERAL("v:imagedata"));
							if (p2) {
								this->xread_image2003(pos);
								continue;
							}
						}
						
						try {
							shape_type elem;
							elem.style(style_);
							if (bu_) elem.bullets(*bu_);
							elem.read(pos);
							shape_.push_back(elem);
							v_.push_back(1);
						}
						catch (std::runtime_error& e) {
							clx::logger::warn(WARNF("Failed to parse a shape: %s", e.what()));
							// ignore the unknown shape type.
						}
					}
					if (is_shape) continue;
					
					// chart and image 2007.
					pos = it->first_node(LITERAL("w:drawing"));
					if (pos) {
						image_type elem;
						node_ptr p1 = pos->first_node();
						node_ptr p2 = p1->first_node(LITERAL("a:graphic"));
						if (p2) {
							node_ptr p3 = p2->first_node(LITERAL("a:graphicData"));
							if (p3) {
								node_ptr p4 = p3->first_node(LITERAL("c:chart"));
								if (p4) {
									this->xread_chart(pos, elem);
									image_.push_back(elem);
									v_.push_back(3);
									continue;
								}
							}
						}
						
						elem.read(pos);
						image_.push_back(elem);
						v_.push_back(2);
						continue;
					}
					
					// text style.
					text_type elem;
					font_type f(font_);
					elem.decorate(deco);
					elem.scale(scale_);
					pos = it->first_node(LITERAL("w:rPr"));
					if (pos) this->xread_rstyle(pos, elem, f);
					
					// instrText.
					bool valid = false;
					deco = elem.decorate();
					font_type extf(f);
					pos = it->first_node(LITERAL("w:instrText"));
					if (pos && pos->value_size() > 0) {
						string_type descr(pos->value());
						child = child->next_sibling();
						while (child) {
							pos = child->first_node(LITERAL("w:fldChar"));
							if (pos) {
								attr_ptr attr = pos->first_attribute(LITERAL("w:fldCharType"));
								if (attr && attr->value_size() > 0 && string_type(attr->value()) == LITERAL("end")) break;
							}
							
							pos = child->first_node(LITERAL("w:rPr"));
							if (pos) {
								elem.decorate(deco);
								this->xread_rstyle(pos, elem, extf);
								deco = elem.decorate();
							}
							
							pos = child->first_node(LITERAL("w:instrText"));
							if (pos && pos->value_size() > 0) descr += string_type(pos->value());
							child = child->next_sibling();
						}
						
						string_type dat;
						string_type ext;
						if (descr.find(LITERAL("HYPERLINK")) != string_type::npos) {
							clx::basic_scanner<CharT>(descr, LITERAL("%sHYPERLINK \"%s\" %s"))(1, dat)(2, ext);
							if (dat.compare(0, 7, LITERAL("mailto:")) == 0) dat.erase(0, 7);
							elem.data(dat);
							elem.ext(ext);
							f = extf;
							valid = true;
							deco |= text_type::hyperlink;
						}
						else if (descr.find(LITERAL("\\o\\ad")) != string_type::npos) {
							clx::basic_scanner<CharT>(descr, LITERAL("%s\\o\\ad(%s(%s),%s)"))(3, dat)(2, ext);
							if (descr.find(LITERAL("hps")) != string_type::npos) {
								try {
									size_type sz;
									clx::basic_scanner<CharT>(descr, LITERAL("%shps%s %s"))(1, sz);
									extf.size(sz / 2.0);
								}
								catch (...) {}
							}
							elem.data(dat);
							elem.ext(ext);
							elem.extf(extf);
							valid = true;
							deco |= text_type::ruby;
						}
						else if (descr.find(LITERAL("\\o\\ac")) != string_type::npos) {
							clx::basic_scanner<CharT>(descr, LITERAL("%s\\o\\ac(%s,%s)"))(2, dat)(1, ext);
							elem.data(dat);
							elem.ext(ext);
							elem.extf(extf);
							valid = true;
							deco |= text_type::overlap;
						}
						else {
							clx::logger::warn(WARNF("Unknown instrText: %s", descr.c_str()));
							continue;
						}
					}
					
					elem.decorate(deco);
					elem.font(f);
					
					// text data.
					size_type idx = 0;
					for (pos = it->first_node(); pos; pos = pos->next_sibling()) {
						if (string_type(pos->name()) == LITERAL("w:t") && pos->value_size() > 0) {
							if (!elem.data().empty()) elem.data(elem.data() + LITERAL("  "));
							elem.data(elem.data() + string_type(pos->value()));
							valid = true;
						}
						else if (string_type(pos->name()) == LITERAL("w:tab")) {
							if (elem.data().empty()) {
								if (idx < tabs_.size()) elem.tab(tabs_.at(idx));
								else elem.tab(840 * (idx + 1));
								++idx;
							}
							valid = true;
						}
						else if (string_type(pos->name()) == LITERAL("w:br")) {
							string_type code(LITERAL("\n"));
							attr_ptr attr = pos->first_attribute(LITERAL("w:type"));
							if (attr && attr->value_size() > 0) {
								if (string_type(attr->value()) == LITERAL("page")) code = LITERAL("\f");
							}
							
							if (elem.data().empty()) elem.data(code);
							else elem.data(elem.data() + code);
							text_.push_back(elem);
							v_.push_back(0);
							elem.data(string_type());
							valid = false;
						}
						else continue;
					}
					
					if (valid) {
						text_.push_back(elem);
						v_.push_back(0);
					}
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_rstyle
			/* ------------------------------------------------------------- */
			template <class XMLNode, class Type, class FontT>
			basic_paragraph& xread_rstyle(XMLNode* root, Type& dest, FontT& f) {
				if (!root) return *this;
				
				node_ptr pos = root->first_node(LITERAL("w:sz"));
				if (!pos) pos = root->first_node(LITERAL("w:szCs"));
				if (pos) {
					attr_ptr attr = pos->first_attribute(LITERAL("w:val"));
					if (attr && attr->value_size()) {
						f.size(clx::lexical_cast<double>(attr->value()) / 2.0);
					}
				}
				
				pos = root->first_node(LITERAL("w:rFonts"));
				if (pos) {
					attr_ptr attr = pos->first_attribute(LITERAL("w:ascii"));
					if (attr && attr->value_size() > 0) f.latin(string_type(attr->value()));
					attr = pos->first_attribute(LITERAL("w:eastAsia"));
					if (attr && attr->value_size() > 0) f.japan(string_type(attr->value()));
				}
				
				// text decoration.
				size_type deco = dest.decorate();
				pos = root->first_node(LITERAL("w:b"));
				if (pos) deco |= text_type::bold;
				pos = root->first_node(LITERAL("w:i"));
				if (pos) deco |= text_type::italic;
				pos = root->first_node(LITERAL("w:u"));
				if (pos) {
					deco |= text_type::underline;
					attr_ptr attr = pos->first_attribute(LITERAL("w:val"));
					if (attr && attr->value_size() > 0 &&
						string_type(attr->value()) == LITERAL("double")) {
						deco |= text_type::doubled;
					}
				}
				pos = root->first_node(LITERAL("w:strike"));
				if (pos) {
					deco |= text_type::strike;
					attr_ptr attr = pos->first_attribute(LITERAL("w:val"));
					if (attr && attr->value_size() > 0 &&
						string_type(attr->value()) == LITERAL("double")) {
						deco |= text_type::doubled;
					}
				}
				
				pos = root->first_node(LITERAL("w:bdr"));
				if (pos) {
					deco |= text_type::box;
					if (tpad_ < 2.0) tpad_ += 2.0;
					if (bpad_ < 2.0) bpad_ += 2.0;
				}
				
				pos = root->first_node(LITERAL("w:shd"));
				if (pos) {
					deco |= text_type::highlight;
					dest.background(0xdadada);
					if (tpad_ < 2.0) tpad_ += 2.0;
					if (bpad_ < 2.0) bpad_ += 2.0;
				}
				
				pos = root->first_node(LITERAL("w:highlight"));
				if (pos) {
					deco |= text_type::highlight;
					attr_ptr attr = pos->first_attribute(LITERAL("w:val"));
					if (attr && attr->value_size() > 0) {
						dest.background(faml::officex::getrgb(string_type(attr->value())));
						if (tpad_ < 2.0) tpad_ += 2.0;
						if (bpad_ < 2.0) bpad_ += 2.0;
					}
				}
				
				// font color
				pos = root->first_node(LITERAL("w:color"));
				if (pos) {
					attr_ptr attr = pos->first_attribute(LITERAL("w:val"));
					if (attr && attr->value_size()) {
						f.rgb(clx::lexical_cast<size_type>(attr->value(), std::ios::hex));
					}
				}
				
				pos = root->first_node(LITERAL("w:w"));
				if (pos) {
					attr_ptr attr = pos->first_attribute(LITERAL("w:val"));
					if (attr && attr->value_size() > 0) {
						dest.scale(clx::lexical_cast<size_type>(attr->value()));
					}
				}
				
				pos = root->first_node(LITERAL("w:spacing"));
				if (pos) {
					attr_ptr attr = pos->first_attribute(LITERAL("w:val"));
					if (attr && attr->value_size() > 0) {
						int sp = clx::lexical_cast<int>(attr->value());
						if (sp > 0) dest.space(sp);
					}
				}
				
				dest.decorate(deco);
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_chart
			/* ------------------------------------------------------------- */
			template <class XMLNode, class Type>
			basic_paragraph& xread_chart(XMLNode* root, Type& dest) {
				if (!root) return *this;
				
				node_ptr parent = root->first_node();
				node_ptr pos = parent->first_node(LITERAL("wp:extent"));
				if (!pos) throw std::runtime_error("cannot find <wp:extent>");
				attr_ptr attr = pos->first_attribute(LITERAL("cx"));
				if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find <wp:extent x>");
				dest.width(clx::lexical_cast<double>(attr->value()));
				attr = pos->first_attribute(LITERAL("cy"));
				if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find <wp:extent y>");
				dest.height(clx::lexical_cast<double>(attr->value()));
				
				node_ptr p1 = parent->first_node(LITERAL("a:graphic"));
				if (!p1) throw std::runtime_error("cannot find <a:graphic>");
				node_ptr p2 = p1->first_node(LITERAL("a:graphicData"));
				if (!p2) throw std::runtime_error("cannot find <a:graphicData>");
				pos = p2->first_node(LITERAL("c:chart"));
				if (!pos) throw std::runtime_error("cannot find <c:chart>");
				attr = pos->first_attribute(LITERAL("r:id"));
				if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find <c:chart r:id>");
				dest.reference(string_type(attr->value()));
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_image2003
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_paragraph& xread_image2003(XMLNode* root) {
				if (!root) std::runtime_error("cannot find <w:pict> tag");
				
				node_ptr pos = root->first_node(LITERAL("v:shape"));
				if (!pos) throw std::runtime_error("cannot find <v:shape> tag");
				attr_ptr attr = pos->first_attribute(LITERAL("style"));
				if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find style attribute");
				
				// origin, width, and height
				double x = 0.0;
				double y = 0.0;
				double w = 0.0;
				double h = 0.0;
				
				string_type s = string_type(attr->value());
				std::vector<string_type> v;
				clx::split_if(s, v, clx::is_any_of(LITERAL(";:")));
				for (size_type i = 0; i < v.size(); ++i) {
					if (i + 1 >= v.size()) break;
					double val = 0.0;
					if (v.at(i) == LITERAL("margin-left") ||
						v.at(i) == LITERAL("margin-top") ||
						v.at(i) == LITERAL("width") ||
						v.at(i) == LITERAL("height")) {
						if (v.at(i + 1).find(LITERAL("pt")) != string_type::npos) {
							val = clx::lexical_cast<double>(v.at(i + 1).substr(
								0, v.at(i + 1).find(LITERAL("pt"))
							));
						}
						else if (v.at(i + 1).find(LITERAL("in")) != string_type::npos) {
							double tmp = clx::lexical_cast<double>(v.at(i + 1).substr(
								0, v.at(i + 1).find(LITERAL("in"))
							));
							val = faml::officex::inch(tmp);
						}
						else throw std::runtime_error("emerged unexpected unit");
					}
					
					if (v.at(i) == LITERAL("margin-left")) x = val;
					else if (v.at(i) == LITERAL("margin-top")) y = val;
					else if (v.at(i) == LITERAL("width")) w = val;
					else if (v.at(i) == LITERAL("height")) h = val;
				}
				
				image_type elem;
				elem.offset(coordinate(x, y));
				elem.width(w);
				elem.height(h);
				
				// reference
				node_ptr child = pos->first_node(LITERAL("v:imageData"));
				if (!child) child = pos->first_node(LITERAL("v:imagedata"));
				if (!child) throw std::runtime_error("cannot find <v:imageData> tag");
				attr = child->first_attribute(LITERAL("r:id"));
				if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find r:id attribute");
				elem.reference(string_type(attr->value()));
				
				image_.push_back(elem);
				v_.push_back(2);
				return *this;
			}
		};
	}
}

#endif // FAML_DOCX_PARAGRAPH_H
