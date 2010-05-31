/* ------------------------------------------------------------------------- */
/*
 *  wmf.h
 *
 *  Copyright (c) 2009, Four and More. All rights reserved.
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PDF_WMF_H
#define FAML_PDF_WMF_H

#include "config.h"
#include <algorithm>
#include <string>
#include <iterator>
#include <istream>
#include <sstream>
#include "clx/literal.h"
#include "clx/format.h"
#include "page.h"
#include "page_property.h"
#include "font.h"
#include "text_contents.h"
#include "contents/wmf.h"

namespace faml {
	namespace pdf {
		/* ----------------------------------------------------------------- */
		//  basic_wmf
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_wmf {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef std::basic_istream<CharT, Traits> istream_type;
			typedef unsigned char  byte_type;
			typedef unsigned short WORD_type;
			typedef unsigned int   DWORD_type;
			typedef short SHORT_type;
			
			basic_wmf(istream_type& in, const page_property& page,
				int x = scale::expand, int pos = align::center | align::middle) :
				in_(in), page_(page), scale_(x), align_(pos),
				font_(LITERAL("Times-Roman"), charset::win),
				fontj_(LITERAL("ＭＳ ゴシック"), charset::win) {}
			
			virtual ~basic_wmf() throw() {}
			
			template <class OutStream, class PDFManager>
			bool operator()(OutStream& out, PDFManager& pm) {
				
				if (!font_(out, pm)) return false;
				if (!fontj_(out, pm)) return false;
				
				this->xproperty();
				page_type newpage(page_);
				this->xresources(out, pm);
				newpage.resources(pm.index());
				this->xcontents(out, pm);
				newpage.contents(pm.index());
				
				if (width_ > page_.width() || height_ > page_.height()) {
					page_.width(width_);
					page_.height(height_);
					newpage.property(page_);
				}
				
				return newpage(out, pm);
			}
			
			/* ------------------------------------------------------------- */
			//  Access methods (set).
			/* ------------------------------------------------------------- */
			void scale(int x) { scale_ = x; }
			void align(int pos) { align_ = pos; }
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			int scale() const { return scale_; }
			int align() const { return align_; }
			
		private:
			typedef clx::basic_format<CharT, Traits> fmt;
			typedef basic_page<CharT, Traits> page_type;
			typedef basic_font<CharT, Traits> font_type;
			typedef basic_text_contents<CharT, Traits> text_contents;
			
			istream_type& in_;
			page_property page_;
			int scale_;
			int align_;
			font_type font_;
			font_type fontj_;
			
			int width_;
			int height_;
			int OffsetX_;
			int OffsetY_;
			double inch_; // 1 point per inch_/72 inch
			
			/* ------------------------------------------------------------- */
			//  current_GDI_funciton
			/* ------------------------------------------------------------- */
			struct current_GDI_function {
				DWORD_type RecordSize;
				WORD_type FunctionNumber;
				std::vector<WORD_type> Parameters;
				current_GDI_function() : RecordSize(0), FunctionNumber(0) {}
			};
			current_GDI_function CrFunc_;
			
			/* ------------------------------------------------------------- */
			//  current_status
			/* ------------------------------------------------------------- */
			struct current_status {
				SHORT_type OrgX;
				SHORT_type OrgY;
				SHORT_type ExtX;
				SHORT_type ExtY;
				SHORT_type BrushObjectID;
				SHORT_type FontObjectID;
				byte_type BGRed;
				byte_type BGGreen;
				byte_type BGBlue;
				
				current_status() :
					OrgX(0), OrgY(0), ExtX(0), ExtY(0),
					BrushObjectID(0), FontObjectID(0),
					BGRed(0), BGGreen(0), BGBlue(0) {}
			};
			current_status CrStat_;
			
			/* ------------------------------------------------------------- */
			//  BrushObject
			/* ------------------------------------------------------------- */
			struct BrushObject {
				byte_type Red;
				byte_type Green;
				byte_type Blue;
				bool StoredFlg;
				
				BrushObject() :
					Red(0), Green(0), Blue(0),
					StoredFlg(false) {}
			};
			BrushObject BrushSwap_;
			std::vector<BrushObject> BrushObjectTable_;
			
			/* ------------------------------------------------------------- */
			//  FontObject
			/* ------------------------------------------------------------- */
			struct FontObject {
				WORD_type Height;
				WORD_type Width;
				WORD_type Escapement;
				WORD_type Orientation;
				WORD_type Weight;
				WORD_type Italic;
				WORD_type Underline;
				byte_type StrikeOut;
				byte_type CharSet;
				bool StoredFlg;
				
				FontObject() :
					Height(0), Width(0),
					Escapement(0), Orientation(0),
					Weight(0), Italic(0), Underline(0), StrikeOut(0),
					CharSet(0), StoredFlg(false) {}
			};
			FontObject FontSwap_;
			std::vector<FontObject> FontObjectTable_;
			
			/* ------------------------------------------------------------- */
			//  PenObject
			/* ------------------------------------------------------------- */
			/*struct PenObject {
				short dammy;
				PenObject() : dammy(0){}
			};
			PenObject PenSwap_;
			st::vector<PenObject> PenObjectTable_;*/
				
			/* ------------------------------------------------------------- */
			//  xproperty
			/* ------------------------------------------------------------- */
			bool xproperty() {
				xheader();
				xscaling();
				xposition();
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xresources
			/* ------------------------------------------------------------- */
			template <class OutStream, class PDFManager>
			bool xresources(OutStream& out, PDFManager& pm) {
				int index = pm.newindex();
				out << fmt(LITERAL("%d 0 obj")) % index << std::endl;
				out << LITERAL("<<") << std::endl;
				out << LITERAL("/Font") << std::endl;
				out << LITERAL("<<") << std::endl;
				out << fmt(LITERAL("/%s %d 0 R")) % font_.label() % font_.index() << std::endl;
				out << fmt(LITERAL("/%s %d 0 R")) % fontj_.label() % fontj_.index() << std::endl;
				out << LITERAL(">>") << std::endl;
				out << LITERAL("/ProcSet [ /PDF /Text /ImageC ]") << std::endl;
				out << LITERAL(">>") << std::endl;
				out << LITERAL("endobj") << std::endl;
				out << std::endl;
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xcontents
			/* ------------------------------------------------------------- */
			template <class OutStream, class PDFManager>
			bool xcontents(OutStream& out, PDFManager& pm) {
				size_type index = pm.newindex();
				
				std::basic_stringstream<CharT, Traits> ss;
				// main operation start
				in_.seekg(0);
				contents::basic_wmf<CharT, Traits> cts(in_);
				cts.read_header();
				cts.font1st(font_);
				cts.font2nd(fontj_);
				//cts.origin(coordinate(OffsetX_, OffsetY_));
				//cts.width(width_);
				//cts.height(height_);
				//cts.inch(inch_);
				cts(ss);
				
				//xmakestream(ss,pm);
				// main operation end
				out << fmt(LITERAL("%d 0 obj")) % index << std::endl;
				out << fmt(LITERAL("<< /Length %d >>")) % ss.str().size() << std::endl;
				out << LITERAL("stream") << std::endl;
				out << ss.str();
				out << std::endl;
				out << LITERAL("endstream") << std::endl;
				out << LITERAL("endobj") << std::endl;
				out << std::endl;
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xheader
			/* ------------------------------------------------------------- */
			bool xheader() {
				DWORD_type Key = 0;
				xgetdata<DWORD_type>(Key);
				bool amp = xcompare(Key,(DWORD_type)0x9AC6CDD7);
			 	if(amp){
			 		
			 		/* Placeable WMF Header */
			 		WORD_type Handle = 0;		xgetdata<WORD_type>(Handle);
			 		SHORT_type Left = 0;		xgetdata<SHORT_type>(Left);
			 		SHORT_type Top = 0;			xgetdata<SHORT_type>(Top);
			 		SHORT_type Right = 0;		xgetdata<SHORT_type>(Right);
			 		SHORT_type Bottom = 0;		xgetdata<SHORT_type>(Bottom);
			 		WORD_type Inch = 0;			xgetdata<WORD_type>(Inch);
			 		DWORD_type Reserved = 0;	xgetdata<DWORD_type>(Reserved);
			 		WORD_type Checksum = 0;		xgetdata<WORD_type>(Checksum);
			 		width_ = (size_type)(Right-Left);
			 		height_ = (size_type)(Bottom-Top);
			 		
				}else{
					in_.seekg(0);
				}
				
			 	/* Standard WMF Header */
			 	WORD_type FileType = 0;			xgetdata<WORD_type>(FileType);
			 	WORD_type HeaderSize = 0;		xgetdata<WORD_type>(HeaderSize);
			 	WORD_type Version = 0;			xgetdata<WORD_type>(Version);
			 	DWORD_type FileSize = 0;		xgetdata<DWORD_type>(FileSize);
			 	WORD_type NumOfObjects = 0;		xgetdata<WORD_type>(NumOfObjects);
			 	DWORD_type MaxRecordSize = 0;	xgetdata<DWORD_type>(MaxRecordSize);
			 	WORD_type NumOfParams = 0;		xgetdata<WORD_type>(NumOfParams);
			 	for(int i=0; i<(int)NumOfObjects; i++){
			 		BrushObject elem;
			 		BrushObjectTable_.push_back(elem);
			 	}
			 	BrushSwap_.StoredFlg = true;
			 	for(int i=0; i<(int)NumOfObjects; i++){
			 		FontObject elem;
			 		FontObjectTable_.push_back(elem);
			 	}
			 	FontSwap_.StoredFlg = true;
			 	
				if (!amp) {
					throw std::runtime_error("Exception : This file does not have Aldus Placebal MetafileHeader.");
					return false;
				}
				
				return true;
			 }
			
			/* ------------------------------------------------------------- */
			//  xscaling
			/* ------------------------------------------------------------- */
			bool xscaling() {
				int w,h;
				int page_w_max = page_.width() - page_.margin().left() - page_.margin().right();
				int page_h_max = page_.height() - page_.margin().top() - page_.margin().bottom();
				if (scale_ > 0) {
					w = static_cast<int>(width_ * scale_ / 100.0);
					h = static_cast<int>(height_ * scale_ / 100.0);
				}
				else if (scale_ == scale::full) {
					w = page_w_max;
					h = height_*(page_w_max/width_);
				}else{
					w = width_;
					h = height_;
				}
				
				if(w>page_w_max){
					inch_ = (double)page_w_max/(double)w;
					if(inch_*h>page_h_max) inch_ = inch_*(page_h_max/(inch_*h));
				}else if(h>page_h_max){
					inch_ = page_h_max/h;
				}else{
					inch_ = 1;
				}
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xposition
			/* ------------------------------------------------------------- */
			bool xposition() {
				//int pos = 0;
				switch (align_ & 0x0f) {
				case align::center:
					OffsetX_ = (int)(page_.width() - page_.margin().left() - page_.margin().right() - width_*inch_)/2  + page_.margin().left();
					break;
				case align::right:
					OffsetX_ = page_.width() - page_.margin().right() - (int)(width_*inch_);
					break;
				case align::left:
				default:
					OffsetX_ = page_.margin().left();
					break;
				}
				
				switch (align_ & 0xf0) {
				case align::top:
					OffsetY_ = page_.height() - page_.margin().top() - (int)(height_*inch_);
					break;
				case align::middle:
					OffsetY_ = (int)(page_.height() - page_.margin().top() - page_.margin().bottom() - height_*inch_)/2 + page_.margin().bottom();
					break;
				case align::bottom:
				default:
					OffsetY_ = page_.margin().bottom();
					break;
				}
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xgetdata
			/* ------------------------------------------------------------- */
			template <class InStream, class Type>
			bool xgetdata(Type& byte_data) {
				Type dest = 0;
				in_.read(reinterpret_cast<char*>(&dest), sizeof(Type));
				if (in_.gcount() < static_cast<int>(sizeof(Type))) return Type(-1);
				byte_data = dest;
				if (in_.fail()) return false;
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xcompare
			/* ------------------------------------------------------------- */
			template <class Type>
			bool xcompare(Type byteChar, Type byte16){
				Type temp = byteChar^byte16;
				Type zero_byte = 0x00;
				if(temp==zero_byte)return true;
				return false;
			}
			
			/* ----------------------------------------------------------------- */
			//  xcombine
			/* ----------------------------------------------------------------- */
			template <class Type, class Source1, class Source2>
			Type xcombine(const Source1& x1, const Source2& x2) {
				Type dest = 0;
				int shift = sizeof(Source1) * 8;
				dest |= x1;
				dest |= (x2 << shift);
				return dest;
			}
		};
		typedef basic_wmf<char> wmf;
	}
}

#endif // FAML_PDF_WMF_H
