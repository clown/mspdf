#ifndef FAML_PDF_DOCX_H
#define FAML_PDF_DOCX_H

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

#include "unit.h"
#include "code_convert.h"
#include "page.h"
#include "font.h"
#include "font_instance.h"
#include "text_helper.h"
#include "jpeg.h"
#include "png.h"

#include "contents/msshape.h"
#include "contents/wmf.h"
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

#include "docx/document.h"

#define DOCX_DEFAULT_LATIN "Century"
#define DOCX_DEFAULT_JAPAN "ＭＳ 明朝"

namespace faml {
	namespace pdf {
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_docx {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			
			typedef basic_page<CharT, Traits> page_object;
			typedef basic_font<CharT, Traits> font_object;
			typedef std::map<string_type, font_object> font_map;
			
			basic_docx(const string_type& path) :
				doc_(path), pp_(slide::width, slide::height), f_(),
				space_(5.4), baseline_(18.0) {
				this->xinit(path);
			}
			
			virtual ~basic_docx() throw() {}
			
			template <class OutStream, class PDFManager>
			bool operator()(OutStream& out, PDFManager& pm) {
				// currently implementation
				for (typename font_map::iterator pos = f_.begin(); pos != f_.end(); ++pos) {
					(pos->second)(out, pm);
				}
				
				this->xobjects(out, pm, doc_.texts());
				this->xobjects(out, pm, doc_.header());
				this->xobjects(out, pm, doc_.footer());
				
				size_type i = 0;
				while (i < doc_.size()) {
					page_object newpage;
					
					int idx = 0;
					idx = this->xcontents(out, pm, i);
					if (idx < 0) throw std::runtime_error("something was happened in the contents object");
					newpage.contents(idx);
					this->xput_font_objects(out, pm);
					idx = this->xresources(out, pm);
					if (idx < 0) throw std::runtime_error("something was happened in the contents object");
					newpage.resources(idx);
					
					newpage.property(pp_);
					if (!newpage(out, pm)) throw std::runtime_error("something was happened in the page object");
				}
				
				return true;
			}
			
		private:
			typedef clx::basic_format<CharT, Traits> fmt;
			typedef clx::basic_unzip<CharT, Traits> storage_type;
			typedef faml::docx::basic_document<CharT, Traits> doc_type;
			typedef basic_font_property<CharT, Traits> font_property;
			
			// contents
			typedef basic_text_contents<CharT, Traits> text_contents;
			typedef basic_image_contents<CharT, Traits> image_contents;
			typedef contents::basic_table<CharT, Traits> table_contents;
			typedef contents::basic_shape<CharT, Traits> shape_contents;
			typedef clx::shared_ptr<shape_contents> shape_ptr;
			
			// member variables
			doc_type doc_;
			storage_type in_;
			page_property pp_;
			font_map f_;
			double space_;
			double baseline_;
			
			/* ------------------------------------------------------------- */
			//  xobject
			/* ------------------------------------------------------------- */
			struct xobj_type {
				int index;
				string_type label;
				string_type path;
			};
			std::deque<xobj_type> objs_;
			
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
				double margin_top;
				double margin_bottom;
				double margin_left;
				double margin_right;
				
				font_map fonts;
				std::set<string_type> fused;
				size_type para;
				size_type tbl;
				std::deque<xobj_type> objs;
				std::vector<size_type> lists;
				
				size_type txt;
				size_type shp;
				size_type img;
				size_type cht;
				int interrupt;
				bool head;
				
				current_status() :
					x(0), y(0), width(0), height(0), fonts(), fused(),
					para(0), tbl(0), objs(), lists(),
					txt(0), shp(0), img(0), cht(0), interrupt(-1), head(true) {}
			};
			current_status cur_;
			
			/* ------------------------------------------------------------- */
			//  xinit
			/* ------------------------------------------------------------- */
			void xinit(const string_type& path) {
				string_type latin(LITERAL(DOCX_DEFAULT_LATIN));
				string_type japan(LITERAL(DOCX_DEFAULT_JAPAN));
				
				font_object f(latin, charset::utf16);
				font_object fj(japan, charset::utf16);
				
				f_[latin] = f;
				f_[japan] = fj;
				cur_.fused.insert(latin);
				cur_.fused.insert(japan);
				
				if (!in_.open(path)) throw std::runtime_error("cannot find docx file");
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
			//  xobjects
			/* ------------------------------------------------------------- */
			template <class OutStream, class PDFManager, class Source>
			bool xobjects(OutStream& out, PDFManager& pm, const Source& src) {
				for (size_type i = 0; i < src.size(); ++i) {
					for (size_type j = 0; j < src.at(i).images().size(); ++j) {
						string_type ref = src.at(i).images().at(j).reference();
						if (ref.empty() || ref.compare(0, 5, LITERAL("chart")) == 0) continue;
						string_type ext = ref.substr(ref.find_last_of(LITERAL(".")));
						if (ext == LITERAL(".wmf") || ext == LITERAL(".emf")) continue;
						
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
							if (!png(out, pm)) throw std::runtime_error("error is occured in png resource.");
							elem.index = png.index();
							elem.label = png.label();
						}
						else continue;
						
						objs_.push_back(elem);
					}
				}
				
				return true;
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
			/*
			 *  xcontents
			 *
			 *  Output some information about contents object (e.g.,
			 *  stream length, filter information, and contents stream).
			 */
			/* ------------------------------------------------------------- */
			template <class OutStream, class PDFManager>
			int xcontents(OutStream& out, PDFManager& pm, size_type& pos) {
				//cur_.fused.clear();
				std::basic_stringstream<CharT, Traits> ss;
				string_type filter;
#ifdef FAML_USE_ZLIB
				filter = LITERAL("/FlateDecode");
				clx::basic_zstream<Z_DEFAULT_COMPRESSION, CharT, Traits> z(ss);
				if (!this->xputstream(ss, pos)) return -1;
				z.finish();
#else
				if (!this->xputstream(ss, pos)) return -1;
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
			//  xputstream
			/* ------------------------------------------------------------- */
			template <class OutStream>
			bool xputstream(OutStream& out, size_type& pos) {
				this->xsetpp();
				
				cur_.x = pp_.margin().left();
				cur_.y = cur_.height - twip(doc_.margin_header());
				cur_.width = pp_.width();
				cur_.height = pp_.height();
				/*
				cur_.margin_left = pp_.margin().left();
				cur_.margin_right = pp_.margin().right();
				cur_.margin_top = pp_.margin().top();
				cur_.margin_bottom = pp_.margin().bottom();
				*/
				this->xput_container(out, doc_.header());
				
				while (pos < doc_.size()) {
					bool status = true;
					switch (doc_.at(pos)) {
					case doc_type::text:
						status = this->xputparagraph(out, pos);
						break;
					case doc_type::table:
						status = this->xputtable(out, pos);
						break;
					default:
						break;
					}
					
					if (!status || cur_.y - baseline_ < pp_.margin().bottom()) break;
				}
				
				cur_.x = pp_.margin().left();
				cur_.y = twip(doc_.margin_footer()) + twip(doc_.margin_bottom());
				cur_.width = pp_.width();
				cur_.height = pp_.height();
				/*
				cur_.margin_left = pp_.margin().left();
				cur_.margin_right = pp_.margin().right();
				cur_.margin_top = pp_.margin().top();
				cur_.margin_bottom = pp_.margin().bottom();
				*/
				this->xput_container(out, doc_.footer());
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xput_container
			/* ------------------------------------------------------------- */
			template <class OutStream, class Source>
			bool xput_container(OutStream& out, const Source& src) {
				typedef typename Source::value_type paragraph;
				for (size_type i = 0; i < src.size(); ++i) {
					const paragraph& v = src.at(i);
					
					double x = pp_.margin().left();
					double y = cur_.y;
					double w = cur_.width;
					double h = cur_.height;
					
					size_type txt = 0, shp = 0, img = 0, cht = 0; // indexes
					babel::init_babel();
					
					for (size_type j = 0; j < v.size(); ++j) {
						switch (v.at(j)) {
						case 0: // text
						{
							if (txt >= v.texts().size()) throw std::runtime_error("luck of text data");
							
							if (v.texts().at(txt).data().empty()) {
								if (v.texts().at(txt).tab() > 0.0) {
									cur_.x += twip(v.texts().at(txt).tab());
								}
								++txt;
								continue;
							}
							
							//string_type s = v.texts().at(txt).data();
							this->xput_text(out, v, txt);
							//cur_.y -= v.font().size() * baseline_;
							cur_.y -= baseline_;
							++txt;
							
							break;
						}
						case 1: // shape
							if (shp >= v.drawings().size()) throw std::runtime_error("luck of shape data");
							this->xput_drawing(out, v.drawings(), shp, x, y);
							++shp;
							break;
						case 2: // image
						{
							if (img >= v.images().size()) throw std::runtime_error("luck of image data");
							//double ox = (v.images().at(img).position() == 0) ? cur_.x : x;
							double ox = x;
							double oy = (v.images().at(img).position() == 0) ? cur_.y : y;
							this->xput_image(out, v, img, ox, oy);
							++img;
							break;
						}
						case 3: // chart
							if (img >= v.images().size() ||
								cht >= v.charts().size()) throw std::runtime_error("luck of chart data");
							cur_.width = emu(v.images().at(img).width());
							cur_.height = emu(v.images().at(img).height());
							this->xput_chart(out, v.charts(), cht, x, y);
							++img;
							++cht;
							break;
						default:
							break;
						}
					}
					
					cur_.x = cur_.margin_left;
					cur_.width = w;
					cur_.height = h;
				}
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xputparagraph
			/* ------------------------------------------------------------- */
			template <class OutStream>
			bool xputparagraph(OutStream& out, size_type& pos) {
				typedef typename doc_type::paragraph paragraph;
				if (cur_.para >= doc_.texts().size()) throw std::runtime_error("wrong text index");
				
				const paragraph& v = doc_.texts().at(cur_.para);
				cur_.y -= v.top_margin();
				
				double x = pp_.margin().left();
				double y = cur_.y;
				double w = cur_.width;
				double h = cur_.height;
				
				if (cur_.interrupt < 0) {
					cur_.txt = 0;
					cur_.shp = 0;
					cur_.img = 0;
					cur_.cht = 0;
					if (v.baseline() > 0.0) baseline_ = v.baseline();
				}
				
				babel::init_babel();
				this->xmove_horizon(v, cur_.txt, string_type());
				size_type i = (cur_.interrupt >= 0) ? cur_.interrupt : 0;
				double extsz = 0.0;
				for (; i < v.size(); ++i) {
					bool newpage = false;
					switch (v.at(i)) {
					case 0: // text
					{
						if (cur_.txt >= v.texts().size()) throw std::runtime_error("luck of text data");
						
						if (v.texts().at(cur_.txt).data().empty()) {
							if (v.texts().at(cur_.txt).tab() > 0.0) {
								if (cur_.head) cur_.x += twip(v.texts().at(cur_.txt).tab());
								else cur_.x += v.font().size();
							}
							++cur_.txt;
							continue;
						}
						
						string_type s = v.texts().at(cur_.txt).data();
						if ((s.size() >= 1 && s.at(s.size() - 1) == 0x0c)) newpage = true;
						
						this->xput_text(out, v, cur_.txt);
						if ((v.texts().at(cur_.txt).decorate() & text_contents::ruby)) {
							extsz = v.texts().at(cur_.txt).extf().size();
						}
						++cur_.txt;
						
						break;
					}
					case 1: // shape
						if (cur_.shp >= v.drawings().size()) throw std::runtime_error("luck of shape data");
						this->xput_drawing(out, v.drawings(), cur_.shp, x, y);
						++cur_.shp;
						break;
					case 2: // image
					{
						//double ox = (v.images().at(cur_.img).position() == 0) ? cur_.x : x;
						double ox = x;
						double oy = (v.images().at(cur_.img).position() == 0) ? cur_.y : y;
						if (cur_.img >= v.images().size()) throw std::runtime_error("luck of image data");
						this->xput_image(out, v, cur_.img, ox, oy);
						++cur_.img;
						break;
					}
					case 3: // chart
						if (cur_.img >= v.images().size() ||
							cur_.cht >= v.charts().size()) throw std::runtime_error("luck of chart data");
						cur_.width = emu(v.images().at(cur_.img).width());
						cur_.height = emu(v.images().at(cur_.img).height());
						this->xput_chart(out, v.charts(), cur_.cht, x, y);
						++cur_.img;
						++cur_.cht;
						break;
					default:
						break;
					}
					
					if (cur_.y < pp_.margin().bottom()) newpage = true;
					if (newpage) {
						cur_.interrupt = i + 1;
						return false;
					}
				}
				
				double vsp = baseline_;
				while (v.font().size() + extsz > vsp) vsp += baseline_;
				cur_.y -= vsp;
				//cur_.y -= (v.font().size() + extsz) * baseline_;
				cur_.y -= v.bottom_margin();
				cur_.x = cur_.margin_left;
				cur_.width = w;
				cur_.height = h;
				cur_.interrupt = -1;
				++cur_.para;
				++pos;
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xmove_horizon
			/* ------------------------------------------------------------- */
			template <class Source>
			bool xmove_horizon(const Source& src, size_type pos, const string_type& rest) {
				cur_.head = true;
				double width = 0.0;
				if (!rest.empty()) {
					if (pos == 0) throw std::runtime_error("wrong text index");
					
					string_type latin = src.texts().at(pos - 1).font().latin();
					if (f_.find(latin) == f_.end()) throw std::runtime_error("cannof find font");
					text_helper h(f_[latin].property(), charset::utf16);

					string_type japan = src.texts().at(pos - 1).font().japan();
					if (f_.find(japan) == f_.end()) throw std::runtime_error("cannof find font");
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
					width += (w / 1000.0)
						* src.texts().at(pos - 1).font().size()
						* (src.texts().at(pos - 1).scale() / 100.0)
						+ twip(src.texts().at(pos - 1).tab());
					
					double indent = twip(src.indent());
					double rindent = twip(src.rindent());
					double space = cur_.width - cur_.margin_left - cur_.margin_right - width;
					if (space - indent - rindent < 0) {
						cur_.x = cur_.margin_left + indent;
						return true;
					}
					
					if (newline) {
						if (src.align() == 1) cur_.x = cur_.margin_left + space / 2.0;
						else if (src.align() == 2) cur_.x = cur_.margin_left + space - rindent;
						else cur_.x = cur_.margin_left + indent;
						return true;
					}
				}
				
				double indent = twip(src.indent());
				double rindent = twip(src.rindent());
				if (pos == 0) indent += twip(src.indent1st());
				
				for (size_type i = pos; i < src.texts().size(); ++i) {
					string_type latin = src.texts().at(i).font().latin();
					if (f_.find(latin) == f_.end()) {
						font_object f(latin, charset::utf16);
						f_[latin] = f;
						cur_.fonts[latin] = f;
						cur_.fused.insert(latin);
					}
					text_helper h(f_[latin].property(), charset::utf16);
					
					string_type japan = src.texts().at(i).font().japan();
					if (f_.find(japan) == f_.end()) {
						font_object f(japan, charset::utf16);
						f_[japan] = f;
						cur_.fonts[japan] = f;
						cur_.fused.insert(japan);
					}
					text_helper hj(f_[japan].property(), charset::utf16);
					
					std::basic_string<wchar_t> tmp = babel::utf8_to_unicode(src.texts().at(i).data());
					string_type s = to_narrow(tmp.begin(), tmp.end());
					//string_type s = win32conv(tmp.begin(), tmp.end());
					bool newline = false;
					if (s.size() >= 2 && s.at(s.size() - 2) == 0x00 &&
						(s.at(s.size() - 1) == 0x0a || s.at(s.size() - 1) == 0x0d || s.at(s.size() - 1) == 0x0c)) {
						s.erase(s.size() - 2);
						newline = true;
					}
					
					double w = (is_ascii(s.begin(), s.end(), charset::utf16))
						? h.width(s.begin(), s.end())
						: hj.width(s.begin(), s.end());
					width += (w / 1000.0)
						* src.texts().at(i).font().size()
						* (src.texts().at(i).scale() / 100.0)
						+ twip(src.texts().at(i).tab());
					
					double space = cur_.width - cur_.margin_left - cur_.margin_right - width;
					if (space - indent - rindent < 0.0) {
						cur_.x = cur_.margin_left + indent;
						return true;
					}
					
					if (newline) {
						if (src.align() == 1) cur_.x = cur_.margin_left + space / 2.0;
						else if (src.align() == 2) cur_.x = cur_.margin_left + space - rindent;
						else cur_.x = cur_.margin_left + indent;
						return true;
					}
				}
				
				double space = cur_.width - cur_.margin_left - cur_.margin_right - width;
				if (src.align() == 1) cur_.x = cur_.margin_left + space / 2.0;
				else if (src.align() == 2) cur_.x = cur_.margin_left + space - rindent;
				else cur_.x = cur_.margin_left + indent;
				
				return true;
			}
			
			
			/* ------------------------------------------------------------- */
			//  xput_text
			/* ------------------------------------------------------------- */
			template <class OutStream, class Source>
			bool xput_text(OutStream& out, const Source& src, size_type pos) {
				if (pos == 0 && !src.bullet().empty()) this->xput_bullet(out, src);
				
				clx::logger::trace(TRACEF("Font: %s, %s",
					src.texts().at(pos).font().latin().c_str(),
					src.texts().at(pos).font().japan().c_str()));
				clx::logger::trace(TRACEF("Text: %s\n", src.texts().at(pos).data().c_str()));
				
				std::basic_string<wchar_t> tmp = babel::utf8_to_unicode(src.texts().at(pos).data());
				string_type s = to_narrow(tmp.begin(), tmp.end());
				
				string_type latin = src.texts().at(pos).font().latin();
				if (f_.find(latin) == f_.end()) {
					font_object f(latin, charset::utf16);
					f_[latin] = f;
					cur_.fonts[latin] = f;
					cur_.fused.insert(latin);
				}
				text_helper h(f_[latin].property(), charset::utf16);
				
				string_type japan = src.texts().at(pos).font().japan();
				if (f_.find(japan) == f_.end()) {
					font_object f(japan, charset::utf16);
					f_[japan] = f;
					cur_.fonts[japan] = f;
					cur_.fused.insert(japan);
				}
				text_helper hj(f_[japan].property(), charset::utf16);
				
				typename string_type::iterator it = s.begin();
				
				bool newline = false;
				if (s.size() >= 2 && s.at(s.size() - 2) == 0x00 &&
					(s.at(s.size() - 1) == 0x0a || s.at(s.size() - 1) == 0x0d || s.at(s.size() - 1) == 0x0c)) {
					newline = true;
					s.erase(s.size() - 2);
				}
				
				double rindent = twip(src.rindent());
				while (1) {
					if (src.texts().at(pos).tab() > 0.0) {
						if (cur_.head) cur_.x += twip(src.texts().at(pos).tab());
						else cur_.x += src.font().size();
					}
					
					double size = src.texts().at(pos).font().size();
					if ((src.texts().at(pos).decorate() & text_contents::ruby)) {
						size += src.texts().at(pos).extf().size();
					}
					double vsp = baseline_;
					while (vsp < size) vsp += baseline_;
					
					string_type line;
					double width = cur_.width - cur_.x - cur_.margin_right - rindent;
					int limit = static_cast<int>(width / (src.texts().at(pos).font().size()
						* src.texts().at(pos).scale() / 100.0) * 1000);
					if (limit < 500) {
						//cur_.y -= size * baseline_;
						cur_.y -= vsp;
						this->xmove_horizon(src, pos + 1, s);
						//continue;
					}
					int used = is_ascii(it, s.end(), charset::utf16)
						? h.getline(it, s.end(), std::inserter(line, line.end()), limit)
						: hj.getline(it, s.end(), std::inserter(line, line.end()), limit);
					
					double n = is_ascii(line.begin(), line.end(), charset::utf16)
						? h.width(line.begin(), line.end()) / 1000.0
						: hj.width(line.begin(), line.end()) / 1000.0;
					if (line.empty()) break;
					
					size_type deco = src.texts().at(pos).decorate();
					size_type rgb = src.texts().at(pos).font().rgb();
					if ((deco & text_contents::hyperlink)) {
						deco |= text_contents::underline;
						rgb = 0x0000ff;
					}
					
					//double top = size * ((baseline_ - 1.0) / 2.0);
					double top = (vsp - size) / 2.0;
					text_contents txt(line, coordinate(cur_.x, cur_.y - top),
						f_[latin], f_[japan]);
					txt.font_size(src.texts().at(pos).font().size());
					txt.font_color(color(rgb));
					txt.decorate(deco);
					if ((deco & text_contents::highlight)) {
						txt.background(color(src.texts().at(pos).background()));
					}
					
					if ((deco & text_contents::ruby) && !src.texts().at(pos).ext().empty()) {
						tmp = babel::utf8_to_unicode(src.texts().at(pos).ext());
						string_type ext = to_narrow(tmp.begin(), tmp.end());
						txt.ext(ext);
						txt.extsize(src.texts().at(pos).extf().size());
					}
					
					if ((deco & text_contents::overlap) && !src.texts().at(pos).ext().empty()) {
						tmp = babel::utf8_to_unicode(src.texts().at(pos).ext());
						string_type ext = to_narrow(tmp.begin(), tmp.end());
						txt.ext(ext);
						txt.extsize(src.texts().at(pos).extf().size());
					}
					
					txt.scale(src.texts().at(pos).scale());
					double sp = (limit - used) / 1000.0 * src.texts().at(pos).font().size();
					if (it != s.end() && sp < 0.0) txt.space(sp);
					else if (twip(src.texts().at(pos).space()) > 0) {
						sp = twip(src.texts().at(pos).space())
							* (h.count(line.begin(), line.end()) - 1) * 2;
						txt.space(sp);
					}
					txt(out);
					cur_.head = false;
					
					// Memo: テスト結果から w:spacing の値が本来の半分になっているように思える．
					if (it == s.end()) {
						cur_.x += n * src.texts().at(pos).font().size()
							* (src.texts().at(pos).scale() / 100.0)
							+ twip(src.texts().at(pos).space())
							* h.count(line.begin(), line.end()) * 2;
						break;
					}
					
					line.erase();
					std::copy(it, s.end(), std::inserter(line, line.end()));
					//cur_.y -= size * baseline_;
					cur_.y -= vsp;
					this->xmove_horizon(src, pos + 1, line);
				}
				
				if (newline) {
					double size = src.texts().at(pos).font().size();
					if ((src.texts().at(pos).decorate() & text_contents::ruby)) {
						size += src.texts().at(pos).extf().size();
					}
					
					double vsp = baseline_;
					while (vsp < size) vsp += baseline_;
					cur_.y -= vsp;
					//cur_.y -= size * baseline_;
					this->xmove_horizon(src, pos + 1, string_type());
				}
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xput_bullet
			/* ------------------------------------------------------------- */
			template <class OutStream, class Source>
			bool xput_bullet(OutStream& out, const Source& src) {
				while (cur_.lists.size() > src.list_level() + 1) cur_.lists.pop_back();
				if (cur_.lists.size() <= src.list_level()) cur_.lists.push_back(0);
				
				string_type s = src.bullet();
				if (src.list_type() == 2) { // ol
					s = src.bullet();
					s.at(s.size() - 1) += cur_.lists.back();
					++cur_.lists.back();
					std::basic_stringstream<CharT, Traits> ss;
					ss << fmt(src.bufmt()) % s;
					s = ss.str();
				}
				
				std::basic_string<wchar_t> tmp = babel::utf8_to_unicode(s);
				s = to_narrow(tmp.begin(), tmp.end());
				//s = win32conv(tmp.begin(), tmp.end());
				
				double x = cur_.x - twip(src.list_margin());
				
				text_contents txt(s, coordinate(x, cur_.y),
					f_[DOCX_DEFAULT_LATIN], f_[DOCX_DEFAULT_JAPAN]);
				txt.font_size(src.font().size());
				txt.font_color(color(src.font().rgb()));
				txt(out);
				
				return true;
			}
			
			
			/* ------------------------------------------------------------- */
			//  xput_drawing
			/* ------------------------------------------------------------- */
			template <class OutStream, class Container>
			bool xput_drawing(OutStream& out, const Container& src, size_type pos, double ox, double oy) {
				typedef typename doc_type::paragraph paragraph;
				
				double x = cur_.x;
				double y = cur_.y;
				double w = cur_.width;
				double h = cur_.height;
				double t = cur_.margin_top;
				double b = cur_.margin_bottom;
				double l = cur_.margin_left;
				double r = cur_.margin_right;
				
				cur_.margin_top = 0;
				cur_.margin_bottom = 0;
				cur_.margin_left = ox + src.at(pos).origin().x();
				cur_.margin_right = 0;
				cur_.x = ox + src.at(pos).origin().x() - 2.0;
				cur_.y = oy - src.at(pos).origin().y();
				cur_.width = src.at(pos).width() + src.at(pos).weight() * 2 + 12.0;
				cur_.height = src.at(pos).height();
				
				// put shape.
				typedef clx::shared_ptr<contents::basic_shape<CharT, Traits> > shape_ptr;
				shape_ptr sh = contents::make_msshape<CharT, Traits>(src.at(pos).type(),
					coordinate(cur_.x, cur_.y - cur_.height), cur_.width, cur_.height);
				if (!sh) return false;
				
#if 1
				color clr = color(src.at(pos).border());
				if (src.at(pos).border() > 0x00ffffff) clr.invalidate();
#else
				color clr = color(0);
#endif
				sh->border(clr);
				if (src.at(pos).border_type() > 0) sh->type(src.at(pos).border_type());	
				
				clr = color(src.at(pos).background());
				if (src.at(pos).background() > 0x00ffffff) clr.invalidate();
				sh->background(clr);
				sh->weight(src.at(pos).weight());
				
#ifdef FAML_VALID_ADJUST
				typedef typename Container::value_type::adjust_map::const_iterator adj_iterator;
				for (adj_iterator it = src.at(pos).adjusts().begin();
					it != src.at(pos).adjusts().end(); ++it) {
					sh->adjust(shu(it->second), it->first);
				}
#endif
				(*sh)(out);
				
				cur_.y -= src.at(pos).weight() + 5.0;
				cur_.width += cur_.margin_left; // compatible for other functions.
				for (size_type i = 0; i < src.at(pos).texts().size(); ++i) {
					const paragraph& v = src.at(pos).texts().at(i);
					if (v.baseline() > 0.0) baseline_ = v.baseline();
					babel::init_babel();
					this->xmove_horizon(v, 0, string_type());
					for (size_type j = 0; j < v.size(); ++j) {
						if (j >= v.texts().size()) throw std::runtime_error("luck of text data");
						
						if (v.texts().at(j).data().empty()) {
							if (v.texts().at(j).tab() > 0.0) {
								if (cur_.head) cur_.x += twip(v.texts().at(cur_.txt).tab());
								//else cur_.x += v.font().size();
							}
							continue;
						}
						
						this->xput_text(out, v, j);
					}
					
					double vsp = baseline_;
					while (vsp < v.font().size()) vsp += baseline_;
					cur_.y -= vsp;
					//cur_.y -= v.font().size() * baseline_;
					cur_.x = cur_.margin_left;
				}
				
				cur_.x = x;
				cur_.y = y;
				cur_.width = w;
				cur_.height = h;
				cur_.margin_top = t;
				cur_.margin_bottom = b;
				cur_.margin_left = l;
				cur_.margin_right = r;
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xput_chart
			/* ------------------------------------------------------------- */
			template <class OutStream, class Container>
			bool xput_chart(OutStream& out, const Container& src, size_type pos, double ox, double oy) {
				typedef typename Container::value_type chart_type;
				
				switch (src.at(pos).type()) {
				case 0:
				{
					contents::barchart<chart_type> ch(coordinate(ox, oy - cur_.height), cur_.width, cur_.height);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				case 1:
				{
					contents::rowchart<chart_type> ch(coordinate(ox, oy - cur_.height), cur_.width, cur_.height);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				case 2:
				{
					contents::linechart<chart_type> ch(coordinate(ox, oy - cur_.height), cur_.width, cur_.height);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				case 4:
				{
					contents::areachart<chart_type> ch(coordinate(ox, oy - cur_.height), cur_.width, cur_.height);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				case 5:
				{
					contents::scatterchart<chart_type> ch(coordinate(ox, oy - cur_.height), cur_.width, cur_.height);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				case 6:
				{
					contents::bar3dchart_box<chart_type> ch(coordinate(ox, oy - cur_.height), cur_.width, cur_.height);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				case 7:
				{
					contents::bar3dchart_cylinder<chart_type> ch(coordinate(ox, oy - cur_.height), cur_.width, cur_.height);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				case 8:
				{
					contents::bar3dchart_cone<chart_type> ch(coordinate(ox, oy - cur_.height), cur_.width, cur_.height);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				case 9:
				{
					contents::bar3dchart_pyramid<chart_type> ch(coordinate(ox, oy - cur_.height), cur_.width, cur_.height);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				case 10:
				{
					contents::row3dchart_box<chart_type> ch(coordinate(ox, oy - cur_.height), cur_.width, cur_.height);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				case 18:
				{
					contents::pie3dchart<chart_type> ch(coordinate(ox, oy - cur_.height), cur_.width, cur_.height);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				case 19:
				{
					contents::pie3dchart_explosion<chart_type> ch(coordinate(ox, oy - cur_.height), cur_.width, cur_.height);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				default:
				{
					contents::barchart<chart_type> ch(coordinate(ox, oy - cur_.height), cur_.width, cur_.height);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				}
				
				cur_.y -= cur_.width;
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xputchart
			/* ------------------------------------------------------------- */
			template <class OutStream, class Container, class Type>
			bool xputchart_exec(OutStream& out, const Container& src, size_type pos, Type& ch) {
				ch.font1st(f_[DOCX_DEFAULT_LATIN]);
				ch.font2nd(f_[DOCX_DEFAULT_JAPAN]);
				if (src.at(pos).border().first) {
					ch.border(color(src.at(pos).border().second));
				}
				
				if (src.at(pos).background().first) {
					ch.background(color(src.at(pos).background().second));
				}
				
				if (src.at(pos).plot_border().first) {
					ch.plot_border(color(src.at(pos).plot_border().second));
				}
				
				if (src.at(pos).plot_area().first) {
					ch.plot_area(color(src.at(pos).plot_area().second));
				}
				
				ch.data(src.at(pos));
				if (doc_.theme()) ch.palette(doc_.theme()->palette().data());
				ch(out);
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xput_image
			/* ------------------------------------------------------------- */
			template <class OutStream, class Container>
			bool xput_image(OutStream& out, const Container& src, size_type pos, double ox, double oy) {
				double x = ox + src.images().at(pos).offset().x();
				double y = oy - src.images().at(pos).offset().y();
				double w = src.images().at(pos).width();
				double h = src.images().at(pos).height();
				
				if (src.align() == 1) {
					x += std::max(0.0, (cur_.width - cur_.margin_left - cur_.margin_right - w) / 2.0);
				}
				else if (src.align() == 2) {
					x += std::max(0.0, (cur_.width - cur_.margin_left - cur_.margin_right - w));
				}
				
				typedef typename std::deque<xobj_type>::iterator iter;
				for (iter it = objs_.begin(); it != objs_.end(); ++it) {
					if (it->path == src.images().at(pos).reference()) {
						image_contents image(it->label, coordinate(x, y - h), w, h);
						image(out);
						cur_.objs.push_back(*it);
						//objs_.erase(it);
						break;
					}
				}
				
				string_type ext = src.images().at(pos).reference().substr(
					src.images().at(pos).reference().find_last_of(LITERAL(".")));
				if (ext == LITERAL(".wmf")) {
					typename storage_type::iterator it = in_.find(src.images().at(pos).reference());
					if (it == in_.end()) throw std::runtime_error("cannot find image file");
					std::vector<char_type> s;
					clx::read(*it, s);
					clx::basic_ivstream<CharT, Traits> vs(s);
					
					contents::basic_wmf<CharT, Traits> wmf(vs);
					try {
						wmf.read_header();
					}
					catch (std::runtime_error&) {}
					wmf.origin(coordinate(x, y - h));
					wmf.width(w);
					wmf.height(h);
					wmf.font1st(f_[DOCX_DEFAULT_LATIN]);
					wmf.font2nd(f_[DOCX_DEFAULT_JAPAN]);
					wmf(out);
				}
				
				if (src.images().at(pos).position() == 0) cur_.y -= h;
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xputtable
			/* ------------------------------------------------------------- */
			template <class OutStream>
			bool xputtable(OutStream& out, size_type& pos) {
				typedef typename doc_type::txtable txtable;
				if (cur_.tbl >= doc_.tables().size()) throw std::runtime_error("wrong table index");
				const txtable& v = doc_.tables().at(cur_.tbl);
				++pos;
				++cur_.tbl;
				
				table_contents tbl;
				tbl.origin() = coordinate(cur_.x - space_, cur_.y);
				std::basic_stringstream<CharT, Traits> ss; // stored text.
				
				double w = cur_.width;
				double l = cur_.margin_left;
				double r = cur_.margin_right;
				cur_.margin_left = cur_.x;
				cur_.margin_right = 0;
				for (size_type row = 0; row < v.size(); ++row) {
					double height = 0.0;
					for (size_type col = 0; col < v.at(row).size(); ++col) {
						// 1. regist the cells information.
						double width = twip(v.at(row).at(col).width());
						double x = cur_.x;
						double y = cur_.y;
						cur_.width = width + cur_.margin_left; // compatible for other functions.
						
						// 2. output text.
						double h = twip(v.at(row).at(col).height());
						if (h > height) height = h;
						if (!v.at(row).at(col).empty()) this->xputcell(ss, v.at(row).at(col));
						if (y - cur_.y > height) height = y - cur_.y;
						
						table_cell c(width, height);
						c.background() = color(v.at(row).at(col).rgb());
						if (v.at(row).at(col).rgb() >= 0xffffff) c.background().invalidate();
						c.top() = v.top();
						c.bottom() = v.bottom();
						c.left() = v.left();
						c.right() = v.right();
						tbl.add(c);
						
						cur_.x = x + width;
						cur_.margin_left = cur_.x;
						cur_.y = y;
					}
					
					cur_.x = pp_.margin().left();
					cur_.margin_left = cur_.x;
					cur_.y -= height;
					tbl.newline();
				}
				cur_.width = w;
				cur_.margin_left = l;
				cur_.margin_right = r;
				
				// finalize
				tbl(out);
				out << ss.str();
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xputcell
			/* ------------------------------------------------------------- */
			template <class OutStream, class CellT>
			bool xputcell(OutStream& out, const CellT& src) {
				typedef typename doc_type::paragraph paragraph;
				double x = cur_.x;
				for (size_type i = 0; i < src.size(); ++i) {
					const paragraph& p = src.at(i);
					if (p.empty()) {
						cur_.x = x;
						cur_.y -= p.font().size();
					}
					
					size_type txt = 0;
					size_type shp = 0;
					this->xmove_horizon(p, 0, string_type());
					for (size_type j = 0; j < p.size(); ++j) {
						switch (p.at(j)) {
						case 0: // text
							if (txt >= p.texts().size()) throw std::runtime_error("luck of text data");
							if (p.texts().at(txt).data().empty()) {
								if (p.texts().at(txt).tab() > 0.0) {
									cur_.x += twip(p.texts().at(txt).tab());
								}
								++txt;
								continue;
							}
							
							this->xput_text(out, p, txt);
							++txt;
							break;
						case 1: // shape
							if (shp >= p.drawings().size()) throw std::runtime_error("luck of shape data");
							//this->xput_drawing(out, p.drawings(), shp, x, y);
							this->xput_drawing(out, p.drawings(), shp, cur_.x, cur_.y);
							++shp;
							break;
						default:
							break;
						}
					}
					double vsp = baseline_;
					while (vsp < p.font().size()) vsp += baseline_;
					cur_.y -= vsp;
					//cur_.y -= p.font().size() * baseline_;
				}
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xsetpp
			/* ------------------------------------------------------------- */
			bool xsetpp() {
				pp_.width(twip(doc_.width()));
				pp_.height(twip(doc_.height()));
				pp_.margin(page_margin(
					twip(doc_.margin_top()) + twip(doc_.margin_header()),
					twip(doc_.margin_bottom()) + twip(doc_.margin_footer()),
					std::max(twip(doc_.margin_left()) - 6.0, 0.0),
					std::max(twip(doc_.margin_right()) - 6.0, 0.0)
				));
				
				cur_.width = pp_.width();
				cur_.height = pp_.height();
				cur_.margin_top = pp_.margin().top();
				cur_.margin_bottom = pp_.margin().bottom();
				cur_.margin_left = pp_.margin().left();
				cur_.margin_right = pp_.margin().right();
				cur_.x = pp_.margin().left();
				cur_.y = pp_.height() - pp_.margin().top();
				
				return true;
			}
		};
		
		typedef basic_docx<char> docx;
	}
}

#endif // FAML_PDF_DOCX_H
