/* ------------------------------------------------------------------------- */
/*
 *  wmf.h
 *
 *  Copyright (c) 2009, Four and More. All rights reserved.
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PDF_EMF_H
#define FAML_PDF_EMF_H

#include "config.h"
#include <algorithm>
#include <string>
#include <iterator>
#include <istream>
#include <sstream>
#include "clx/literal.h"
#include "clx/format.h"
#include "clx/utility.h"
#include "page.h"
#include "contents/emf.h"

namespace faml {
	namespace pdf {
		/* ----------------------------------------------------------------- */
		//  basic_emf
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_emf {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef std::basic_istream<CharT, Traits> istream_type;
			typedef unsigned char  byte_type;
			typedef unsigned short WORD_type;
			typedef unsigned int   DWORD_type;
			typedef short SHORT_type;
			typedef long LONG_type;
			typedef float FLOAT_type;
			
			basic_emf(istream_type& in, const page_property& page,
				int x = scale::expand, int pos = align::center | align::middle) :
				page_(page), scale_(x), align_(pos), in_(in) {}
			
			template <class OutStream, class PDFManager>
			bool operator()(OutStream& out, PDFManager& pm) {
				this->xproperty();
				page_type newpage(page_);
				this->xresources(out, pm);
				newpage.resources(pm.index());
				this->xcontents(out, pm);
				newpage.contents(pm.index());
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
			
			page_property page_;
			int scale_;
			int align_;
			istream_type& in_;
			int width_;
			int height_;
			int OffsetX_;
			int OffsetY_;
			double inch_; // 1 point per inch_/72 inch
			
			/* ------------------------------------------------------------- */
			//  current_GDI_funciton
			/* ------------------------------------------------------------- */
			struct current_GDI_function {
				DWORD_type FunctionNumber;
				DWORD_type RecordSize;
				std::vector<DWORD_type> Parameters;
				current_GDI_function() : FunctionNumber(0), RecordSize(0) {}
			};
			current_GDI_function CrFunc_;
			
			/* ------------------------------------------------------------- */
			//  current_status
			/* ------------------------------------------------------------- */
			struct current_status {
				SHORT_type OrgX;
				SHORT_type OrgY;
				LONG_type PosX;
				LONG_type PosY;
				SHORT_type ExtX;
				SHORT_type ExtY;
				SHORT_type ObjectID;
				byte_type BGRed;
				byte_type BGGreen;
				byte_type BGBlue;
				current_status() : OrgX(0),OrgY(0),PosX(0),PosY(0),ExtX(0),ExtY(0),
					ObjectID(0),BGRed(0),BGGreen(0),BGBlue(0) {}
			};
			current_status CrStat_;
			
			/* ------------------------------------------------------------- */
			//  WorldTransformMatrix
			/* ------------------------------------------------------------- */
			struct WorldTransformMatrix {
				FLOAT_type M[2][2];
				WorldTransformMatrix() : M() {}
			};
			WorldTransformMatrix WTMat_;
			
			/* ------------------------------------------------------------- */
			//  BrushObject
			/* ------------------------------------------------------------- */
			struct BrushObject {
				byte_type Red;
				byte_type Green;
				byte_type Blue;
				LONG_type OrgX;
				LONG_type OrgY;
				bool StoredFlg;
				BrushObject() : Red(0),Green(0),Blue(0),OrgX(0),OrgY(0),StoredFlg(false){}
			};
			BrushObject BrushSwap_;
			std::vector<BrushObject> BrushObjectTable_;
			
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
				out << LITERAL("/ProcSet [ /PDF /ImageC ]") << std::endl;
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
				contents::basic_emf<CharT, Traits> cts(in_);
				cts.read_header();
				cts.scale(inch_);
				cts.origin(coordinate(OffsetX_, OffsetY_));
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
				DWORD_type RecordType = 0;
				xgetdata<DWORD_type>(RecordType);
			 	if(xcompare(RecordType,(DWORD_type)0x00000001)){
		 		
			 		/* Enhanced Metafile Header */
			 		DWORD_type RecordSize = 0;		xgetdata<DWORD_type>(RecordSize);
			 		LONG_type BoundsLeft = 0;		xgetdata<LONG_type>(BoundsLeft);
			 		LONG_type BoundsTop = 0;		xgetdata<LONG_type>(BoundsTop);
			 		LONG_type BoundsRight = 0;		xgetdata<LONG_type>(BoundsRight);
			 		LONG_type BoundsBottom = 0;		xgetdata<LONG_type>(BoundsBottom);
			 		LONG_type FrameLeft = 0;		xgetdata<LONG_type>(FrameLeft);
			 		LONG_type FrameTop = 0;			xgetdata<LONG_type>(FrameTop);
			 		LONG_type FrameRight = 0;		xgetdata<LONG_type>(FrameRight);
			 		LONG_type FrameBottom = 0;		xgetdata<LONG_type>(FrameBottom);
			 		DWORD_type Signature = 0;		xgetdata<DWORD_type>(Signature);
			 		DWORD_type Version = 0;			xgetdata<DWORD_type>(Version);
			 		DWORD_type Size = 0;			xgetdata<DWORD_type>(Size);
			 		DWORD_type NumOfRecords = 0;	xgetdata<DWORD_type>(NumOfRecords);
			 		WORD_type NumOfHandles = 0;		xgetdata<WORD_type>(NumOfHandles);
			 		WORD_type Reserved = 0;			xgetdata<WORD_type>(Reserved);
			 		DWORD_type SizeOfDescrip = 0;	xgetdata<DWORD_type>(SizeOfDescrip);
			 		DWORD_type OffsOfDescrip = 0;	xgetdata<DWORD_type>(OffsOfDescrip);
			 		DWORD_type NumPalEntries = 0;	xgetdata<DWORD_type>(NumPalEntries);
			 		LONG_type WidthDevPixels = 0;	xgetdata<LONG_type>(WidthDevPixels);
					LONG_type HeightDevPixels = 0;	xgetdata<LONG_type>(HeightDevPixels);
					LONG_type WidthDevMM = 0;		xgetdata<LONG_type>(WidthDevMM);
					LONG_type HeightDevMM = 0;		xgetdata<LONG_type>(HeightDevMM);
					int gotsize = 88;
					
			 		double margin_Left_ = (double)(FrameLeft * 0.032);
			 		double margin_Top_ = (double)(FrameTop * 0.032);
			 		double margin_Right_ = (double)(FrameRight * 0.032);
			 		double margin_Bottom_ = (double)(FrameBottom * 0.032);
			 		//width_ = (size_type)(BoundsRight-BoundsLeft+1);
			 		//height_ = (size_type)(BoundsBottom-BoundsTop+1);
			 		width_ = (size_type)(margin_Right_ - margin_Left_ + 1);
			 		height_ = (size_type)(margin_Bottom_ - margin_Top_ + 1);
			 		
			 		/* Get Header Size */
			 		DWORD_type HeaderSize = 88;
			 		if(RecordSize >= 88){
			 			HeaderSize = RecordSize;
			 			if((OffsOfDescrip >= 88) && (SizeOfDescrip * 2 <= RecordSize)){
			 				HeaderSize = OffsOfDescrip;
			 			}
			 		}
			 		
			 		/* Header Extention 1 */
			 		DWORD_type cdPixelFormat = 0;
			 		DWORD_type offPixelFormat = 0;
			 		DWORD_type bOpenGL = 0;
			 		if(HeaderSize >= 100){
			 			xgetdata<DWORD_type>(cdPixelFormat);
			 			xgetdata<DWORD_type>(offPixelFormat);
			 			xgetdata<DWORD_type>(bOpenGL);
			 			gotsize += 12;
			 			
			 			if((offPixelFormat >= 100) && (offPixelFormat + cdPixelFormat <= RecordSize)){
			 				if(offPixelFormat < HeaderSize){
			 					HeaderSize = offPixelFormat;
			 				}
			 			}
			 		}
			 		
			 		/* Header Extention 2 */
			 		DWORD_type MicrometersX = 0;
			 		DWORD_type MicrometersY = 0;
			 		if(HeaderSize >= 108){
			 			xgetdata<DWORD_type>(MicrometersX);
			 			xgetdata<DWORD_type>(MicrometersY);
			 			gotsize += 8;
			 		}
			 		
			 		/* Description String */
			 		for(int i=gotsize; i<(int)OffsOfDescrip; i++){
			 			byte_type temp;
			 			xgetdata<byte_type>(temp);
			 			gotsize++;
			 		}
			 		for(int i=0; i<(int)SizeOfDescrip; i++){
			 			WORD_type temp;
			 			xgetdata<WORD_type>(temp);
			 			gotsize += 2;
			 		}
			 		
			 		/* Pixel Format Descriptor */
			 		/*if(RecordSize>=100){
			 			for(int i=gotsize; i<(int)offPixelFormat; i++){
				 			byte_type temp;
				 			xgetdata<byte_type>(temp);
				 			gotsize++;
				 		}
				 		for(int i=0; i<(int)cdPixelFormat; i++){
				 			WORD_type temp;
				 			xgetdata<WORD_type>(temp);
				 		}
			 		}*/
			 		
			 		for(int i=gotsize; i<(int)RecordSize; i++){
			 			byte_type temp;
			 			xgetdata<byte_type>(temp);
			 			gotsize++;
			 		}
		 		
			 	}else{
			 		throw "Exception : This file is not Enhanced Metafile Format File.\n";
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
					inch_ = page_h_max/(double)h;
				}else{
					inch_ = 1;
				}
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xposition
			/* ------------------------------------------------------------- */
			bool xposition() {
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
			
			/* ----------------------------------------------------------------- */
			//  xdivide
			/* ----------------------------------------------------------------- */
			template <class Type, class Source>
			Type xdivide(const Source& src, int num) {
				Type dest = 0;
				int shift = num * 8;
				dest |= (src >> shift);
				return dest;
			}
		};
		
		typedef basic_emf<char> emf;
	}
}

#endif // FAML_PDF_WMF_H
