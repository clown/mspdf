/* ------------------------------------------------------------------------- */
/*
 *  font.h
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
 *  Last-modified: Wed 14 Jan 2009 11:41:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PDF_FONT_H
#define FAML_PDF_FONT_H

#include <string>
#include "clx/lexical_cast.h"
#include "clx/format.h"
#include "clx/literal.h"
#include "clx/hexdump.h"
#include "utility.h"
#include "font_property.h"
#include "font_instance.h"

namespace faml {
	enum {
		horizon		= 0x01,
		vertical	= 0x02,
	};
	
	namespace detail {
		/* ----------------------------------------------------------------- */
		//  hexdump
		/* ----------------------------------------------------------------- */
		template <class Ch, class Tr>
		std::basic_string<Ch, Tr> hexdump(const std::basic_string<Ch, Tr>& s) {
			std::basic_string<Ch, Tr> dest;
			std::basic_string<Ch, Tr> tmp = clx::hexdump(s);
			
			size_t i = 0;
			while (i < tmp.size() - 1) {
				dest += LITERAL("#");
				dest += tmp.at(i++);
				dest += tmp.at(i++);
			}
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  csetstr
		/* ----------------------------------------------------------------- */
		template <class Ch, class Tr>
		inline void csetstr(int cset, int dir, std::basic_string<Ch, Tr>& dest) {
			switch (cset) {
			case faml::pdf::charset::win:
				dest = LITERAL("90ms-RKSJ");
				break;
			case faml::pdf::charset::mac:
				dest = LITERAL("90pv-RKSJ");
				break;
			case faml::pdf::charset::euc:
				dest = LITERAL("EUC");
				break;
			case faml::pdf::charset::utf8:
				dest = LITERAL("UniJIS-UTF8");
				break;
			case faml::pdf::charset::utf16:
				dest = LITERAL("UniJIS-UTF16");
				break;
			case faml::pdf::charset::utf32:
				dest = LITERAL("UniJIS-UTF32");
				break;
			default:
				dest = LITERAL("Identity");
				break;
			}
			
			if (dir == faml::horizon) dest += LITERAL("-H");
			else dest += LITERAL("-V");
		}
	}
	
	namespace pdf {
		/* ----------------------------------------------------------------- */
		/*
		 *  basic_font
		 *
		 *  The names of standard 14 fonts are as follow:
		 *    Times-Roman, Times-Bold, Times-Italic, Times-BoldItalic,
		 *    Helvetica, Helvetica-Bold, Helvetica-Oblique,
		 *    Helvetica-BoldOblique, Courier, Courier-Bold,
		 *    Courier-Oblique, Courier-BoldOblique, Symbol, ZapfDingbats
		 */
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_font {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef basic_font_property<CharT, Traits> property_type;
			
			basic_font() :
				index_(0), label_(), prop_(), cset_(0), dir_(0x01) {
				label_ = makelabel(LITERAL("F"));
			}
			
			basic_font(const basic_font& cp) :
				index_(cp.index_), label_(cp.label_),
				prop_(cp.prop_), cset_(cp.cset_), dir_(cp.dir_) {}
			
			basic_font& operator=(const basic_font& cp) {
				index_ = cp.index_;
				label_ = cp.label_;
				prop_ = cp.prop_;
				cset_ = cp.cset_;
				dir_ = cp.dir_;
				return *this;
			}
			
			explicit basic_font(const property_type& prop, int cset, int dir = 0x01) :
				index_(0), label_(), prop_(prop), cset_(cset), dir_(dir) {
				label_ = makelabel(LITERAL("F"));
			}
			
			explicit basic_font(const string_type& name, int cset, int dir = 0x01) :
				index_(0), label_(), cset_(cset), dir_(dir) {
				label_ = makelabel(LITERAL("F"));
				prop_ = getfont(name);
			}
			
			explicit basic_font(const char_type* name, int cset, int dir = 0x01) :
				index_(0), label_(), cset_(cset), dir_(dir) {
				label_ = makelabel(LITERAL("F"));
				prop_ = getfont(name);
			}
			
			template <class OutStream, class PDFGenerator>
			bool operator()(OutStream& out, PDFGenerator& pm) {
				if (index_ > 0) pm.setp(index_);
				else index_ = pm.newindex();
				return this->xbase(out, pm);
			}
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			size_type index() const { return index_; }
			const string_type& label() const { return label_; }
			int charset() const { return cset_; }
			int direction() const { return dir_; }
			const string_type& name() const { return prop_.name(); }
			const property_type& property() const { return prop_; }
			
			/* ------------------------------------------------------------- */
			//  Access methods (set).
			/* ------------------------------------------------------------- */
			void index(size_type cp) { index_ = cp; }
			void charset(int cp) { cset_ = cp; }
			void direction(int cp) { dir_ = cp; }
			void name(const string_type& cp) { prop_ = getfont(cp); }
			void property(const property_type& cp) { prop_ = cp; }
			
		private:
			typedef clx::basic_format<CharT, Traits> fmt;
			
			size_type index_;
			string_type label_;
			property_type prop_;
			int cset_;
			int dir_;
			
			/* ------------------------------------------------------------- */
			//  xbase
			/* ------------------------------------------------------------- */
			template <class OutStream, class PDFManager>
			bool xbase(OutStream& out, PDFManager& pm) {
				string_type scset;
				faml::detail::csetstr(cset_, dir_, scset);
				
				out << fmt(LITERAL("%d 0 obj")) % index_ << std::endl;
				out << LITERAL("<<") << std::endl;
				out << LITERAL("/Type /Font") << std::endl;
				out << LITERAL("/Subtype /Type") << prop_.type() << std::endl;
				out << LITERAL("/Name /") << label_ << std::endl;
				out << LITERAL("/BaseFont /") << faml::detail::hexdump(prop_.name()) << std::endl;
				out << LITERAL("/Encoding /") << scset << std::endl;
				
				switch (prop_.type()) {
				case 0:
					out << fmt(LITERAL("/DescendantFonts [ %d 0 R ]")) % (index_ + 1) << std::endl;
					break;
				case 1:
					out << fmt(LITERAL("/FontDescriptor %d 0 R")) % (index_ + 1) << std::endl;
					out << LITERAL("/FirstChar 0") << std::endl;
					out << LITERAL("/LastChar ") << prop_.widths().size() - 1 << std::endl;
					out << LITERAL("/Widths [ ");
					for (size_type i = 0; i < prop_.widths().size(); ++i) {
						out << std::dec << prop_.widths().at(i) << LITERAL(" ");
					}
					out << LITERAL(" ]") << std::endl;
					break;
				default:
					clx::logger::error(ERRORF("unknown font type"));
					return false;
				}
				
				out << LITERAL(">>") << std::endl;
				out << LITERAL("endobj") << std::endl;
				out << std::endl;
				
				if (prop_.type() == 0) return this->xcid(out, pm);
				else return this->xdescriptor(out, pm);
			}
			
			/* ------------------------------------------------------------- */
			//  xcid
			/* ------------------------------------------------------------- */
			template <class OutStream, class PDFManager>
			bool xcid(OutStream& out, PDFManager& pm) {
				out << fmt(LITERAL("%d 0 obj")) % pm.newindex() << std::endl;
				out << LITERAL("<<") << std::endl;
				out << LITERAL("/Type /Font") << std::endl;
				out << LITERAL("/Subtype /CIDFontType2") << std::endl;
				out << LITERAL("/BaseFont /") << faml::detail::hexdump(prop_.name()) << std::endl;
				out << fmt(LITERAL("/FontDescriptor %d 0 R")) % (pm.index() + 1) << std::endl;
				out << LITERAL("/CIDSystemInfo << /Registry (Adobe) /Ordering (Japan1) /Supplement 2 >>") << std::endl;
				out << LITERAL("/DW 1000") << std::endl;
				if (prop_.widths().empty()) {
					out << LITERAL("/W [ 1 632 500 ]") << std::endl;
				}
				else {
					out << LITERAL("/W [ ");
					for (size_type i = 0; i < prop_.widths().size(); ++i) {
						int w = prop_.widths().at(i);
						if (w != 1000) out << fmt(LITERAL("%d [%d] ")) % i % w;
					}
					out << LITERAL("]") << std::endl;
				}
				out << LITERAL(">>") << std::endl;
				out << LITERAL("endobj") << std::endl;
				out << std::endl;
				
				return this->xdescriptor(out, pm);
			}
			
			/* ------------------------------------------------------------- */
			//  xdescriptor
			/* ------------------------------------------------------------- */
			template <class OutStream, class PDFManager>
			bool xdescriptor(OutStream& out, PDFManager& pm) {
				out << fmt(LITERAL("%d 0 obj")) % pm.newindex() << std::endl;
				out << LITERAL("<<") << std::endl;
				out << LITERAL("/Type /FontDescriptor") << std::endl;
				out << LITERAL("/FontName /") << faml::detail::hexdump(prop_.name()) << std::endl;
				out << LITERAL("/Flags ") << prop_.flags() << std::endl;
				out << fmt(LITERAL("/FontBBox [ %d %d %d %d ]")) %
					prop_.bbox().lower_left().x() % prop_.bbox().lower_left().y() %
					prop_.bbox().upper_right().x() % prop_.bbox().upper_right().y() << std::endl;
				out << LITERAL("/CapHeight ") << prop_.cap_height() << std::endl;
				out << LITERAL("/Ascent ") << prop_.ascent() << std::endl;
				out << LITERAL("/Descent ") << prop_.descent() << std::endl;
				out << LITERAL("/StemV ") << prop_.stemv() << std::endl;
				out << LITERAL("/ItalicAngle ") << prop_.italic_angle() << std::endl;
				out << LITERAL("/MissingWidth 500") << std::endl;
				if (!prop_.panose().empty()) {
					out << fmt(LITERAL("/Style << /Panose <%s> >>")) % prop_.panose() << std::endl;
				}
				out << LITERAL(">>") << std::endl;
				out << LITERAL("endobj") << std::endl;
				out << std::endl;
				
				return true;
			}
		};
		
		typedef basic_font<char> font;
	}
}

#endif // FAML_PDF_FONT_H
