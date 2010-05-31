/* ------------------------------------------------------------------------- */
/*
 *  xlsx/document.h
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
 *  Last-modified: Mon 08 Jun 2009 01:57:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_XLSX_DOCUMENT_H
#define FAML_XLSX_DOCUMENT_H

#include <string>
#include <vector>
#include "clx/unzip.h"
#include "clx/literal.h"
#include "clx/shared_ptr.h"
#include "rapidxml/rapidxml.hpp"
#include "../officex/theme.h"
#include "../officex/reference.h"
#include "worksheet.h"
#include "style.h"
#include "shared_string.h"
#include "chart.h"

namespace faml {
	namespace xlsx { // Microsoft Excel (OpenXML)
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
			typedef clx::basic_unzip<CharT, Traits> storage_type;
			typedef basic_worksheet<CharT, Traits> worksheet;
			typedef std::vector<worksheet> container;
			typedef typename container::value_type value_type;
			typedef typename container::const_iterator const_iterator;
			typedef basic_chart<CharT, Traits> chart_type;
			
			/* ------------------------------------------------------------- */
			//  Types of additional data.
			/* ------------------------------------------------------------- */
			typedef faml::officex::basic_theme<CharT, Traits> theme_type;
			typedef clx::shared_ptr<theme_type> theme_ptr;
			typedef basic_style_container<CharT, Traits> style_container;
			typedef typename style_container::value_type style_type;
			typedef basic_shared_string<CharT, Traits> shared_string;
			
			explicit basic_document(const string_type& path) :
				in_(), v_(), styles_(), sst_(), theme_() {
				this->read(path);
			}
			
			virtual ~basic_document() throw() {}
			
			basic_document& read(const string_type& path) {
				if (!in_.open(path)) throw std::runtime_error("cannot find input file");
				
				// 1. theme1.xml
				typename storage_type::iterator pos = in_.find(LITERAL("xl/theme/theme1.xml"));
				if (pos != in_.end()) theme_ = theme_ptr(new theme_type(*pos));
				
				// 2. sharedStrings.xml
				pos = in_.find(LITERAL("xl/sharedStrings.xml"));
				if (pos == in_.end()) throw std::runtime_error("cannot fine xl/sharedStrings.xml");
				sst_.read(*pos);
				
				// 3. worksheets and drawings
				pos = in_.find(LITERAL("xl/worksheets/sheet1.xml"));
				size_type i = 1;
				while (pos != in_.end()) {
					value_type elem(*pos);
					
					std::basic_stringstream<CharT, Traits> ss;
					if (!elem.reference().empty()) {
						ss << LITERAL("xl/worksheets/_rels/sheet") << i << LITERAL(".xml.rels");
						pos = in_.find(ss.str());
						
						string_type draw;
						if (pos != in_.end()) {
							std::map<string_type, string_type> ref;
							faml::officex::read_reference(*pos, ref);
							
							typedef typename std::map<string_type, string_type>::iterator iter;
							for (iter it = ref.begin(); it != ref.end(); ++it) {
								if (it->first == elem.reference()) {
									draw = it->second;
									break;
								}
							}
						}
						
						if (!draw.empty()) {
							ss.str(LITERAL(""));
							ss << LITERAL("xl/drawings/") << draw;
							pos = in_.find(ss.str());
							if (pos != in_.end()) {
								elem.drawings().widths(elem.widths());
								elem.drawings().heights(elem.heights());
								elem.drawings().read(*pos, *this);
								
								ss.str(LITERAL(""));
								
								for (size_type i = 0; i < elem.drawings().size(); ++i) {
									if (elem.drawings().at(i).reference().empty()) continue;
									string_type ref = elem.drawings().at(i).reference();
									if (ref.compare(0, 5, LITERAL("chart")) == 0) {
										ss.str(LITERAL(""));
										ss << LITERAL("xl/charts/") << ref;
										pos = in_.find(ss.str());
										if (pos == in_.end()) throw std::runtime_error("cannot find chart.xml");
										
										try {
											chart_type ch;
											if (theme_) ch.theme(theme_.get());
											ch.reference(elem.data());
											ch.sst(sst_);
											ch.read(*pos);
											elem.charts().push_back(ch);
										}
										catch (std::runtime_error&) {
											elem.drawings().at(i).name(LITERAL("invalid"));
											elem.drawings().at(i).reference(LITERAL(""));
											elem.drawings().at(i).width(0.0);
											elem.drawings().at(i).height(0.0);
										}
									}
								}
							}
						}
					}
					
					v_.push_back(elem);
					
					++i;
					ss.str(LITERAL(""));
					ss << LITERAL("xl/worksheets/sheet") << i << LITERAL(".xml");
					pos = in_.find(ss.str());
				}
				
				// 4. styles.xml
				pos = in_.find(LITERAL("xl/styles.xml"));
				if (pos == in_.end()) throw std::runtime_error("cannot fine xl/styles.xml");
				if (theme_) styles_.theme(theme_.get());
				styles_.read(*pos);
				
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
			storage_type& storage() { return in_; }
			
			/* ------------------------------------------------------------- */
			//  Additional data.
			/* ------------------------------------------------------------- */
			const style_container& styles() const { return styles_; }
			const shared_string& sst() const { return sst_; }
			const theme_ptr& theme() const { return theme_; }
			
		private:
			typedef rapidxml::xml_node<CharT>* node_ptr;
			typedef rapidxml::xml_attribute<CharT>* attr_ptr;
			
			storage_type in_;
			container v_;
			style_container styles_;
			shared_string sst_;
			theme_ptr theme_;
		};
	}
}

#endif // FAML_XLSX_DOCUMENT_H
