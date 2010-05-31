/* ------------------------------------------------------------------------- */
/*
 *  xlsx.h
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
 *  Last-modified: Sat 18 Apr 2009 01:25:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PDF_XLSX_H
#define FAML_PDF_XLSX_H

#include <algorithm>
#include <ios>
#include <istream>
#include <sstream>
#include <iterator>
#include <string>
#include <set>
#include <map>
#include <vector>

#include "babel/babel.h"
#include "clx/lexical_cast.h"
#include "clx/literal.h"
#include "clx/format.h"
#include "clx/shared_ptr.h"
#include "clx/tokenizer.h"

#include "unit.h"
#include "code_convert.h"
#include "page.h"
#include "piece.h"
#include "font.h"
#include "font_instance.h"
#include "text_helper.h"

#include "contents/shape_factory.h"
#include "contents/custom_shape.h"
#include "contents/barchart.h"
#include "contents/bar3dchart_box.h"
#include "contents/bar3dchart_cylinder.h"
#include "contents/bar3dchart_cone.h"
#include "contents/bar3dchart_pyramid.h"
#include "contents/rowchart.h"
#include "contents/row3dchart_box.h"
#include "contents/linechart.h"
#include "contents/piechart.h"
#include "contents/pie3dchart.h"
#include "contents/pie3dchart_explosion.h"
#include "contents/areachart.h"
#include "contents/scatterchart.h"
#include "contents/table.h"
#include "text_contents.h"
#include "image_contents.h"

#include "xlsx/document.h"
#include "xlsx/format.h"

#define XLSX_DEFAULT_LATIN "Arial"
#define XLSX_DEFAULT_JAPAN "ÇlÇr ÉSÉVÉbÉN"
#define XLSX_FONT_SCALE 1.0
#define XLSX_CELL_WIDTH 6.0

namespace faml {
	namespace pdf {
		/* ----------------------------------------------------------------- */
		//  basic_xlsx
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_xlsx {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			
			typedef basic_page<CharT, Traits> page_object;
			typedef basic_font<CharT, Traits> font_object;
			typedef std::map<string_type, font_object> font_map;
			
			basic_xlsx(const string_type& path) :
				doc_(path), in_(), pp_(a4::width, a4::height), f_(),
				space_(3.6), vspace_(0.0), baseline_(1.2) {
				this->xinit(path);
			}
			
			template <class OutStream, class PDFManager>
			bool operator()(OutStream& out, PDFManager& pm) {
				// currently implementation
				for (typename font_map::iterator pos = f_.begin(); pos != f_.end(); ++pos) {
					(pos->second)(out, pm);
				}
				
				for (size_type i = 0; i < doc_.size(); ++i) {
					if (doc_.at(i).empty()) continue; // empty worksheet.
					this->xadjust_size(doc_.at(i));
					page_object newpage(pp_);
					
					if (!doc_.at(i).drawings().empty()) this->xobjects(out, pm, doc_.at(i).drawings());
					
					int idx = 0;
					idx = this->xcontents(out, pm, doc_.at(i));
					if (idx < 0) throw std::runtime_error("something was happened in the contents object");
					clx::logger::debug(DEBUGF("add width: %f", cur_.right));
					if (cur_.right > 0) {
						pp_.width(pp_.width() + pp_.margin().right() + cur_.right);
						newpage.property(pp_);
					}
					newpage.contents(idx);
					
					if (!cur_.fonts.empty()) this->xput_font_objects(out, pm);
					idx = this->xresources(out, pm);
					if (idx < 0) throw std::runtime_error("something was happened in the contents object");
					newpage.resources(idx);
					
					if (!newpage(out, pm)) return false;
				}
				
				return true;
			}
			
		private:
			typedef clx::basic_format<CharT, Traits> fmt;
			typedef clx::basic_unzip<CharT, Traits> storage_type;
			typedef faml::xlsx::basic_document<CharT, Traits> doc_type;
			typedef basic_font_property<CharT, Traits> font_property;
			typedef basic_extpiece<CharT, Traits> piece_type;
			
			// contents
			typedef basic_image_contents<CharT, Traits> image_contents;
			typedef basic_text_contents<CharT, Traits> text_contents;
			typedef contents::basic_table<CharT, Traits> table_contents;
			typedef contents::basic_shape<CharT, Traits> shape_contents;
			typedef clx::shared_ptr<shape_contents> shape_ptr;
			
			// member variables
			doc_type doc_;
			storage_type in_;
			page_property pp_;
			font_map f_;
			double space_;
			double vspace_;
			double baseline_;
			
			/* ------------------------------------------------------------- */
			//  xobject
			/* ------------------------------------------------------------- */
			struct xobj_type {
				int index;
				string_type label;
				string_type path;
			};
			std::map<string_type, xobj_type> objs_;
			
			/* ------------------------------------------------------------- */
			/*
			 *  current_status
			 *
			 *  The current_status structure keeps some information needed
			 *  for sub-records. When the parent class finished parsing,
			 *  values of the structure member were nonsense. Members of
			 *  the private structure depend on its parent class.
			 */
			/* ------------------------------------------------------------- */
			struct current_status {
				double x;
				double y;
				double width;
				double height;
				double right;
				std::set<string_type> fused; // used font list.
				font_map fonts;
				std::deque<xobj_type> objs;
				
				current_status() :
					x(0), y(0), width(0), height(0), right(0),
					fused(), fonts(), objs() {}
			};
			current_status cur_;
			
			/* ------------------------------------------------------------- */
			//  xinit
			/* ------------------------------------------------------------- */
			void xinit(const string_type& path) {
				pp_.margin(page_margin(30));
				string_type latin(LITERAL(XLSX_DEFAULT_LATIN));
				string_type japan(LITERAL(XLSX_DEFAULT_JAPAN));
				
				font_object f(latin, charset::utf16);
				font_object fj(japan, charset::utf16);
				
				f_[latin] = f;
				f_[japan] = fj;
				cur_.fused.insert(latin);
				cur_.fused.insert(japan);
				
				if (!in_.open(path)) throw std::runtime_error("cannot find xlsx file");
			}
			
			/* ------------------------------------------------------------- */
			/*
			 *  xresources
			 *
			 *  Output some information about resources object (e.g.,
			 *  some reference information (font, image resource, ...)).
			 */
			/* ------------------------------------------------------------- */
			template <class OutStream, class PDFManager>
			int xresources(OutStream& out, PDFManager& pm) {
				int index = static_cast<int>(pm.newindex());
				
				string_type proc = LITERAL("/PDF /Text");
				out << fmt(LITERAL("%d 0 obj")) % index << std::endl;
				out << LITERAL("<<") << std::endl;
				//out << LITERAL("/ProcSet [ /PDF /Text ]") << std::endl;
				
				// output font references used in the current page.
				out << LITERAL("/Font") << std::endl;
				out << LITERAL("<<") << std::endl;
				for (typename std::set<string_type>::const_iterator it = cur_.fused.begin();
					it != cur_.fused.end(); ++it) {
					typename font_map::const_iterator pos = f_.find(*it);
					if (pos == f_.end()) return -1;
					out << fmt(LITERAL("/%s %d 0 R"))
						% pos->second.label() % pos->second.index() << std::endl;
				}
				out << LITERAL(">>") << std::endl;
				
				// xobject reference
				if (!cur_.objs.empty()) {
					proc += LITERAL(" /ImageC");
					out << LITERAL("/XObject") << std::endl;
					out << LITERAL("<<") << std::endl;
					for (size_type i = 0; i < cur_.objs.size(); ++i) {
						out << fmt(LITERAL("/%s %d 0 R"))
							% cur_.objs.at(i).label % cur_.objs.at(i).index
						<< std::endl;
					}
					out << LITERAL(">>") << std::endl;
					cur_.objs.clear();
				}
				
				out << fmt(LITERAL("/ProcSet [ %s ]")) % proc << std::endl;
				out << LITERAL(">>") << std::endl;
				out << LITERAL("endobj") << std::endl;
				out << std::endl;
				
				return index;
			}
			
			/* ------------------------------------------------------------- */
			//  xobjects
			/* ------------------------------------------------------------- */
			template <class OutStream, class PDFManager, class Source>
			bool xobjects(OutStream& out, PDFManager& pm, const Source& src) {
				for (size_type i = 0; i < src.size(); ++i) {
					string_type ref = src.at(i).reference();
					if (ref.empty() || ref.compare(0, 5, LITERAL("chart")) == 0) continue;
					string_type ext = ref.substr(ref.find_last_of(LITERAL(".")));
					if (ext == LITERAL(".wmf") || ext == LITERAL(".emf")) continue;
					if (objs_.find(ref) != objs_.end()) continue;
					
					typename storage_type::iterator pos = in_.find(ref);
					if (pos == in_.end()) throw std::runtime_error("cannot find image file");
					std::vector<char_type> s;
					clx::read(*pos, s);
					clx::basic_ivstream<CharT, Traits> vs(s);
					
					xobj_type elem;
					elem.path = pos->path();
					if (ext == LITERAL(".jpeg") || ext == LITERAL(".jpg")) {
						jpeg_resource<CharT, Traits> jpeg(vs);
						if (!jpeg(out, pm)) throw std::runtime_error("error is occured in jpeg resource.");
						elem.index = jpeg.index();
						elem.label = jpeg.label();
					}
					else if (ext == LITERAL(".png")) {
						png_resource<CharT, Traits> png(vs);
						if (!png(out, pm)) throw std::runtime_error("error is occured in gif resource.");
						elem.index = png.index();
						elem.label = png.label();
					}
					else continue;
					
					objs_[ref] = elem;
				}
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xput_font_objects
			/* ------------------------------------------------------------- */
			template <class OutStream, class PDFManager>
			bool xput_font_objects(OutStream& out, PDFManager& pm) {
				for (typename font_map::iterator pos = cur_.fonts.begin();
					pos != cur_.fonts.end(); ++pos) {
					(pos->second)(out, pm);
					f_[pos->first] = pos->second;
				}
				cur_.fonts.clear();
				return true;
			}
			
			/* ------------------------------------------------------------- */
			/*
			 *  xcontents
			 *
			 *  Output some information about contents object (e.g.,
			 *  stream length, filter information, and contents stream).
			 */
			/* ------------------------------------------------------------- */
			template <class OutStream, class PDFManager, class WorkSheet>
			int xcontents(OutStream& out, PDFManager& pm, const WorkSheet& src) {
				//cur_.fused.clear();
				std::basic_stringstream<CharT, Traits> ss;
				string_type filter;
#ifdef FAML_USE_ZLIB
				filter = LITERAL("/FlateDecode");
				clx::basic_zstream<Z_DEFAULT_COMPRESSION, CharT, Traits> z(ss);
				if (!this->xputstream(ss, src)) return -1;
				z.finish();
#else
				if (!this->xputstream(ss, src)) return -1;
#endif
				int index = static_cast<int>(pm.newindex());
				out << fmt(LITERAL("%d 0 obj")) % index << std::endl;
				out << LITERAL("<<") << std::endl;
				out << fmt(LITERAL("/Length %d")) % ss.str().size() << std::endl;
				out << fmt(LITERAL("/Filter [ %s ]")) % filter << std::endl;
				out << LITERAL(">>") << std::endl;
				out << LITERAL("stream") << std::endl;
				out << ss.str();
				out << std::endl;
				out << LITERAL("endstream") << std::endl;
				out << LITERAL("endobj") << std::endl;
				out << std::endl;
				
				return index;
			}
			
			/* ------------------------------------------------------------- */
			//  xset_border
			/* ------------------------------------------------------------- */
			template <class Source, class Type>
			bool xset_border(const Source& src, Type& dest) {
				double weight = 0.75;
				size_type type = 0;
				color fill;
				if (src.first == 0) fill.invalidate();
				else fill = color(src.second);
				
				switch ((src.first & 0x00ff)) {
				case 1:		// thin
					type = 0x001;
					break;
				case 2:		// medium
					type = 0x001;
					weight = 1.5;
					break;
				case 3:		// thick
					type = 0x001;
					weight = 3.0;
					break;
				case 4:		// doubled
					type = 0x011; // currently draws solid line.
					break;
				case 10:	// dotted
					type = 0x003;
					break;
				case 14:	// hair
					type = 0x004;
					break;
				case 11:	// dash_dot
					type = 0x005;
					break;
				case 12:	// dash_wdot
					type = 0x006;
					break;
				case 13:	// dashed
					type = 0x002;
					break;
				case 20:	// m_dash_dot (medium dash-dot)
					type = 0x005;
					weight = 1.5;
					break;
				case 21:	// m_dash_wdot (medium dash-double-dot)
					type = 0x006;
					weight = 1.5;
					break;
				case 22:	// m_dashed (medium dashed)
					type = 0x002;
					weight = 1.5;
					break;
				default:
					break;
				}
				
				type |= (src.first & 0xff00);
				dest = Type(fill, weight, type);
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xputstream
			/* ------------------------------------------------------------- */
			template <class OutStream, class WorkSheet>
			bool xputstream(OutStream& out, const WorkSheet& src) {
				typedef typename WorkSheet::value_type value_type;
				typedef typename doc_type::style_type style_type;
				
				cur_.x = pp_.margin().left();
				cur_.y = pp_.height() - pp_.margin().top();
				cur_.right = 0.0;
				table_contents tbl;
				tbl.origin() = coordinate(cur_.x, cur_.y);
				std::basic_stringstream<CharT, Traits> ss; // stored text.
				
				ss << LITERAL("BT") << std::endl;
				for (size_type row = 0; row < src.size(); ++row) {
					if (src.at(row).empty()) {
						table_cell dummy(1.0, src.hdefault());
						tbl.add(dummy);
					}
					
					for (size_type col = 0; col < src.at(row).size(); ++col) {
						if ((src.at(row).at(col).type() & 0xf000)) continue; // hidden
						const style_type& st = doc_.styles().at(src.at(row).at(col).style());
						
						// 1. regist the cells information.
						double w = src.at(row).at(col).width() * XLSX_CELL_WIDTH;
						double h = src.at(row).at(col).height();
						cur_.width = xgetwidth(src, st, row, col);
						cur_.height = xgetheight(src, row, col);
						table_cell c(w, h);
						if (st.rgb() != 0xffffff) c.background() = color(st.rgb());
						this->xset_border(st.border().top(), c.top());
						this->xset_border(st.border().bottom(), c.bottom());
						this->xset_border(st.border().left(), c.left());
						this->xset_border(st.border().right(), c.right());
						this->xset_border(st.border().diagonal(), c.diagonal());
						
						if ((src.at(row).at(col).type() & value_type::row_beg)) {
							c.type() |= table_cell::row_beg;
						}
						else if ((src.at(row).at(col).type() & value_type::row_merge)) {
							c.type() |= table_cell::row_merge;
						}
						else if ((src.at(row).at(col).type() & value_type::row_end)) {
							c.type() |= table_cell::row_end;
						}
						
						if ((src.at(row).at(col).type() & value_type::col_beg)) {
							c.type() |= table_cell::col_beg;
						}
						else if ((src.at(row).at(col).type() & value_type::col_merge)) {
							c.type() |= table_cell::col_merge;
						}
						else if ((src.at(row).at(col).type() & value_type::col_end)) {
							c.type() |= table_cell::col_end;
						}
						tbl.add(c);
						
						// 2. output text.
						if (!(src.at(row).at(col).type() & value_type::empty)) {
							bool empty = true;
							if (col + 1 < src.at(row).size() &&
								(src.at(row).at(col + 1).type() & (value_type::value | value_type::sst))) {
								empty = false;
							}
							this->xputcell(ss, src.at(row).at(col), st, empty);
						}
						cur_.x += w;
					}
					
					cur_.x = pp_.margin().left();
					cur_.y -= (src.at(row).empty()) ? src.hdefault() : src.at(row).at(0).height();
					tbl.newline();
				}
				ss << LITERAL("ET") << std::endl;
				
				// finalize
				tbl(out);
				if (!src.drawings().empty()) this->xputshape(out, src);
				out << ss.str();
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xgetw
			/* ------------------------------------------------------------- */
			template <class InIter, class Helper>
			double xgetw(InIter first, InIter last, Helper& h/*, Helper& hj*/) {
#if 0
				double dest = is_ascii(first, last, charset::utf16) ?
					h.width(first, last) : hj.width(first, last);
#endif
				double dest = h.width(first, last);
				return dest / 1000.0;
			}
			
			/* ------------------------------------------------------------- */
			//  xputsst_vert
			/* ------------------------------------------------------------- */
			template <class OutStream, class Container, class StyleT>
			bool xputsst_vert(OutStream& out, const Container& v, const StyleT& st) {
				double y = cur_.y - vspace_ - 2.0;
				
				string_type latin;
				string_type japan;
				
				string_type name = st.font().latin();
				if (f_.find(name) == f_.end()) {
					font_object f(name, charset::utf16);
					f_[name] = f;
					cur_.fonts[name] = f;
					cur_.fused.insert(name);
				}
				
#if 0
				if (f_[name].property().type() == 1) {
					latin = name;
					japan = XLSX_DEFAULT_JAPAN;
				}
				else {
					latin = XLSX_DEFAULT_LATIN;
					japan = name;
				}
				
				text_helper h(f_[latin].property(), charset::utf16);
				text_helper hj(f_[japan].property(), charset::utf16);
#else
				text_helper h(f_[name].property(), charset::utf16);
#endif
				
				double x = cur_.x + space_;
				size_type i = 0;
				double size = (v.at(i).size() > 0) ? v.at(i).size() : st.font().size();
				double hs = size * XLSX_FONT_SCALE;
				double sp = cur_.width - 2.0 * space_ - hs;
				if (sp > 0) x += sp / 2.0;
				typename string_type::const_iterator pos = v.at(i).data().begin();
				while (i < v.size()) {
					string_type tmp;
					tmp += *pos;
					++pos;
					tmp += *pos;
					++pos;
					
					//text_contents txt(tmp, coordinate(x, y), f_[latin], f_[japan], false);
					text_contents txt(tmp, coordinate(x, y), f_[name], false);
					txt.font_size(hs);
					if (v.at(i).fill().is_valid()) txt.font_color(v.at(i).fill());
					else txt.font_color(color(st.font().rgb()));
					txt.decorate(v.at(i).decorate());
					txt(out);
					
					y -= size;
					//x = cur_.x + space_ + 2.0;
					if (pos == v.at(i).data().end()) {
						++i;
						if (i >= v.size()) break;
						pos = v.at(i).data().begin();
						size = (v.at(i).size() > 0) ? v.at(i).size() : st.font().size();
						hs = size * XLSX_FONT_SCALE;
					}
				}
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xmove_vpos
			/* ------------------------------------------------------------- */
			template <class OutStream, class Container, class StyleT, class Helper>
			double xmove_vpos(OutStream& out, const Container& v,
				const StyleT& st, Helper& h/*, Helper& hj*/) {
				double height = 0.0;
				double total = 0.0;
				for (size_type i = 0; i < v.size(); ++i) {
					double size = (v.at(i).size() > 0) ? v.at(i).size() : st.font().size();
					double hs = size * XLSX_FONT_SCALE;
					double w = this->xgetw(v.at(i).data().begin(), v.at(i).data().end(), h) * hs;
					total += w;
				}
				
				double width = cur_.width - 2.0 * space_;
				double x = cur_.x + space_;
				
				size_type i = 0;
				double size = (v.at(i).size() > 0) ? v.at(i).size() : st.font().size();
				double hs = size * XLSX_FONT_SCALE;
				typename string_type::const_iterator pos = v.at(i).data().begin();
				if (st.wrap() || st.align() == 3 || st.align() == 4) {
					if (total > width && total <= width + 2.0 * space_) {
						double delta = width + 2.0 * space_ - total;
						x -= delta / 2.0;
						width += delta;
					}
					
					typename string_type::const_iterator newline = std::find(pos, v.at(i).data().end(), LITERAL('\n'));
					while (total > width || newline != v.at(i).data().end()) {
						double limit = width / hs * 1000.0;
						string_type tmp;
						h.getline(pos, v.at(i).data().end(), std::inserter(tmp, tmp.end()), limit);
						
						if (tmp.empty() && pos != v.at(i).data().end()) {
							tmp += *pos;
							++pos;
							tmp += *pos;
							++pos;
						}
						
						total -= this->xgetw(tmp.begin(), tmp.end(), h) * hs;
						utf16::chomp(tmp);
						
						if (!tmp.empty()) {
							height += size * baseline_;
							x = cur_.x + space_;
						}
						
						if (pos == v.at(i).data().end()) {
							++i;
							if (i >= v.size()) break;
							pos = v.at(i).data().begin();
							size = (v.at(i).size() > 0) ? v.at(i).size() : st.font().size();
							hs = size * XLSX_FONT_SCALE;
						}
						
						newline = std::find(pos, v.at(i).data().end(), LITERAL('\n'));
					}
				}
				
				if (i < v.size()) {
					size = (v.at(i).size() > 0) ? v.at(i).size() : st.font().size();
					height += size * baseline_;
				}
				
				double y = cur_.y - vspace_;
				double vsp = cur_.height - 2.0 * vspace_ - height;
				if (st.valign() == 0) y -= 2.0;
				if (st.valign() == 1) y -= vsp / 2.0;
				else if (st.valign() == 2) y -= (vsp - 2.0);
				
				return y;
			}
			
			/* ------------------------------------------------------------- */
			//  xputsst
			/* ------------------------------------------------------------- */
			template <class OutStream, class Container, class StyleT>
			bool xputsst(OutStream& out, const Container& v, const StyleT& st, size_type type, bool empty) {
				if (v.empty()) return true;
				if (st.rotate() == 90 || st.rotate() == 180 || st.rotate() == 255) {
					return this->xputsst_vert(out, v, st);
				}
				
				string_type name = st.font().latin();
				if (f_.find(name) == f_.end()) {
					font_object f(name, charset::utf16);
					f_[name] = f;
					cur_.fonts[name] = f;
					cur_.fused.insert(name);
				}
				text_helper h(f_[name].property(), charset::utf16);
				
				// ëSï∂éöóÒÇÃïùÇÃåvéZÅD
				double total = 0.0;
				for (size_type i = 0; i < v.size(); ++i) {
					double size = (v.at(i).size() > 0) ? v.at(i).size() : st.font().size();
					double hs = size * XLSX_FONT_SCALE;
					double w = this->xgetw(v.at(i).data().begin(), v.at(i).data().end(), h) * hs;
					total += w;
				}
				
				double x = cur_.x + space_;
				
				double width = cur_.width - 2.0 * space_;
				double y = this->xmove_vpos(out, v, st, h);
				
				size_type i = 0;
				double size = (v.at(i).size() > 0) ? v.at(i).size() : st.font().size();
				double hs = size * XLSX_FONT_SCALE;
				typename string_type::const_iterator pos = v.at(i).data().begin();
				if (st.wrap() || st.align() == 3 || st.align() == 4) {
					if (total > width && total <= width + 2.0 * space_) {
						double delta = width + 2.0 * space_ - total;
						x -= delta / 2.0;
						width += delta;
					}
					
					if (x + width > pp_.width()) cur_.right = std::max(cur_.right, x + width - pp_.width());
					
					typename string_type::const_iterator newline = std::find(pos, v.at(i).data().end(), LITERAL('\n'));
					while (total > width || newline != v.at(i).data().end()) {
						double limit = width / hs * 1000.0;
						string_type tmp;
						h.getline(pos, v.at(i).data().end(), std::inserter(tmp, tmp.end()), limit);
						if (tmp.empty() && pos != v.at(i).data().end()) {
							tmp += *pos;
							++pos;
							tmp += *pos;
							++pos;
						}
						
						total -= this->xgetw(tmp.begin(), tmp.end(), h) * hs;
						utf16::chomp(tmp);
						
						if (!tmp.empty()) {
							double sp = width - this->xgetw(tmp.begin(), tmp.end(), h) * hs;
							if (st.align() == 1) x += std::max(0.0, sp / 2.0);
							else if (st.align() == 2 ||  (st.align() == 5 && type == 0x04)) {
								x += std::max(0.0, sp);
							}
							
							double pad = (size - st.font().size()) / 2.0;
							text_contents txt(tmp, coordinate(x, y + pad), f_[name], false);
							txt.font_size(hs);
							if (v.at(i).fill().is_valid()) txt.font_color(v.at(i).fill());
							else txt.font_color(color(st.font().rgb()));
							txt.decorate(v.at(i).decorate());
							txt(out);
							
							y -= size * baseline_;
							x = cur_.x + space_;
						}
						
						if (pos == v.at(i).data().end()) {
							++i;
							if (i >= v.size()) break;
							pos = v.at(i).data().begin();
							size = (v.at(i).size() > 0) ? v.at(i).size() : st.font().size();
							hs = size * XLSX_FONT_SCALE;
						}
						
						newline = std::find(pos, v.at(i).data().end(), LITERAL('\n'));
					}
				}
				
				double sp = width - total;
				if (st.align() == 1) {
					if (st.wrap()) x += std::max(0.0, sp / 2.0);
					else x += sp / 2.0;
					if (x + total / 2.0 > pp_.width()) cur_.right = std::max(cur_.right, x + total / 2.0 - pp_.width());
				}
				else if (st.align() == 2 ||  (st.align() == 5 && type == 0x04)) {
					if (st.wrap()) x += std::max(0.0, sp);
					else x += sp;
					if (x > pp_.width()) cur_.right = std::max(cur_.right, x - pp_.width());
				}
				else {
					if (x + total > pp_.width()) cur_.right = std::max(cur_.right, x + total - pp_.width());
				}
				
				double wline = 0.0;
				while (i < v.size()) {
					string_type tmp;
					
					double w = this->xgetw(pos, v.at(i).data().end(), h) * hs;
					bool done = false;
					if (!st.wrap() && !empty && wline + w > width) {
						if (wline + w > width + 2 * space_) {
							size_type limit = std::max((width - wline) / hs * 1000.0, 0.0);
							h.getline(pos, v.at(i).data().end(), std::inserter(tmp, tmp.end()), limit);
						}
						else tmp = string_type(pos, v.at(i).data().end());
						done = true;
					}
					else {
						tmp = string_type(pos, v.at(i).data().end());
						wline += w;
					}
					
					double pad = size - st.font().size();
					if (st.valign() == 0) pad = 0.0;
					else if (st.valign() == 1) pad /= 2.0;
					text_contents txt(tmp, coordinate(x, y + pad), f_[name], false);
					txt.font_size(hs);
					if (v.at(i).fill().is_valid()) txt.font_color(v.at(i).fill());
					else txt.font_color(color(st.font().rgb()));
					txt.decorate(v.at(i).decorate());
					txt(out);
					x += w;
					++i;
					if (i >= v.size() || done) break;
					pos = v.at(i).data().begin();
					size = (v.at(i).size() > 0) ? v.at(i).size() : st.font().size();
					hs = size * XLSX_FONT_SCALE;
				}
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xputcell
			/* ------------------------------------------------------------- */
			template <class OutStream, class Type, class StyleT>
			bool xputcell(OutStream& out, const Type& data, const StyleT& st, bool empty) {
				string_type latin;
				string_type japan;
				string_type name = st.font().latin();
				if (f_.find(name) == f_.end()) {
					font_object f(name, charset::utf16);
					f_[name] = f;
					cur_.fonts[name] = f;
					cur_.fused.insert(name);
				}
				
				if (f_[name].property().type() == 1) {
					latin = name;
					japan = XLSX_DEFAULT_JAPAN;
				}
				else {
					latin = XLSX_DEFAULT_LATIN;
					japan = name;
				}
				
				text_helper h(f_[latin].property(), charset::utf16);
				text_helper hj(f_[japan].property(), charset::utf16);
				
				// 1. split string.
				double size = st.font().size();
				//double limit = (cur_.width - (space_ + 2.0) * 2.0) / size * 1000.0;
				std::vector<string_type> v;
				std::vector<piece_type> ssts;
				string_type s = this->xgets(data, st,
					static_cast<size_type>((cur_.width - space_ * 2.0) / (size * 0.6)), ssts);
				if (!ssts.empty()) return this->xputsst(out, ssts, st, data.type(), empty);
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xputshape
			/* ------------------------------------------------------------- */
			template <class OutStream, class WorkSheet>
			bool xputshape(OutStream& out, const WorkSheet& src) {
				typedef typename faml::officex::basic_shape<CharT, Traits>::path_container paths_type;
				
				size_type ch = 0;
				for (size_type i = 0; i < src.drawings().size(); ++i) {
					double w = emu(src.drawings().at(i).width());
					double h = emu(src.drawings().at(i).height());
					double x = pp_.margin().left() + emu(src.drawings().at(i).origin().x());
					double y = pp_.height() - (pp_.margin().top() + emu(src.drawings().at(i).origin().y()) + h);
					
					cur_.width = emu(src.drawings().at(i).width());
					cur_.height = emu(src.drawings().at(i).height());
					cur_.x = pp_.margin().left() + emu(src.drawings().at(i).origin().x());
					cur_.y = pp_.height() - (pp_.margin().top() + emu(src.drawings().at(i).origin().y()));
					string_type ref = src.drawings().at(i).reference();
					if (!ref.empty()) {
						if (ref.compare(0, 5, LITERAL("chart")) == 0) {
							this->xputchart(out, src, ch, x, y, w, h);
							++ch;
						}
						else this->xput_image(out, src.drawings().at(i));
						continue;
					}
					
					// put shape.
					shape_ptr sh;
					if (src.drawings().at(i).name() == LITERAL("custom")) {
						sh = shape_ptr(new contents::basic_custom_shape<paths_type>(coordinate(x, y), w, h));
						dynamic_cast<contents::basic_custom_shape<paths_type>* >(
							sh.get())->paths(src.drawings().at(i).paths());
					}
					else sh = contents::makeshape(src.drawings().at(i).name(), coordinate(x, y), w, h);
					if (!sh) continue;
					
					if (src.drawings().at(i).name() == LITERAL("line")) {
						dynamic_cast<contents::basic_line<CharT, Traits>* >(sh.get())->option(src.drawings().at(i).option());
					}
					color clr = color(src.drawings().at(i).border());
					if (src.drawings().at(i).border() > 0x00ffffff) clr.invalidate();
					sh->border(clr);
					if (src.drawings().at(i).border_type() > 0) sh->type(src.drawings().at(i).border_type());
					
					if (!src.drawings().at(i).backgrounds().empty()) {
						clr = color(src.drawings().at(i).background());
						if (src.drawings().at(i).background() > 0x00ffffff) clr.invalidate();
						sh->background(clr);
					}
					
					sh->angle(clx::radian(src.drawings().at(i).angle()));
					sh->flip(src.drawings().at(i).flip());
					sh->weight(src.drawings().at(i).weight());
					
					typedef typename faml::officex::basic_shape<CharT, Traits>::adjust_map::const_iterator adj_iterator;
					for (adj_iterator pos = src.drawings().at(i).adjusts().begin();
						pos != src.drawings().at(i).adjusts().end(); ++pos) {
						sh->adjust(shu2(pos->second), pos->first);
					}
					
					typedef typename faml::officex::basic_shape<CharT, Traits>::effect_map::const_iterator effect_iterator;
					for (effect_iterator pos = src.drawings().at(i).effects().begin();
						pos != src.drawings().at(i).effects().end(); ++pos) {
						sh->effect(pos->first, pos->second);
					}
					(*sh)(out);
					
					// put textbox.
					this->xput_shape_text(out, src.drawings().at(i));
				}
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xput_shape_text
			/* ------------------------------------------------------------- */
			template <class OutStream, class ShapeT>
			bool xput_shape_text(OutStream& out, const ShapeT& sh) {
				typedef typename ShapeT::txbox::value_type paragraph;
				if (sh.texts().empty()) return true;
				
				/*
				babel::init_babel();
				size_type line = this->xgetline(sh.texts());
				double size = 0.0;
				for (size_type i = 0; i < sh.texts().size(); ++i) {
					double avgh = 0.0;
					for (size_type j = 0; j < sh.texts().at(i).size(); ++j) {
						avgh += sh.texts().at(i).at(j).font().size();
					}
					
					if (!sh.texts().at(i).empty()) size += (avgh / sh.texts().at(i).size());
					else size += sh.texts().at(i).font().size();
				}
				size /= sh.texts().size();
				*/
				
				/*
				double vsp = std::max(0.0, cur_.height - size * baseline_ * line);
				if (sh.texts().valign() == 1) {
					cur_.y -= (vsp / 2.0 - 3.5);
				}
				else if (sh.texts().valign() == 2) cur_.y -= (vsp - 3.5);
				*/
				
				//bool first_line = true;
				double ox = cur_.x;
				for (size_type i = 0; i < sh.texts().size(); ++i) {
					const paragraph& p = sh.texts().at(i);
					if (p.empty()) {
						/*
						size_type foo = i;
						while (foo > 0 && sh.texts().at(foo).empty()) --foo;
						if (foo == 0) cur_.y -= p.font().size();
						else cur_.y -= sh.texts().at(foo).at(0).font().size();
						if (first_line) {
							cur_.y -= 3.5;
							first_line = false;
						}
						*/
						cur_.y -= 9.0;
						continue;
					}
					
					this->xmove_horizon(ox, p, 0, string_type());
					
					/*
					if (first_line) {
						cur_.y -= 3.5;
						first_line = false;
					}
					*/
					
					for (size_type j = 0; j < p.size(); ++j) {
						this->xput_shape_text_piece(out, p, j, ox);
					}
					cur_.x = ox;
				}
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xmove_horizon
			/* ------------------------------------------------------------- */
			template <class Source>
			bool xmove_horizon(double ox, const Source& src, size_type pos, const string_type& rest) {
				double width = 0.0;
				if (!rest.empty()) {
					if (pos == 0) throw std::runtime_error("wrong text index");
					
					string_type latin = src.at(pos - 1).font().latin();
					if (f_.find(latin) == f_.end()) throw std::runtime_error("cannof find font");
					string_type japan = src.at(pos - 1).font().japan();
					if (f_.find(japan) == f_.end()) throw std::runtime_error("cannof find font");
					text_helper h(f_[latin].property(), charset::utf16);
					text_helper hj(f_[japan].property(), charset::utf16);
					
					bool newline = false;
					string_type s = rest;
					if (s.size() >= 2 && s.at(s.size() - 2) == 0x00 &&
						(s.at(s.size() - 1) == 0x0a || s.at(s.size() - 1) == 0x0d || s.at(s.size() - 1) == 0x0c)) {
						s.erase(s.size() - 2);
						newline = true;
					}
					
					double w = (is_ascii(rest.begin(), rest.end(), charset::utf16))
						? h.width(rest.begin(), rest.end())
						: hj.width(rest.begin(), rest.end());
					width += (w / 1000.0) * src.at(pos - 1).font().size() * XLSX_FONT_SCALE;
					
					double indent = emu(src.indent());
					double space = cur_.width - width;
					if (space - indent < 0) {
						cur_.x = ox + indent + space_;
						return true;
					}
					
					if (newline) {
						if (src.align() == 1) cur_.x = ox + space / 2.0;
						else if (src.align() == 2) cur_.x = ox + space - space_;
						else cur_.x = ox + indent + space_;
						return true;
					}
				}
				
				double indent = emu(src.indent());
				for (size_type i = pos; i < src.size(); ++i) {
					string_type latin = src.at(i).font().latin();
					if (f_.find(latin) == f_.end()) {
						font_object f(latin, charset::utf16);
						f_[latin] = f;
						cur_.fonts[latin] = f;
						cur_.fused.insert(latin);
					}
					text_helper h(f_[latin].property(), charset::utf16);
					
					string_type japan = src.at(i).font().japan();
					if (f_.find(japan) == f_.end()) {
						font_object f(japan, charset::utf16);
						f_[japan] = f;
						cur_.fonts[japan] = f;
						cur_.fused.insert(japan);
					}
					text_helper hj(f_[japan].property(), charset::utf16);
					
					std::basic_string<wchar_t> tmp = babel::utf8_to_unicode(src.at(i).data());
					//string_type s = win32conv(tmp.begin(), tmp.end());
					string_type s = to_narrow(tmp.begin(), tmp.end());
					bool newline = false;
					if (s.size() >= 2 && s.at(s.size() - 2) == 0x00 &&
						(s.at(s.size() - 1) == 0x0a || s.at(s.size() - 1) == 0x0d || s.at(s.size() - 1) == 0x0c)) {
						s.erase(s.size() - 2);
						newline = true;
					}
					
					double w = (is_ascii(s.begin(), s.end(), charset::utf16))
						? h.width(s.begin(), s.end())
						: hj.width(s.begin(), s.end());
					width += (w / 1000.0) * src.at(i).font().size() * XLSX_FONT_SCALE;
					double space = cur_.width - width;
					if (space - indent < 0.0) {
						cur_.x = ox + indent + space_;
						return true;
					}
					
					if (newline) {
						if (src.align() == 1) cur_.x = ox + space / 2.0;
						else if (src.align() == 2) cur_.x = ox + space - space_;
						else cur_.x = ox + indent + space_;
						return true;
					}
				}
				
				double space = cur_.width - width;
				if (src.align() == 1) cur_.x = ox + space / 2.0;
				else if (src.align() == 2) cur_.x = ox + space - space_;
				else cur_.x = ox + indent + space_;
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xput_shape_text_piece
			/* ------------------------------------------------------------- */
			template <class OutStream, class Source>
			bool xput_shape_text_piece(OutStream& out, const Source& src, size_type pos, double ox) {
				//double size = src.at(pos).font().size() * XLSX_FONT_SCALE;
				double size = 9.0;
				if (src.at(pos).data().empty()) {
					if(pos >= src.size() - 1) {
						//cur_.y -= size * baseline_;
						cur_.y -= src.at(pos).font().size() * baseline_;
						this->xmove_horizon(ox, src, pos + 1, string_type());
					}
					return true;
				}
				
				//string_type latin = src.at(pos).font().latin();
				string_type latin = XLSX_DEFAULT_LATIN;
				if (f_.find(latin) == f_.end()) {
					font_object f(latin, charset::utf16);
					f_[latin] = f;
					cur_.fonts[latin] = f;
					cur_.fused.insert(latin);
				}
				text_helper h(f_[latin].property(), charset::utf16);
				
				//string_type japan = src.at(pos).font().japan();
				string_type japan = XLSX_DEFAULT_JAPAN;
				if (f_.find(japan) == f_.end()) {
					font_object f(japan, charset::utf16);
					f_[japan] = f;
					cur_.fonts[japan] = f;
					cur_.fused.insert(japan);
				}
				text_helper hj(f_[japan].property(), charset::utf16);
				std::basic_string<wchar_t> tmp = babel::utf8_to_unicode(src.at(pos).data());
				//string_type s = win32conv(tmp.begin(), tmp.end());
				string_type s = to_narrow(tmp.begin(), tmp.end());
				
				typename string_type::iterator it = s.begin();
				
				bool newline = false;
				if (s.size() >= 2 && s.at(s.size() - 2) == 0x00 &&
					(s.at(s.size() - 1) == 0x0a || s.at(s.size() - 1) == 0x0d || s.at(s.size() - 1) == 0x0c)) {
					newline = true;
					s.erase(s.size() - 2);
				}
				
				while (1) {
					string_type line;
					double width = cur_.width - (cur_.x - ox) - space_;
					int limit = static_cast<int>(width / size * 1000);
					if (limit < 1000) {
						cur_.y -= src.at(pos).font().size() * baseline_;
						this->xmove_horizon(ox, src, pos + 1, s);
						width = cur_.width - (cur_.x - ox) - space_;
						limit = static_cast<int>(width / size * 1000);
					}
					
					if (is_ascii(it, s.end(), charset::utf16)) {
						h.getline(it, s.end(), std::inserter(line, line.end()), limit, false);
					}
					else hj.getline(it, s.end(), std::inserter(line, line.end()), limit, false);
					
					if (line.empty()) {
						if (it != s.end()) {
							line += *it;
							++it;
							line += *it;
							++it;
						}
						else break;
					}
					
					double n = (is_ascii(line.begin(), line.end(), charset::utf16))
						? h.width(line.begin(), line.end()) / 1000.0
						: hj.width(line.begin(), line.end()) / 1000.0;
					double sp = width - n * size;
					size_type rgb = src.at(pos).font().rgb();
					if (rgb > 0x00ffffff) rgb = 0;
					
					size_type deco = src.at(pos).decorate();
					if ((deco & 0x10)) {
						deco |= 0x04;
						rgb = 0x0000ff;
					}
					
					text_contents txt(line, coordinate(cur_.x, cur_.y), f_[latin], f_[japan]);
					txt.font_size(size);
					txt.font_color(color(rgb));
					txt.decorate(deco);
					if (sp < 0.0) txt.space(sp);
					txt(out);
					
					if (it == s.end()) {
						cur_.x += n * size;
						break;
					}
					
					line.assign(it, s.end());
					cur_.y -= size * baseline_;
					//cur_.y -= src.at(pos).font().size() * baseline_;
					this->xmove_horizon(ox, src, pos + 1, line);
				}
				
				if (newline || pos >= src.size() - 1) {
					cur_.y -= size * baseline_;
					//cur_.y -= src.at(pos).font().size() * baseline_;
					this->xmove_horizon(ox, src, pos + 1, string_type());
				}
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xput_image
			/* ------------------------------------------------------------- */
			template <class OutStream, class ShapeT>
			bool xput_image(OutStream& out, const ShapeT& sh) {
				if (objs_.find(sh.reference()) != objs_.end()) {
					const xobj_type& ref = objs_[sh.reference()];
					image_contents image(ref.label,
						coordinate(cur_.x, cur_.y - cur_.height), cur_.width, cur_.height);
					image(out);
					cur_.objs.push_back(ref);
					return true;
				}
				
				string_type ext = sh.reference().substr(sh.reference().find_last_of(LITERAL(".")));
				if (ext == LITERAL(".wmf")) {
					typename storage_type::iterator pos = in_.find(sh.reference());
					if (pos == in_.end()) throw std::runtime_error("cannot find image file");
					std::vector<char_type> s;
					clx::read(*pos, s);
					clx::basic_ivstream<CharT, Traits> vs(s);
					
					contents::basic_wmf<CharT, Traits> wmf(vs);
					try {
						wmf.read_header();
					}
					catch (std::runtime_error&) {}
					wmf.origin(coordinate(cur_.x, cur_.y - cur_.height));
					wmf.width(cur_.width);
					wmf.height(cur_.height);
					wmf.font1st(f_[XLSX_DEFAULT_LATIN]);
					wmf.font2nd(f_[XLSX_DEFAULT_JAPAN]);
					wmf(out);
				}
				else if (ext == LITERAL(".emf")) {
					typename storage_type::iterator pos = in_.find(sh.reference());
					if (pos == in_.end()) throw std::runtime_error("cannot find image file");
					std::vector<char_type> s;
					clx::read(*pos, s);
					clx::basic_ivstream<CharT, Traits> vs(s);
					
					contents::basic_emf<CharT, Traits> emf(vs);
					try {
						emf.read_header();
					}
					catch (std::runtime_error&) {}
					emf.origin(coordinate(cur_.x, cur_.y - cur_.height));
					emf.width(cur_.width);
					emf.height(cur_.height);
					emf(out);
				}
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xputchart
			/* ------------------------------------------------------------- */
			template <class OutStream, class WorkSheet>
			bool xputchart(OutStream& out, const WorkSheet& src,
				size_type pos, double x, double y, double w, double h) {
				typedef typename WorkSheet::chart_type chart_type;
				
				switch (src.charts().at(pos).type()) {
				case 0:
				{
					contents::barchart<chart_type> ch(coordinate(x, y), w, h);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				case 1:
				{
					contents::rowchart<chart_type> ch(coordinate(x, y), w, h);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				case 2:
				{
					contents::linechart<chart_type> ch(coordinate(x, y), w, h);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				case 3:
				{
					contents::piechart<chart_type> ch(coordinate(x, y), w, h);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				case 4:
				{
					contents::areachart<chart_type> ch(coordinate(x, y), w, h);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				case 5:
				{
					contents::scatterchart<chart_type> ch(coordinate(x, y), w, h);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				case 6:
				{
					contents::bar3dchart_box<chart_type> ch(coordinate(x, y), w, h);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				case 7:
				{
					contents::bar3dchart_cylinder<chart_type> ch(coordinate(x, y), w, h);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				case 8:
				{
					contents::bar3dchart_cone<chart_type> ch(coordinate(x, y), w, h);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				case 9:
				{
					contents::bar3dchart_pyramid<chart_type> ch(coordinate(x, y), w, h);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				case 10:
				{
					contents::row3dchart_box<chart_type> ch(coordinate(x, y), w, h);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				case 18:
				{
					contents::pie3dchart<chart_type> ch(coordinate(x, y), w, h);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				case 19:
				{
					contents::pie3dchart_explosion<chart_type> ch(coordinate(x, y), w, h);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				default:
				{
					contents::barchart<chart_type> ch(coordinate(x, y), w, h);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				}
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xputchart
			/* ------------------------------------------------------------- */
			template <class OutStream, class WorkSheet, class Type>
			bool xputchart_exec(OutStream& out, const WorkSheet& src, size_type pos, Type& ch) {
				ch.font1st(f_[XLSX_DEFAULT_LATIN]);
				ch.font2nd(f_[XLSX_DEFAULT_JAPAN]);
				if (src.charts().at(pos).border().first) {
					ch.border(color(src.charts().at(pos).border().second));
				}
				
				if (src.charts().at(pos).background().first) {
					ch.background(color(src.charts().at(pos).background().second));
				}
				
				if (src.charts().at(pos).plot_border().first) {
					ch.plot_border(color(src.charts().at(pos).plot_border().second));
				}
				
				if (src.charts().at(pos).plot_area().first) {
					ch.plot_area(color(src.charts().at(pos).plot_area().second));
				}
				
				if (src.charts().at(pos).legend_border().first) {
					ch.legend_border(color(src.charts().at(pos).plot_area().second));
				}
				
				if (src.charts().at(pos).legend_area().first) {
					ch.legend_area(color(src.charts().at(pos).plot_area().second));
				}
				
				ch.option(src.charts().at(pos).option());
				ch.position(src.charts().at(pos).legend_position());
				ch.value_axis(src.charts().at(pos).value_axis());
				ch.data(src.charts().at(pos));
				if (doc_.theme()) ch.palette(doc_.theme()->palette().data());
				ch(out);
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xsplit
			/* ------------------------------------------------------------- */
			template <class Container, class StyleT>
			bool xsplit(const string_type& src, Container& dest, const StyleT& st, size_type rgb) {
				typename string_type::const_iterator pos = src.begin();
				bool ascii = is_ascii(pos, src.end(), charset::utf16);
				string_type s;
				while (pos != src.end()) {
					if (ascii != is_ascii(pos, src.end(), charset::utf16)) {
						piece_type elem;
						elem.data(s);
						elem.size(st.font().size());
						elem.decorate(st.font().decorate());
						if (rgb <= 0x00ffffff) elem.fill(color(rgb));
						else elem.fill(color(st.font().rgb()));
						dest.push_back(elem);
						s.erase();
						ascii = !ascii;
					}
					else {
						s += *pos;
						++pos;
						if (pos == src.end()) break;
						s += *pos;
						++pos;
					}
				}
				
				if (!s.empty()) {
					piece_type elem;
					elem.data(s);
					elem.size(st.font().size());
					elem.decorate(st.font().decorate());
					if (rgb <= 0x00ffffff) elem.fill(color(rgb));
					else elem.fill(color(st.font().rgb()));
					dest.push_back(elem);
				}
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xgets
			/* ------------------------------------------------------------- */
			template <class Type, class StyleT, class Container>
			string_type xgets(const Type& data, const StyleT& st, size_type limit, Container& v) {
				string_type dest;
				
				if ((data.type() & Type::value)) {
					size_type fill = size_type(-1);
					faml::xlsx::format(dest, fill, st.numfmt(), data.data());
					size_t first = dest.find_last_of(LITERAL("."));
					if (first != string_type::npos) {
						size_type pos = dest.size() - 1;
						while (pos > first) {
							if (pos < limit) {// && dest.at(pos) != LITERAL('0')) {
								if (pos < dest.size() - 1) ++pos;
								break;
							}
							--pos;
						}
						if (pos < dest.size() - 1) dest.erase(pos);
					}
					std::basic_string<wchar_t> tmp = babel::utf8_to_unicode(dest);
					//dest = win32conv(tmp.begin(), tmp.end());
					dest = to_narrow(tmp.begin(), tmp.end());
					this->xsplit(dest, v, st, fill);
				}
				else if ((data.type() & Type::sst)) {
					int index = static_cast<int>(data.data());
					for (size_type i = 0; i < doc_.sst().at(index).size(); ++i) {
						piece_type elem(doc_.sst().at(index).at(i));
						clx::escape_separator<char_type> sep(LITERAL(";"), LITERAL("\""), LITERAL("\\"));
						clx::basic_tokenizer<clx::escape_separator<char_type>, string_type> tok(st.numfmt(), sep);
						string_type s;
						if (!tok.empty() && tok.at(0).find(LITERAL("@")) != string_type::npos) {
							faml::xlsx::format(s, st.numfmt(), elem.data());
						}
						else s = elem.data();
						
						std::basic_string<wchar_t> tmp = babel::utf8_to_unicode(s);
						s = to_narrow(tmp.begin(), tmp.end());
						elem.data(s);
						if (doc_.sst().at(index).at(i).size() > 0.0) elem.size(doc_.sst().at(index).at(i).size());
						else elem.size(st.font().size());
						elem.decorate(doc_.sst().at(index).at(i).decorate() | st.font().decorate());
						if (!elem.fill().is_valid()) elem.fill(color(st.font().rgb()));
						v.push_back(elem);
						dest += s;
					}
				}
				
				return dest;
			}
			
			/* ------------------------------------------------------------- */
			/*
			 *  xgetwidth
			 *
			 *  Calculates width of the current cell. Returns the sum of
			 *  merged cell's widths if the current cell is the begining
			 *  of merged cells. Otherwise returns width of the current
			 *  cell.
			 */
			/* ------------------------------------------------------------- */
			template <class WorkSheet, class StyleT>
			double xgetwidth(const WorkSheet& src, const StyleT& st, size_type row, size_type col) {
				typedef typename WorkSheet::value_type value_type;
				//double dest = space_;
				double dest = 0.0;
				dest += src.at(row).at(col).width() * XLSX_CELL_WIDTH;
				if ((src.at(row).at(col).type() & value_type::empty) ||
				   !(src.at(row).at(col).type() & 0x700)) { // (col_beg|col_merge|col_end)
					//return dest + space_;
					return dest;
				}
				
				for (size_type i = col + 1; i < src.at(row).size(); ++i) {
					//dest += space_;
					dest += src.at(row).at(i).width() * XLSX_CELL_WIDTH;
					if ((src.at(row).at(i).type() & value_type::col_end) ||
						(src.at(row).at(i).type() & value_type::hidden)) {
						break;
					}
				}
				//return dest + space_;
				return dest;
			}
			
			/* ------------------------------------------------------------- */
			/*
			 *  xgetheight
			 *
			 *  Calculates height of the current cell. Returns the sum of
			 *  merged cell's heights if the current cell is the begining
			 *  of merged cells. Otherwise returns height of the current
			 *  cell.
			 */
			/* ------------------------------------------------------------- */
			template <class WorkSheet>
			double xgetheight(const WorkSheet& src, size_type row, size_type col) {
				typedef typename WorkSheet::value_type value_type;
				double dest = vspace_;
				dest += src.at(row).at(col).height();
				if ((src.at(row).at(col).type() & value_type::empty) ||
				   !(src.at(row).at(col).type() & 0x070)) { // (row_beg|row_merge|row_end)
					return dest + vspace_;
				}
				
				if ((src.at(row).at(col).type() & 0x060)) { // (row_merge|row_end)
					for (size_type i = row; i > 0; --i) {
						if (col >= src.at(i - 1).size()) break; // ???
						dest += vspace_;
						dest += src.at(i - 1).at(col).height();
						if ((src.at(i - 1).at(col).type() & value_type::row_beg)) break;
					}
				}
				
				if ((src.at(row).at(col).type() & 0x030)) { // (row_beg|row_merge)
					for (size_type i = row + 1; i < src.size(); ++i) {
						if (col >= src.at(i).size()) break; // ???
						dest += vspace_;
						dest += src.at(i).at(col).height();
						if ((src.at(i).at(col).type() & value_type::row_end)) break;
					}
				}
				
				return dest + vspace_;
			}
			
			/* ------------------------------------------------------------- */
			//  xadjust_size
			/* ------------------------------------------------------------- */
			template <class WorkSheet>
			bool xadjust_size(const WorkSheet& src) {
				pp_.width(a4::width);
				pp_.height(a4::height);
				
				double width = 0.0;
				double height = 0.0;
				for (size_type i = 0; i < src.size(); ++i) {
					if (src.at(i).empty()) {
						height += src.hdefault() + 2.0 * vspace_;
						continue;
					}
					height += src.at(i).at(0).height() + 2.0 * vspace_;
					double tmp = 0;
					for (size_type j = 0; j < src.at(i).size(); ++j) {
						if ((src.at(i).at(j).type() & 0xf000)) continue;
						
						if (src.at(i).at(j).width() > 0.0) {
							tmp += src.at(i).at(j).width() * XLSX_CELL_WIDTH;
							//tmp += 2.0 * space_;
						}
					}
					if (width < tmp) width = tmp;
				}
				
				for (size_type i = 0; i < src.drawings().size(); ++i) {
					double tmp = emu(src.drawings().at(i).origin().x()
						+ src.drawings().at(i).width());
					if (width < tmp) width = tmp;
					
					tmp = emu(src.drawings().at(i).origin().y()
						+ src.drawings().at(i).height());
					if (height < tmp) height = tmp;
				}
				
				// custom size
				if (pp_.width() - (pp_.margin().left() + pp_.margin().right()) < width ||
					pp_.height() - (pp_.margin().top() + pp_.margin().bottom()) < height) {
					pp_.width(static_cast<int>(width) + 1 + pp_.margin().left() + pp_.margin().right());
					pp_.height(static_cast<int>(height) + 1 + pp_.margin().top() + pp_.margin().bottom());
				}
				clx::logger::debug(DEBUGF("paper size: %d x %d (%d)", pp_.width(), pp_.height(), pp_.margin().left()));
				
				return true;
			}
		};
		
		typedef basic_xlsx<char> xlsx;
	}
}

#endif // FAML_PDF_XLSX_H
