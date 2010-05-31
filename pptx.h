#ifndef FAML_PDF_PPTX_H
#define FAML_PDF_PPTX_H

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
#include "clx/utility.h"
#include "clx/vstream.h"
#include "clx/replace.h"

#include "unit.h"
#include "utf8.h"
#include "code_convert.h"
#include "page.h"
#include "holder.h"
#include "shading.h"
#include "alpha.h"
#include "font.h"
#include "font_instance.h"
#include "text_helper.h"

#include "contents/shape_factory.h"
#include "contents/custom_shape.h"
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
#include "image_contents.h"

#include "pptx/document.h"

//#define DOCX_DEFAULT_LATIN "Caribli"
#define PPTX_DEFAULT_LATIN "Arial"
#define PPTX_DEFAULT_JAPAN "ＭＳ Ｐゴシック"
#define PPTX_DEFAULT_MONO  "ＭＳ ゴシック"
#define PPTX_FONT_SCALE 1.0

namespace faml {
	namespace pdf {
		/* ----------------------------------------------------------------- */
		//  basic_pptx
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_pptx {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			
			typedef basic_page<CharT, Traits> page_object;
			typedef basic_font<CharT, Traits> font_object;
			typedef std::map<string_type, font_object> font_map;
			
			basic_pptx(const string_type& path, font_factory& ff) :
				ff_(ff), doc_(path), in_(), pp_(slide::width, slide::height), f_(),
				baseline_(1.2), wordsp_(0.0) {
				this->xinit(path);
			}
			
			template <class OutStream, class PDFManager>
			bool operator()(OutStream& out, PDFManager& pm) {
				// currently implementation
				for (typename font_map::iterator pos = f_.begin(); pos != f_.end(); ++pos) {
					(pos->second)(out, pm);
				}
				
				for (size_type i = 0; i < doc_.size(); ++i) {
					cur_.page = i;
					clx::logger::debug(DEBUGF("[SLIDE] %d", i));
					page_object newpage(pp_);
					
					this->xobjects(out, pm, doc_.master().bgimage());
					this->xobjects(out, pm, doc_.at(i).layout());
					this->xobjects(out, pm, doc_.at(i));
					
					int idx = 0;
					idx = this->xcontents(out, pm, doc_.at(i));
					if (idx < 0) throw std::runtime_error("something was happened in the contents object");
					newpage.contents(idx);
					
					if (!cur_.holder.empty()) this->xdelayed_objects(out, pm);
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
			typedef faml::pptx::basic_document<CharT, Traits> doc_type;
			typedef basic_font_property<CharT, Traits> font_property;
			typedef basic_holder<CharT, Traits> holder_type;
			typedef basic_shading<CharT, Traits> shading_type;
			typedef basic_alpha<CharT, Traits> alpha_type;
			typedef basic_piece<CharT, Traits> piece_type;
			
			// contents
			typedef basic_text_contents<CharT, Traits> text_contents;
			typedef basic_image_contents<CharT, Traits> image_contents;
			typedef contents::basic_table<CharT, Traits> table_contents;
			typedef contents::basic_shape<CharT, Traits> shape_contents;
			typedef clx::shared_ptr<shape_contents> shape_ptr;
			
			// member variables
			font_factory& ff_;
			doc_type doc_;
			storage_type in_;
			page_property pp_;
			font_map f_;
			double baseline_;
			double wordsp_;
			
			
			/* ------------------------------------------------------------- */
			//  xobject
			/* ------------------------------------------------------------- */
			struct xobj_type {
				int index;
				string_type label;
				string_type path;
				double width;
				double worigin;
				double height;
				double horigin;
				
				xobj_type() : index(0), label(), path(),
					width(0.0), worigin(0.0), height(0.0), horigin(0.0) {}
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
				double hpad;
				double vpad;
				size_type page;
				double max_font_size;
				std::set<string_type> fused; // used font list.
				font_map fonts;
				std::vector<string_type> labels;
				std::vector<piece_type> pieces;
				std::deque<xobj_type> objs;
				std::deque<xobj_type> shadings;
				std::deque<xobj_type> alphas;
				holder_type holder;
				
				current_status() :
					x(0), y(0), width(0), height(0),
					hpad(0.0), vpad(0.0), page(0), max_font_size(0.0),
					fused(), fonts(), labels(), pieces(), objs(), shadings(), alphas(), holder() {}
			};
			current_status cur_;
			
			/* ------------------------------------------------------------- */
			//  xinit
			/* ------------------------------------------------------------- */
			void xinit(const string_type& path) {
				//string_type latin(LITERAL(PPTX_DEFAULT_LATIN));
				//string_type japan(LITERAL(PPTX_DEFAULT_JAPAN));
				string_type mono(LITERAL(PPTX_DEFAULT_MONO));
				
				pp_.width(static_cast<int>(doc_.width()));
				pp_.height(static_cast<int>(doc_.height()));
				
				//font_object f(latin, charset::utf16);
				//font_object fj(japan, charset::utf16);
				font_object fm(mono, charset::utf16);
				
				//f_[latin] = f;
				//f_[japan] = fj;
				f_[mono] = fm;
				//cur_.fused.insert(latin);
				//cur_.fused.insert(japan);
				cur_.fused.insert(mono);
				
				cur_.hpad = 3.7;
				cur_.vpad = 3.7;
				
				if (!in_.open(path)) throw std::runtime_error("cannot find pptx file");
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
				
				if (!cur_.shadings.empty()) {
					out << LITERAL("/Shading") << std::endl;
					out << LITERAL("<<") << std::endl;
					for (size_type i = 0; i < cur_.shadings.size(); ++i) {
						out << fmt(LITERAL("/%s %d 0 R"))
							% cur_.shadings.at(i).label % cur_.shadings.at(i).index
						<< std::endl;
					}
					out << LITERAL(">>") << std::endl;
					cur_.shadings.clear();
				}
				
				if(!cur_.alphas.empty()) {
					out << LITERAL("/ExtGState") << std::endl;
					out << LITERAL("<<") << std::endl;
					for (size_type i = 0; i < cur_.alphas.size(); ++i) {
						out << fmt(LITERAL("/%s %d 0 R"))
							% cur_.alphas.at(i).label % cur_.alphas.at(i).index
						<< std::endl;
					}
					out << LITERAL(">>") << std::endl;
					cur_.alphas.clear();
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
			template <class OutStream, class PDFManager>
			bool xobjects(OutStream& out, PDFManager& pm, const string_type& ref) {
				if (ref.empty()) return true;
				string_type ext = ref.substr(ref.find_last_of(LITERAL(".")));
				clx::downcase(ext);
				if (ext == LITERAL(".wmf") || ext == LITERAL(".emf")) return false;
				if (objs_.find(ref) != objs_.end()) return false;
				
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
#if 1
					alpha_png_resource<CharT, Traits> png(vs);
					if (!png(out, pm)) throw std::runtime_error("error is occured in png resource.");
					elem.index = png.index();
					elem.label = png.label();
					elem.width = png.width();
					elem.worigin = png.worigin();
					elem.height = png.height();
					elem.horigin = png.horigin();
#else
					png_resource<CharT, Traits> png(vs);
					if (!png(out, pm)) throw std::runtime_error("error is occured in png resource.");
					elem.index = png.index();
					elem.label = png.label();
#endif
				}
				else return false;
				
				objs_[ref] = elem;
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xobjects
			/* ------------------------------------------------------------- */
			template <class OutStream, class PDFManager, class Source>
			bool xobjects(OutStream& out, PDFManager& pm, const Source& src) {
				for (size_type i = 0; i < src.size(); ++i) {
					this->xobjects(out, pm, src.at(i).reference());
					//string_type ref = src.at(i).reference();
				}
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xdelayed_objects
			/* ------------------------------------------------------------- */
			template <class OutStream, class PDFManager>
			bool xdelayed_objects(OutStream& out, PDFManager& pm) {
				cur_.holder(out, pm);
				for (size_type i = 0; i < cur_.holder.size(); ++i) {
					xobj_type elem;
					elem.index = cur_.holder.index(i);
					elem.label = cur_.holder.label(i);
					if (elem.label.find(LITERAL("SH")) != string_type::npos) {
						cur_.shadings.push_back(elem);
					}
					else if (elem.label.find(LITERAL("GS")) != string_type::npos) {
						cur_.alphas.push_back(elem);
					}
				}
				cur_.holder.clear();
				
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
			template <class OutStream, class PDFManager, class SlideT>
			int xcontents(OutStream& out, PDFManager& pm, const SlideT& src) {
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
			//  xput_layout
			/* ------------------------------------------------------------- */
			template <class OutStream, class Source>
			bool xput_layout(OutStream& out, const Source& src) {
				typedef typename Source::value_type::path_container paths_type;
				
				for (size_type i = 0; i < src.size(); ++i) {
					cur_.width = emu(src.at(i).width());
					cur_.height = emu(src.at(i).height());
					cur_.x = emu(src.at(i).origin().x()) + pp_.margin().left();
					cur_.y = pp_.height() - (pp_.margin().top() + emu(src.at(i).origin().y()));
					
					string_type ref = src.at(i).reference();
					
					// put shape.
					shape_ptr sh;
					if (src.at(i).name() == LITERAL("custom")) {
						sh = shape_ptr(new contents::basic_custom_shape<paths_type>(
							coordinate(cur_.x, cur_.y - cur_.height), cur_.width, cur_.height));
						dynamic_cast<contents::basic_custom_shape<paths_type>* >(sh.get())->paths(src.at(i).paths());
					}
					else {
						sh = contents::makeshape(src.at(i).name(),
							coordinate(cur_.x, cur_.y - cur_.height), cur_.width, cur_.height);
					}
					if (!sh) continue;
					
					// Memo: factory を作りなおす．
					if (src.at(i).name() == LITERAL("line") ||
						src.at(i).name() == LITERAL("straightConnector1")) {
						dynamic_cast<contents::basic_line<CharT, Traits>* >(sh.get())->option(src.at(i).option());
					}
					
					color clr = color(src.at(i).border());
					if (src.at(i).border() > 0x00ffffff) clr.invalidate();
					sh->border(clr);
					if (src.at(i).border_type() > 0) sh->type(src.at(i).border_type());
					if (src.at(i).bg_type() == 0x02) {
						shading_type grad;
						grad.add(color(src.at(i).backgrounds().at(0)));
						grad.add(color(src.at(i).backgrounds().at(src.at(i).backgrounds().size() - 1)));
						
						double ox = 0.0;
						double oy = 0.0;
						
						double x = std::cos(clx::radian(src.at(i).direction()));
						if (x < 0.0) {
							ox += 1.0;
							x += 1.0;
						}
						
						double y = std::sin(clx::radian(360 - src.at(i).direction()));
						if (y < 0.0) {
							oy += 1.0;
							y += 1.0;
						}
						
						grad.first(coordinate(ox, oy));
						grad.last(coordinate(x, y));
						cur_.holder.add(grad);
						sh->shading(grad.label());
					}
					else if (src.at(i).bg_type() == 0x01) {
						clr = color(src.at(i).background());
						if (src.at(i).background() > 0x00ffffff) clr.invalidate();
						sh->background(clr);
					}
					sh->angle(clx::radian(src.at(i).angle()));
					sh->flip(src.at(i).flip());
					sh->weight(src.at(i).weight());
					
					typedef typename Source::value_type::adjust_map::const_iterator adj_iterator;
					for (adj_iterator pos = src.at(i).adjusts().begin();
						pos != src.at(i).adjusts().end(); ++pos) {
						sh->adjust(shu2(pos->second), pos->first);
					}
					
					typedef typename Source::value_type::effect_map::const_iterator effect_iterator;
					for (effect_iterator pos = src.at(i).effects().begin();
						pos != src.at(i).effects().end(); ++pos) {
						sh->effect(pos->first, pos->second);
					}
					(*sh)(out);
					
					// put object
					if (!ref.empty()) {
						this->xputimage(out, src.at(i));
						//continue;
					}
					
					// put textbox.
					this->xputtext(out, src.at(i));
				}
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xput_shape
			/* ------------------------------------------------------------- */
			template <class OutStream, class Source>
			bool xput_shape(OutStream& out, const Source& src) {
				typedef typename Source::value_type::path_container paths_type;
				
				size_type ch = 0;
				for (size_type i = 0; i < src.size(); ++i) {
					cur_.width = emu(src.at(i).width());
					cur_.height = emu(src.at(i).height());
					cur_.x = emu(src.at(i).origin().x()) + pp_.margin().left();
					cur_.y = pp_.height() - (pp_.margin().top() + emu(src.at(i).origin().y()));
					
					string_type ref = src.at(i).reference();
					
					// put shape.
					shape_ptr sh;
					if (src.at(i).name() == LITERAL("custom")) {
						sh = shape_ptr(new contents::basic_custom_shape<paths_type>(
							coordinate(cur_.x, cur_.y - cur_.height), cur_.width, cur_.height));
						dynamic_cast<contents::basic_custom_shape<paths_type>* >(sh.get())->paths(src.at(i).paths());
					}
					else {
						sh = contents::makeshape(src.at(i).name(),
							coordinate(cur_.x, cur_.y - cur_.height), cur_.width, cur_.height);
					}
					if (!sh) continue;
					
					// Memo: factory を作りなおす．
					if (src.at(i).name() == LITERAL("line") ||
						src.at(i).name() == LITERAL("straightConnector1")) {
						dynamic_cast<contents::basic_line<CharT, Traits>* >(sh.get())->option(src.at(i).option());
					}
					
					color clr = color(src.at(i).border());
					if (src.at(i).border() > 0x00ffffff) clr.invalidate();
					
					sh->border(clr);
					if (src.at(i).border_type() > 0) sh->type(src.at(i).border_type());
					
					if (src.at(i).bg_type() == 0x02) {
						shading_type grad;
						grad.add(color(src.at(i).backgrounds().at(0)));
						grad.add(color(src.at(i).backgrounds().at(src.at(i).backgrounds().size() - 1)));
						
						double ox = 0.0;
						double oy = 0.0;
						
						// Memo: office の（グラデーションの）デフォルトの方向は，右上から左下？
						double x = std::cos(clx::radian(src.at(i).direction()));
						if (x < 0.0) {
							ox += 1.0;
							x += 1.0;
						}
						
						double y = std::sin(clx::radian(360 - src.at(i).direction()));
						if (y < 0.0) {
							oy += 1.0;
							y += 1.0;
						}
						
						grad.first(coordinate(ox, oy));
						grad.last(coordinate(x, y));
						cur_.holder.add(grad);
						sh->shading(grad.label());
					}
					else if (src.at(i).bg_type() == 0x01) {
						clr = color(src.at(i).background());
						if (src.at(i).background() > 0x00ffffff) clr.invalidate();
						sh->background(clr);
					}
					
					if (src.at(i).alpha() > 0.0) {
						alpha_type alp;
						alp.value(src.at(i).alpha());
						cur_.holder.add(alp);
						sh->alpha(alp.label());
					}
					
					sh->angle(clx::radian(src.at(i).angle()));
					sh->flip(src.at(i).flip());
					sh->weight(src.at(i).weight());
					if (!ref.empty()) sh->weight(src.at(i).weight() + 1.0);
					
					typedef typename Source::value_type::adjust_map::const_iterator adj_iterator;
					for (adj_iterator pos = src.at(i).adjusts().begin();
						pos != src.at(i).adjusts().end(); ++pos) {
						sh->adjust(shu2(pos->second), pos->first);
					}
					
					typedef typename Source::value_type::effect_map::const_iterator effect_iterator;
					for (effect_iterator pos = src.at(i).effects().begin();
						pos != src.at(i).effects().end(); ++pos) {
						sh->effect(pos->first, pos->second);
					}
					
					(*sh)(out);
					
					// put object
					if (!ref.empty()) {
						if (ref.compare(0, 5, LITERAL("chart")) == 0) {
							this->xputchart(out, src, ch, cur_.x, cur_.y, cur_.width, cur_.height);
							++ch;
						}
						else this->xputimage(out, src.at(i));
						//continue;
					}
					
					// put textbox.
					cur_.width = sh->width();
					cur_.x = sh->origin().x();
					this->xputtext(out, src.at(i));
				}
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xputstream
			/* ------------------------------------------------------------- */
			template <class OutStream, class SlideT>
			bool xputstream(OutStream& out, const SlideT& src) {
				//this->xputmaster(out);
				this->xput_background(out, src);
				
				this->xput_layout(out, src.layout());
				this->xput_shape(out, src);
				
				// put table
				for (size_type i = 0; i < src.tbls().size(); ++i) this->xput_table(out, src.tbls().at(i));
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xputchart
			/* ------------------------------------------------------------- */
			template <class OutStream, class SlideT>
			bool xputchart(OutStream& out, const SlideT& src,
				size_type pos, double x, double y, double w, double h) {
				typedef typename SlideT::chart_type chart_type;
				
				switch (src.charts().at(pos).type()) {
				case 0:
				{
					contents::barchart<chart_type> ch(coordinate(x, y - h), w, h);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				case 1:
				{
					contents::rowchart<chart_type> ch(coordinate(x, y - h), w, h);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				case 2:
				{
					contents::linechart<chart_type> ch(coordinate(x, y - h), w, h);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				case 4:
				{
					contents::areachart<chart_type> ch(coordinate(x, y - h), w, h);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				case 5:
				{
					contents::scatterchart<chart_type> ch(coordinate(x, y - h), w, h);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				case 6:
				{
					contents::bar3dchart_box<chart_type> ch(coordinate(x, y - h), w, h);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				case 7:
				{
					contents::bar3dchart_cylinder<chart_type> ch(coordinate(x, y - h), w, h);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				case 8:
				{
					contents::bar3dchart_cone<chart_type> ch(coordinate(x, y - h), w, h);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				case 9:
				{
					contents::bar3dchart_pyramid<chart_type> ch(coordinate(x, y - h), w, h);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				case 10:
				{
					contents::row3dchart_box<chart_type> ch(coordinate(x, y - h), w, h);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				case 18:
				{
					contents::pie3dchart<chart_type> ch(coordinate(x, y - h), w, h);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				case 19:
				{
					contents::pie3dchart_explosion<chart_type> ch(coordinate(x, y - h), w, h);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				default:
				{
					contents::barchart<chart_type> ch(coordinate(x, y - h), w, h);
					this->xputchart_exec(out, src, pos, ch);
					break;
				}
				}
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xputchart
			/* ------------------------------------------------------------- */
			template <class OutStream, class SlideT, class Type>
			bool xputchart_exec(OutStream& out, const SlideT& src, size_type pos, Type& ch) {
				ch.font1st(f_[PPTX_DEFAULT_LATIN]);
				ch.font2nd(f_[PPTX_DEFAULT_JAPAN]);
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
				
				ch.data(src.charts().at(pos));
				if (doc_.theme()) ch.palette(doc_.theme()->palette().data());
				ch(out);
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xset_border
			/* ------------------------------------------------------------- */
			template <class Source, class Type>
			bool xset_border(const Source& src, Type& dest) {
				size_type background = doc_.at(cur_.page).rgbs().empty() ? 0xffffff :
					doc_.at(cur_.page).rgbs().at(0);
				size_type rgb = this->xadjust_color(doc_.theme()->palette().at(0), background);
				
				border ln(color(rgb), src, border::solid);
				dest = ln;
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xput_table
			/* ------------------------------------------------------------- */
			template <class OutStream, class TableT>
			bool xput_table(OutStream& out, const TableT& src) {
				double ox = emu(src.origin().x()) + pp_.margin().left();
				double oy = pp_.height() - (pp_.margin().top() + emu(src.origin().y()));
				
				table_contents tbl;
				tbl.origin() = coordinate(ox, oy);
				std::basic_stringstream<CharT, Traits> ss; // stored text.
				
				cur_.x = ox;
				cur_.y = oy;
				
				double width = cur_.width;
				for (size_type row = 0; row < src.size(); ++row) {
					double height = emu(src.at(row).first);
					cur_.height = height;
					for (size_type col = 0; col < src.at(row).second.size(); ++col) {
						// 1. regist the cells information.
						cur_.width = emu(src.at(row).second.at(col).width());
						double x = cur_.x;
						double y = cur_.y;
						
						// 2. output text.
						if (!src.at(row).second.at(col).empty()) {
							this->xputcell(ss, src.at(row).second.at(col));
						}
						
						if (height < y - cur_.y) height = y - cur_.y;
						table_cell c(cur_.width, height);
						c.background() = color(src.at(row).second.at(col).rgb());
						this->xset_border(emu(src.at(row).second.at(col).weight_left()), c.left());
						this->xset_border(emu(src.at(row).second.at(col).weight_right()), c.right());
						this->xset_border(emu(src.at(row).second.at(col).weight_top()), c.top());
						this->xset_border(emu(src.at(row).second.at(col).weight_bottom()), c.bottom());
						tbl.add(c);
						
						cur_.x = x + cur_.width;
						cur_.y = y;
					}
					
					cur_.x = ox;
					cur_.y -= height;
					tbl.newline();
				}
				cur_.width = width;
				
				// finalize
				tbl(out);
				out << ss.str();
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xputimage
			/* ------------------------------------------------------------- */
			template <class OutStream, class ShapeT>
			bool xputimage(OutStream& out, const ShapeT& sh) {
				if (objs_.find(sh.reference()) != objs_.end()) {
					const xobj_type& ref = objs_[sh.reference()];
#if 1
					if (sh.reference().find(LITERAL(".png")) != string_type::npos) {
						double hrate = cur_.width / ref.worigin;
						double w = cur_.width + (ref.width - ref.worigin) * hrate;
						double vrate = cur_.height / ref.horigin;
						double h = cur_.height + (ref.height - ref.horigin) * vrate;
						image_contents image(ref.label, coordinate(cur_.x, cur_.y - h), w, h);
						image(out);
					}
					else {
						image_contents image(ref.label,
							coordinate(cur_.x, cur_.y - cur_.height), cur_.width, cur_.height);
						image(out);
					}
#else
					image_contents image(ref.label,
						coordinate(cur_.x, cur_.y - cur_.height), cur_.width, cur_.height);
					image(out);
#endif
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
					wmf.font1st(f_[PPTX_DEFAULT_LATIN]);
					wmf.font2nd(f_[PPTX_DEFAULT_JAPAN]);
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
			//  xadjust_tail
			/* ------------------------------------------------------------- */
			template <class InIter>
			void xadjust_tail(InIter& pos, InIter last, string_type& dest, int rest) {
				static const int threshold = 500; // [0--1000]
				if (pos == last) return;
				string_type tmp(pos, last);
				if (dest.empty() || (tmp.size() <= 2 && rest > threshold)) {
					dest += *pos;
					++pos;
					dest += *pos;
					++pos;
				}
			}
			
			/* ------------------------------------------------------------- */
			//  xgetline
			/* ------------------------------------------------------------- */
			template <class Source>
			size_type xgetline(const Source& p) {
				if (p.empty()) return 1;
					
				size_type dest = 0;
				string_type s;
				for (size_type j = 0; j < p.size(); ++j) {
					if (!p.at(j).data().empty()) {
						std::basic_string<wchar_t> tmp = babel::utf8_to_unicode(p.at(j).data());
						//s += win32conv(tmp.begin(), tmp.end());
						s += to_narrow(tmp.begin(), tmp.end());
					}
					
					if (s.size() >= 2 && s.at(s.size() - 2) == 0x00 &&
						(s.at(s.size() - 1) == 0x0a || s.at(s.size() - 1) == 0x0d || s.at(s.size() - 1) == 0x0c)) {
						s.erase(s.size() - 2);
						
						string_type latin = p.at(j).font().latin();
						string_type japan = p.at(j).font().japan();
						this->xregist_font(latin, japan);
						text_helper h(f_[latin].property(), charset::utf16);
						text_helper hj(f_[japan].property(), charset::utf16);
						
						typename string_type::iterator it = s.begin();
						double size = p.at(j).font().size() * PPTX_FONT_SCALE;
						while (1) {
							string_type line;
							//double width = cur_.width - 2.0 * 2;
							double width = cur_.width - cur_.hpad * 2;
							int limit = static_cast<int>(width / size * 1000);
							int result = 0;
							if (is_ascii(it, s.end(), charset::utf16)) {
								result = h.getline(it, s.end(), std::inserter(line, line.end()), limit);
							}
							else result = hj.getline(it, s.end(), std::inserter(line, line.end()), limit);
							this->xadjust_tail(it, s.end(), line, limit - result);
							++dest;
							if (it == s.end()) break;
						}
						s.erase();
					}
				}
				
				if (!s.empty()) {
					string_type latin = p.at(p.size() - 1).font().latin();
					string_type japan = p.at(p.size() - 1).font().japan();
					this->xregist_font(latin, japan);
					text_helper h(f_[latin].property(), charset::utf16);
					text_helper hj(f_[japan].property(), charset::utf16);
					
					typename string_type::iterator it = s.begin();
					double size = p.at(0).font().size() * PPTX_FONT_SCALE;
					while (1) {
						string_type line;
						//double width = cur_.width - 2.0 * 2;
						double width = cur_.width - cur_.hpad * 2;
						int limit = static_cast<int>(width / size * 1000);
						int result = 0;
						if (is_ascii(it, s.end(), charset::utf16)) {
							result = h.getline(it, s.end(), std::inserter(line, line.end()), limit, false);
						}
						else result = hj.getline(it, s.end(), std::inserter(line, line.end()), limit, false);
						this->xadjust_tail(it, s.end(), line, limit - result);
						++dest;
						if (it == s.end()) break;
					}
				}
				
				return dest;
			}
			
			/* ------------------------------------------------------------- */
			//  xputcell
			/* ------------------------------------------------------------- */
			template <class OutStream, class CellT>
			bool xputcell(OutStream& out, const CellT& src) {
				typedef typename CellT::value_type paragraph;
				cur_.y -= 2.0;
				if (src.empty()) return true;
				
				
				babel::init_babel();
				double vsp = cur_.height;
				for (size_type i = 0; i < src.size(); ++i) {
					double maxh = 0.0;
					for (size_type j = 0; j < src.at(i).size(); ++j) {
						maxh = std::max(maxh, src.at(i).at(j).font().size());
					}
					if (maxh == 0.0) maxh = src.at(i).font().size();
					
					if (src.at(i).before() > 0.0) vsp -= maxh * src.at(i).before();
					vsp -= maxh * baseline_ * this->xgetline(src.at(i));
				}
				
				vsp -= 2 * cur_.vpad;
				if (src.valign() == 0) cur_.y -= cur_.vpad;
				else if (src.valign() == 1) cur_.y -= (cur_.vpad + vsp / 2.0);
				else if (src.valign() == 2) cur_.y -= (cur_.vpad + vsp);
				
				double ox = cur_.x;
				for (size_type i = 0; i < src.size(); ++i) {
					const paragraph& p = src.at(i);
					
					/*
					 * Note: 1段落の中で複数の文字サイズが存在する場合，
					 * ベースラインは，最も大きなサイズを基準にする．
					 */
					cur_.max_font_size = 0.0;
					for (size_type j = 0; j < p.size(); ++j) {
						cur_.max_font_size = std::max(cur_.max_font_size, p.at(j).font().size());
					}
					if (cur_.max_font_size == 0.0) cur_.max_font_size = p.font().size();
					
					if (p.before() > 0.0) cur_.y -= p.before() * cur_.max_font_size;
					
					if (p.empty()) cur_.y -= cur_.max_font_size * baseline_;
					else {
						this->xmove_horizon(ox, p, 0, string_type());
						if (p.butype() != 0 && !p.bullet().empty()) {
							this->xput_bullet(out, p);
						}
						else if (!p.bullet().empty()) {
							double size = p.font().size() * PPTX_FONT_SCALE;
							std::basic_string<wchar_t> tmp;
							tmp = babel::utf8_to_unicode(p.bullet());
							//string_type s = win32conv(tmp.begin(), tmp.end());
							string_type s = to_narrow(tmp.begin(), tmp.end());
							
							text_helper h(f_[PPTX_DEFAULT_JAPAN].property(), charset::utf16);
							double n = h.width(s.begin(), s.end()) / 1000.0;
							cur_.x -= n * size;
						}
						
						for (size_type j = 0; j < p.size(); ++j) {
							this->xput_text_piece(out, p, j, ox);
						}
					}
					
					cur_.x = ox;
				}
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xputtext
			/* ------------------------------------------------------------- */
			template <class OutStream, class ShapeT>
			bool xputtext(OutStream& out, const ShapeT& sh) {
				typedef typename ShapeT::txbox::value_type paragraph;
				if (sh.texts().empty()) return true;
				
				babel::init_babel();
				double vsp = cur_.height;
				for (size_type i = 0; i < sh.texts().size(); ++i) {
					double maxh = 0.0;
					for (size_type j = 0; j < sh.texts().at(i).size(); ++j) {
						maxh = std::max(maxh, sh.texts().at(i).at(j).font().size());
					}
					if (maxh == 0.0) maxh = sh.texts().at(i).font().size();
					
					if (i == 0) vsp -= std::max(maxh * sh.texts().at(i).before(), cur_.vpad);
					else vsp -= maxh * sh.texts().at(i).before();
					size_type n = this->xgetline(sh.texts().at(i));
					vsp -= maxh * baseline_ * n;
					clx::logger::trace(TRACEF("num of line: %d, vsp: %f", n, vsp));
				}
				vsp -= cur_.vpad;
				
				clx::logger::trace(TRACEF("valign: %d", sh.texts().valign()));
				if (sh.texts().valign() == 1) cur_.y -= vsp / 2.0;
				else if (sh.texts().valign() == 2) cur_.y -= vsp;
				
				double ox = cur_.x;
				for (size_type i = 0; i < sh.texts().size(); ++i) {
					const paragraph& p = sh.texts().at(i);
					
					/*
					 * Note: 1段落の中で複数の文字サイズが存在する場合，
					 * ベースラインは，最も大きなサイズを基準にする．
					 */
					cur_.max_font_size = 0.0;
					for (size_type j = 0; j < p.size(); ++j) {
						cur_.max_font_size = std::max(cur_.max_font_size, p.at(j).font().size());
					}
					if (cur_.max_font_size == 0.0) cur_.max_font_size = p.font().size();
					
					//if (p.before() > 0.0) cur_.y -= p.before() * cur_.max_font_size;
					if (i == 0) cur_.y -= std::max(p.before() * std::max(cur_.max_font_size, p.font().size()), cur_.vpad);
					else cur_.y -= p.before() * std::max(cur_.max_font_size, p.font().size());
					
					if (p.empty()) cur_.y -= cur_.max_font_size * baseline_;
					else {
						this->xmove_horizon(ox, p, 0, string_type());
						if (p.butype() != 0 && !p.bullet().empty()) {
							string_type s = (!p.at(0).data().empty()) ? p.at(0).data() : LITERAL("");
							if (!s.empty()) utf8::chomp(s);
							if (!s.empty()) this->xput_bullet(out, p);
						}
						else if (!p.bullet().empty()) {
							double size = p.font().size() * PPTX_FONT_SCALE;
							std::basic_string<wchar_t> tmp;
							tmp = babel::utf8_to_unicode(p.bullet());
							string_type s = to_narrow(tmp.begin(), tmp.end());
							
							text_helper h(f_[PPTX_DEFAULT_MONO].property(), charset::utf16);
							double n = h.width(s.begin(), s.end()) / 1000.0;
							cur_.x -= n * size;
						}
						
						for (size_type j = 0; j < p.size(); ++j) {
							this->xput_text_piece(out, p, j, ox);
						}
					}
					
					cur_.x = ox;
				}
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xput_text_piece
			/* ------------------------------------------------------------- */
			template <class OutStream, class Source>
			bool xput_text_piece(OutStream& out, const Source& src, size_type pos, double ox) {
				double size = src.at(pos).font().size() * PPTX_FONT_SCALE;
				double vpos = cur_.max_font_size * PPTX_FONT_SCALE - size;
				double baseline = baseline_;
				//double baseline = (src.baseline() < 0.0) ? baseline_ : src.baseline();
				//if (src.baseline() > 0.0) clx::logger::debug(DEBUGF("changed baseline: %f", baseline));
				if (cur_.max_font_size * PPTX_FONT_SCALE < size) {
					clx::logger::warn(WARNF("max font size is wrong: %f, %f", cur_.max_font_size * PPTX_FONT_SCALE, size));
					vpos = 0.0;
				}
				clx::logger::debug(DEBUGF("font size: %f (delta %f)", size, vpos));
				string_type s = (!src.at(pos).data().empty()) ? src.at(pos).data() : LITERAL("");
				
				// Memo: タブ文字は空白8個分で描画（暫定）
				clx::replace_all(s, LITERAL("\t"), LITERAL("　　　　　　　　"));
				bool newline = false;
				if (!s.empty() && (*s.rbegin() == 0x0a || *s.rbegin() == 0x0c || *s.rbegin() == 0x0d)) {
					newline = true;
					utf8::chomp(s);
				}
				
				if (s.empty() && pos == src.size() - 1) {
					double scale = src.after() > 0.0 ? 1.0 + src.after() : baseline;
					cur_.y -= src.at(pos).font().size() * scale;
					this->xmove_horizon(ox, src, pos + 1, string_type());
					return true;
				}
				
				this->xregist_font(src.at(pos).font().latin(), src.at(pos).font().japan());
				string_type name;
				if (utf8::is_ascii(s) && src.at(pos).ascii()) name = src.at(pos).font().latin();
				else name = src.at(pos).font().japan();
				//name = src.at(pos).font().japan();
				text_helper h(f_[name].property(), charset::utf16);
				
				clx::logger::trace(TRACEF("String: %s (%d)", s.c_str(), clx::utf8::distance(s)));
				clx::logger::debug(DEBUGF("UseFont: %s -> %s (%f pt)",
					name.c_str(), f_[name].property().name().c_str(), src.at(pos).font().size()));
				
				std::basic_string<wchar_t> tmp = babel::utf8_to_unicode(s);
				s = to_narrow(tmp.begin(), tmp.end());
				typename string_type::iterator it = s.begin();
				
				while (1) {
					string_type line;
					//double width = cur_.width - (cur_.x - ox);
					double width = cur_.width - (cur_.x - ox) - cur_.hpad;
					int limit = static_cast<int>(width / size * 1000);
					if (limit < 1000) {
						cur_.y -= src.at(pos).font().size() * baseline;
						this->xmove_horizon(ox, src, pos + 1, s);
						//width = cur_.width - (cur_.x - ox);
						width = cur_.width - (cur_.x - ox) - cur_.hpad;
						limit = static_cast<int>(width / size * 1000);
					}
					int result = h.getline(it, s.end(), std::inserter(line, line.end()), limit, false);
					clx::logger::trace(TRACEF("limit: %d, result: %d", limit, result));
					
					if (it != s.end()) this->xadjust_tail(it, s.end(), line, limit - result);
					else if (line.empty()) break;

					double n = h.width(line.begin(), line.end()) / 1000.0;
					//double sp = width - n * size;
					size_type rgb = src.at(pos).font().rgb();
					if (rgb > 0x00ffffff) rgb = 0;
					
					size_type deco = src.at(pos).decorate();
					if ((deco & 0x10)) {
						deco |= 0x04;
						if (doc_.theme() && doc_.theme()->palette().size() > 10) {
							rgb = doc_.theme()->palette().at(10);
						}
						else rgb = 0x0000ff;
					}
					
					size_type background = doc_.at(cur_.page).rgbs().empty() ? 0xffffff :
						doc_.at(cur_.page).rgbs().at(0);
					rgb = this->xadjust_color(rgb, background);
					
					//size_type rise = (src.at(pos).baseline() != 0) ? size * 0.7 * 0.3 : size * 0.3;
					size_type rise = (src.at(pos).baseline() != 0) ? size * 0.7 * 0.2 : size * 0.2;
					text_contents txt(line, coordinate(cur_.x, cur_.y - vpos + rise), f_[name]);
					//text_contents txt(line, coordinate(cur_.x, cur_.y + vpos), f_[name]);
					txt.font_size(size);
					txt.font_color(color(rgb));
					txt.decorate(deco);
					txt.space(-line.size() / 2.0 * wordsp_);
					if (src.at(pos).baseline() != 0) {
						txt.baseline(size * (src.at(pos).baseline() / 100000.0 - 0.3));
						txt.font_size(size * 0.7);
					}
					//if (sp < 0.0) txt.space(sp);
					txt(out);
					clx::logger::trace(TRACEF("output: %s", clx::hexdump(line).c_str()));
					
					if (it == s.end()) {
						double scale = (src.at(pos).baseline() != 0) ? 0.7 : 1.0;
						cur_.x += n * size * scale;
						break;
					}
					
					line.assign(it, s.end());
					cur_.y -= src.at(pos).font().size() * baseline;
					this->xmove_horizon(ox, src, pos + 1, line);
				}
				
				if (newline) {
					cur_.y -= src.at(pos).font().size() * baseline;
					this->xmove_horizon(ox, src, pos + 1, string_type());
				}
				else if (pos >= src.size() - 1) {
					double scale = src.after() > 0.0 ? 1.0 + src.after() : baseline;
					cur_.y -= src.at(pos).font().size() * scale;
					this->xmove_horizon(ox, src, pos + 1, string_type());
				}
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xput_bullet
			/* ------------------------------------------------------------- */
			template <class OutStream, class TextT>
			bool xput_bullet(OutStream& out, const TextT& src) {
				static wchar_t bu[9] = { 0x30fb, 0x2010, 0x30fb, 0x2010, 0x30fb,
					0x2010, 0x30fb, 0x2010, 0x30fb };
				if (src.bullet().empty()) return true;
				
				double size = src.font().size() * PPTX_FONT_SCALE;
				if (!src.empty() && src.at(0).font().size() != src.font().size()) {
					size = src.at(0).font().size() * PPTX_FONT_SCALE;
				}
				std::basic_string<wchar_t> tmp;
				if (!src.bullet().empty()) tmp = babel::utf8_to_unicode(src.bullet());
				else tmp = bu[src.level()];
				//string_type s = win32conv(tmp.begin(), tmp.end());
				string_type s = to_narrow(tmp.begin(), tmp.end());
				
				text_helper h(f_[PPTX_DEFAULT_MONO].property(), charset::utf16);
				double n = h.width(s.begin(), s.end()) / 1000.0;
				
				double x = cur_.x - n * size;
				//double y = cur_.y - size * (1.0 - src.busize());
				double y = cur_.y - size * ((1.0 - src.busize()) / 2.0);
				
				size_type rgb = src.buclr();
				if (rgb > 0x00ffffff) rgb = 0;
				
				size_type deco = src.at(0).decorate();
				if ((deco & 0x10)) {
					deco |= 0x04;
					if (doc_.theme() && doc_.theme()->palette().size() > 10) {
						rgb = doc_.theme()->palette().at(10);
					}
					else rgb = 0x0000ff;
				}
				
				//size_type rise = size * src.busize() * 0.3;
				size_type rise = size * src.busize() * 0.2;
				text_contents txt(s, coordinate(x, y + rise), f_[PPTX_DEFAULT_MONO]);
				//text_contents txt(s, coordinate(x, y), f_[PPTX_DEFAULT_MONO]);
				txt.font_size(size * src.busize());
				txt.decorate(deco);
				txt.font_color(color(rgb));
				txt(out);
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xput_background
			/* ------------------------------------------------------------- */
			template <class OutStream, class SlideT>
			bool xput_background(OutStream& out, const SlideT& src) {
				string_type sh;
				if (!src.rgbs().empty()) {
					out << LITERAL("q") << std::endl;
					if (src.rgbs().size() > 1) {
						shading_type grad;
						grad.add(color(src.rgbs().at(0)));
						grad.add(color(src.rgbs().at(src.rgbs().size() - 1)));
						
						double ox = 0.0;
						double oy = 0.0;
						
						double x = std::cos(clx::radian(src.angle()));
						if (x < 0.0) {
							ox += 1.0;
							x += 1.0;
						}
						
						double y = std::sin(clx::radian(360 - src.angle()));
						if (y < 0.0) {
							oy += 1.0;
							y += 1.0;
						}
						
						grad.first(coordinate(ox, oy));
						grad.last(coordinate(x, y));
						cur_.holder.add(grad);
						sh = grad.label();
					}
					else {
						color bg(src.rgb());
						out << fmt(LITERAL("%f %f %f rg")) % bg.red() % bg.green() % bg.blue() << std::endl;
					}
					out << fmt(LITERAL("0 0 %f %f re")) % doc_.width() % doc_.height() << std::endl;
					if (!sh.empty()) {
						out << LITERAL("W n") << std::endl;
						out << LITERAL("q") << std::endl;
						out << fmt(LITERAL("%f 0 0 %f 0 0 cm"))
							% doc_.width() % doc_.height()
							<< std::endl;
						out << fmt(LITERAL("/%s sh")) % sh << std::endl;
						out << LITERAL("Q") << std::endl;
					}
					else out << LITERAL("f") << std::endl;
					out << LITERAL("Q") << std::endl;
				}
				
				string_type bgimg(src.bgimage());
				if (!bgimg.empty() && objs_.find(bgimg) != objs_.end()) {
					const xobj_type& ref = objs_[bgimg];
#if 1
					if (bgimg.find(LITERAL(".png")) != string_type::npos) {
						double hrate = pp_.width() / ref.worigin;
						double w = pp_.width() + (ref.width - ref.worigin) * hrate;
						double vrate = pp_.height() / ref.horigin;
						double h = pp_.height() + (ref.height - ref.horigin) * vrate;
						image_contents image(ref.label, coordinate(0, 0), w, h);
						image(out);
					}
					else {
						image_contents image(ref.label,
							coordinate(0, 0), pp_.width(), pp_.height());
						image(out);
					}
#else
					image_contents image(ref.label,
						coordinate(0, 0), pp_.width(), pp_.height());
					image(out);
#endif
					cur_.objs.push_back(ref);
					return true;
				}
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xregist_font
			/* ------------------------------------------------------------- */
			bool xregist_font(const string_type& latin, const string_type& japan) {
				if (f_.find(latin) == f_.end()) {
					font_object f = ff_.create(latin, charset::utf16);
					f_[latin] = f;
					cur_.fonts[latin] = f;
					cur_.fused.insert(latin);
				}
				
				if (f_.find(japan) == f_.end()) {
					font_object f = ff_.create(japan, charset::utf16);
					f_[japan] = f;
					cur_.fonts[japan] = f;
					cur_.fused.insert(japan);
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
					width += (w / 1000.0) * src.at(pos - 1).font().size() * PPTX_FONT_SCALE;
					
					double indent = emu(src.indent());
					double space = cur_.width - width;
					if (space - indent < 0) {
						cur_.x = ox + indent + cur_.hpad;
						return true;
					}
					
					if (newline) {
						if (src.align() == 1) cur_.x = ox + space / 2.0;
						else if (src.align() == 2) cur_.x = ox + space - cur_.hpad;
						//else if (indent > 0.0) cur_.x = ox + indent;
						//else cur_.x = ox + cur_.hpad;
						else cur_.x = ox + indent + cur_.hpad;
						return true;
					}
				}
				
				double indent = emu(src.indent());
				for (size_type i = pos; i < src.size(); ++i) {
					string_type latin = src.at(i).font().latin();
					string_type japan = src.at(i).font().japan();
					this->xregist_font(latin, japan);
					text_helper h(f_[latin].property(), charset::utf16);
					text_helper hj(f_[japan].property(), charset::utf16);
					
					string_type s = src.at(i).data();
					bool newline = false;
					if (!s.empty() && (*s.rbegin() == 0x0a || *s.rbegin() == 0x0c || *s.rbegin() == 0x0d)) {
						utf8::chomp(s);
						newline = true;
					}
					
					std::basic_string<wchar_t> tmp = babel::utf8_to_unicode(s);
					s = to_narrow(tmp.begin(), tmp.end());
					
					double w = (is_ascii(s.begin(), s.end(), charset::utf16))
						? h.width(s.begin(), s.end())
						: hj.width(s.begin(), s.end());
					width += (w / 1000.0) * src.at(i).font().size() * PPTX_FONT_SCALE;
					double space = cur_.width - width;
					if (space - indent < 0.0) {
						cur_.x = ox + indent + cur_.hpad;
						return true;
					}
					
					if (newline) {
						if (src.align() == 1) cur_.x = ox + space / 2.0;
						else if (src.align() == 2) cur_.x = ox + space - cur_.hpad;
						//else if (indent > 0.0) cur_.x = ox + indent;
						//else cur_.x = ox + cur_.hpad;
						else cur_.x = ox + indent + cur_.hpad;
						return true;
					}
				}
				
				double space = cur_.width - width;
				if (src.align() == 1) cur_.x = ox + space / 2.0;
				else if (src.align() == 2) cur_.x = ox + space - cur_.hpad;
				//else if (indent > 0.0) cur_.x = ox + indent;
				//else cur_.x = ox + cur_.hpad;
				else cur_.x = ox + indent + cur_.hpad;
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xadjust_color
			/* ------------------------------------------------------------- */
			size_type xadjust_color(size_type fill, size_type background) {
				size_type red = background & 0xff;
				size_type green = (background >> 8) & 0xff;
				size_type blue = (background >> 16) & 0xff;
				
				if (fill == doc_.theme()->palette().at(0) &&
					red < 0x7f && green < 0x7f && blue < 0x7f) {
					return doc_.theme()->palette().at(1);
				}
				
				return fill;
			}
		};
		
		typedef basic_pptx<char> pptx;
	}
}

#endif // FAML_PDF_PPTX_H
