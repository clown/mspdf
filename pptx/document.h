/* ------------------------------------------------------------------------- */
/*
 *  pptx/document.h
 *
 *  Copyright (c) 2009, Four and More. All rights reserved.
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
 *  Last-modified: Fri 12 Jun 2009 19:57:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PPTX_DOCUMENT_H
#define FAML_PPTX_DOCUMENT_H

#include <string>
#include <vector>
#include "clx/unzip.h"
#include "clx/literal.h"
#include "clx/shared_ptr.h"
#include "clx/case_conv.h"
#include "rapidxml/rapidxml.hpp"
#include "../officex/reference.h"
#include "../officex/theme.h"
#include "../officex/unit.h"
#include "../xlsx/chart.h"
#include "slide.h"
#include "master.h"
#include "layout.h"

namespace faml {
	namespace pptx { // Microsoft PowerPoint (OpenXML)
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_document {
		public:
			typedef clx::basic_unzip<CharT, Traits> storage_type;
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef basic_slide<CharT, Traits> value_type;
			typedef std::vector<value_type> container;
			typedef typename container::iterator iterator;
			typedef typename container::const_iterator const_iterator;
			typedef basic_master<CharT, Traits> master_type;
			typedef faml::officex::basic_txstyle<CharT, Traits> txstyle;
			typedef std::vector<txstyle> txstyle_container;
			
			/* ------------------------------------------------------------- */
			//  Types of additional data.
			/* ------------------------------------------------------------- */
			typedef faml::officex::basic_theme<CharT, Traits> theme_type;
			typedef clx::shared_ptr<theme_type> theme_ptr;
			
			explicit basic_document(const string_type& path) :
				v_(), width_(720.0), height_(540.0), in_(), theme_() {
				this->read(path);
			}
			
			virtual ~basic_document() throw() {}
			
			basic_document& read(const string_type& path) {
				if (!in_.open(path)) throw std::runtime_error("cannot find input file");
				
				// 1. theme1.xml
				typename storage_type::iterator pos = in_.find(LITERAL("ppt/theme/theme1.xml"));
				if (pos != in_.end()) theme_ = theme_ptr(new theme_type(*pos));
				
				// 2. presentation.xml
				pos = in_.find(LITERAL("ppt/presentation.xml"));
				if (pos != in_.end()) this->xread_property(*pos);
				
				// 2. master slide.
				pos = in_.find(LITERAL("ppt/slideMasters/slideMaster1.xml"));
				if (pos == in_.end()) throw std::runtime_error("cannot find slideMaster1.xml");
				master_.read(*pos, *this);
				
				// 3. slides.
				pos = in_.find(LITERAL("ppt/slides/_rels/slide1.xml.rels"));
				for (int i = 1; pos != in_.end(); ++i) {
					// parse reference.
					std::map<string_type, string_type> ref;
					faml::officex::read_reference(*pos, ref);
					if (ref.size() < 1) throw std::runtime_error("cannof find slideLayout reference");
					
					// parse style.
					string_type layout;
					typedef typename std::map<string_type, string_type>::iterator iter;
					for (iter it = ref.begin(); it != ref.end(); ++it) {
						if (it->second.compare(0, 11, LITERAL("slideLayout")) == 0) {
							layout = it->second;
							break;
						}
					}
					
					std::stringstream ss;
					ss << LITERAL("ppt/slideLayouts/") << layout;
					pos = in_.find(ss.str());
					layout_type st;
					st.read(*pos, *this);
					
					// parse main data.
					ss.str(LITERAL(""));
					ss << LITERAL("ppt/slides/slide") << i << LITERAL(".xml");
					pos = in_.find(ss.str());
					if (pos == in_.end()) break;
					value_type elem;
					if (theme_) elem.theme(theme_.get());
					elem.master(master_);
					elem.layout(st);
					elem.read(*pos);
					
					for (typename value_type::iterator it = elem.begin(); it != elem.end(); ++it) {
						if (!it->reference().empty()) {
							if (ref.find(it->reference()) == ref.end()) {
								throw std::runtime_error("cannot find reference");
							}
							
							string_type tmp = ref[it->reference()];
							if (tmp.compare(0, 5, LITERAL("chart")) == 0) {
								it->reference(tmp);
								this->xread_chart(tmp, elem, theme_);
								continue;
							}
							
							ss.str(LITERAL(""));
							ss << LITERAL("ppt/media/") << ref[it->reference()];
							it->reference(ss.str());
						}
					}
					v_.push_back(elem);
					
					ss.str(LITERAL(""));
					ss << LITERAL("ppt/slides/_rels/slide") << i + 1 << LITERAL(".xml.rels");
					pos = in_.find(ss.str());
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			double width() const { return width_; }
			double height() const { return height_; }
			bool empty() const { return v_.empty(); }
			size_type size() const { return v_.size(); }
			const value_type& at(size_type pos) const { return v_.at(pos); }
			const value_type& operator[](size_type pos) const { return v_[pos]; }
			const_iterator begin() const { return v_.begin(); }
			const_iterator end() const { return v_.end(); }
			iterator begin() { return v_.begin(); }
			iterator end() { return v_.end(); }
			const master_type& master() const { return master_; }
			theme_ptr& theme() { return theme_; }
			const theme_ptr& theme() const { return theme_; }
			storage_type& storage() { return in_; }
			const txstyle_container& text_style() const { return default_; }
			txstyle_container& text_style() { return default_; }
			
		private:
			typedef basic_layout<CharT, Traits> layout_type;
			typedef rapidxml::xml_node<CharT>* node_ptr;
			typedef rapidxml::xml_attribute<CharT>* attr_ptr;
			typedef faml::xlsx::basic_chart<CharT, Traits> chart_type;
			
			container v_;
			double width_;
			double height_;
			storage_type in_;
			master_type master_;
			theme_ptr theme_;
			txstyle_container default_;
			
			/* ------------------------------------------------------------- */
			//  xread_chart
			/* ------------------------------------------------------------- */
			template <class Type>
			basic_document& xread_chart(const string_type& src, Type& dest, const theme_ptr& theme) {
				std::basic_stringstream<CharT, Traits> ss;
				ss << LITERAL("ppt/charts/") << src;
				typename storage_type::iterator pos = in_.find(ss.str());
				if (pos == in_.end()) throw std::runtime_error("cannot find chart file");
				
				chart_type ch;
				if (theme) ch.theme(theme.get());
				ch.read(*pos);
				dest.charts().push_back(ch);
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_property
			/* ------------------------------------------------------------- */
			template <class Ch, class Tr>
			basic_document& xread_property(std::basic_istream<Ch, Tr>& in) {
				std::vector<CharT> s;
				clx::read(in, s);
				s.push_back(0);
				
				//rapidxml::xml_document<char_type> doc; // <- ??? compile error!!
				rapidxml::xml_document<char> doc;
				doc.parse<0>(reinterpret_cast<char_type*>(&s.at(0)));
				
				node_ptr root = doc.first_node(LITERAL("p:presentation"));
				if (!root) throw std::runtime_error("cannot find <p:presentation>");
				node_ptr pos = root->first_node(LITERAL("p:sldSz"));
				if (pos) {
					attr_ptr attr = pos->first_attribute(LITERAL("cx"));
					if (attr && attr->value_size() > 0) {
						width_ = faml::officex::emu(clx::lexical_cast<double>(attr->value()));
					}
					
					attr = pos->first_attribute(LITERAL("cy"));
					if (attr && attr->value_size() > 0) {
						height_ = faml::officex::emu(clx::lexical_cast<double>(attr->value()));
					}
				}
				
				pos = root->first_node(LITERAL("p:defaultTextStyle"));
				if (pos) this->xread_styles(pos, default_);
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_styles
			/* ------------------------------------------------------------- */
			template <class Container>
			basic_document& xread_styles(node_ptr root, Container& dest) {
				if (!root) std::runtime_error("cannot find <p:defaultTextStyle> tag");
				for (node_ptr child = root->first_node(); child; child = child->next_sibling()) {
					if (string_type(child->name()) == LITERAL("a:defPPr")) continue;
					txstyle elem;
					if (this->theme()) elem.theme(this->theme().get());
					elem.read(child);
					dest.push_back(elem);
				}
				return *this;
			}
		};
	}
}

#endif // FAML_PPTX_DOCUMENT_H
