/* ------------------------------------------------------------------------- */
/*
 *  contents/wmf.h
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
 *  Last-modified: Wed 01 Apr 2009 15:30:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PDF_CONTENTS_WMF_H
#define FAML_PDF_CONTENTS_WMF_H

#include "../config.h"
#include <stdexcept>
#include <algorithm>
#include <string>
#include <iterator>
#include <istream>
#include "clx/literal.h"
#include "clx/format.h"
#include "font.h"
#include "text_contents.h"
#include "circle.h"

namespace faml {
	namespace pdf {
		namespace contents {
			/* ------------------------------------------------------------- */
			//  basic_wmf
			/* ------------------------------------------------------------- */
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
				typedef basic_font<CharT, Traits> font_type;
				
				typedef unsigned char  byte_type;
				typedef unsigned short WORD_type;
				typedef unsigned int   DWORD_type;
				typedef short SHORT_type;
				typedef long LONG_type;
				
				basic_wmf(istream_type& in) :
					in_(in), font_(NULL), fontj_(NULL),
					width_(0), height_(0), OffsetX_(0), OffsetY_(0),
					scaleh_(1.0), scalev_(1.0) {}
				
				virtual ~basic_wmf() throw() {}
				
				template <class OutStream>
				bool operator()(OutStream& out) {
					return this->xmakestream(out);
				}
				
				/* ------------------------------------------------------------- */
				//  read_header
				/* ------------------------------------------------------------- */
				bool read_header() {
					DWORD_type Key = 0;
					xgetdata<DWORD_type>(Key);
					bool amp = xcompare(Key,(DWORD_type)0x9AC6CDD7);
				 	if (amp){
				 		
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
				 		PenObject elem;
				 		PenObjectTable_.push_back(elem);
				 	}
				 	PenSwap_.StoredFlg = true;
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
				
				/* --------------------------------------------------------- */
				//  Access methods (set).
				/* --------------------------------------------------------- */
				void origin(const coordinate& cp) {
					OffsetX_ = static_cast<int>(cp.x());
					OffsetY_ = static_cast<int>(cp.y());
				}
					
				void width(double cp) {
					if (width_ > 0.0) scaleh_ = cp / static_cast<double>(width_);
					else width_ = cp;
				}
				
				void height(double cp) {
					if (height_ > 0.0) scalev_ = cp / static_cast<double>(height_);
					else height_ = cp;
				}
				
				void font1st(const font_type& f) { font_ = &f; }
				void font2nd(const font_type& f) { fontj_ = &f; }
				
			private:
				typedef clx::basic_format<CharT, Traits> fmt;
				typedef basic_text_contents<CharT, Traits> text_contents;
				
				istream_type& in_;
				const font_type* font_;
				const font_type* fontj_;
				int width_;
				int height_;
				int OffsetX_;
				int OffsetY_;
				double scaleh_;
				double scalev_;
				
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
					SHORT_type PosX;
					SHORT_type PosY;
					SHORT_type BrushObjectID;
					SHORT_type FontObjectID;
					SHORT_type PenObjectID;
					byte_type BGRed;
					byte_type BGGreen;
					byte_type BGBlue;
					
					current_status() :
						OrgX(0), OrgY(0), ExtX(0), ExtY(0), PosX(0), PosY(0),
						BrushObjectID(0), FontObjectID(0), PenObjectID(0),
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
				//  PenObject
				/* ------------------------------------------------------------- */
				struct PenObject {
					WORD_type Style;
					DWORD_type Width;
					byte_type Red;
					byte_type Green;
					byte_type Blue;
					bool StoredFlg;
					
					PenObject() :
						Style(0), Width(0), Red(0), Green(0), Blue(0),
						StoredFlg(false) {}
				};
				PenObject PenSwap_;
				std::vector<PenObject> PenObjectTable_;
				
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
				//  xmakestream
				/* ------------------------------------------------------------- */
				template <class OutStream>
				bool xmakestream(OutStream& out) {
					while(!in_.fail()){
						xgetdata<DWORD_type>(CrFunc_.RecordSize);
						xgetdata<WORD_type>(CrFunc_.FunctionNumber);
					 	CrFunc_.Parameters.clear();
						for(int i=1; i<=(int)CrFunc_.RecordSize-3; i++){
							WORD_type Parameter;
							xgetdata<WORD_type>(Parameter);
							CrFunc_.Parameters.push_back(Parameter);
						}
						xGDI(out);
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
				
				/* ------------------------------------------------------------- */
				//  xGDI
				/* ------------------------------------------------------------- */
				template <class OutStream>
				bool xGDI(OutStream& out){
					clx::logger::trace(TRACEF("GDI Function Record: 0x%08x (%d)",
						CrFunc_.FunctionNumber, CrFunc_.Parameters.size()));
					
					switch (CrFunc_.FunctionNumber) {
					case 0x020B: 
						GDI_SetWindowOrg(out);	break;
					case 0x020C:
						GDI_SetWindowExt(out);	break;
					case 0x02FC:
						GDI_CreateBrushIndirect(out);	break;
					case 0x012D:
						GDI_SelectObject(out);	break;
					case 0x02FA:
						GDI_CreatePenIndirect(out);	break;
					case 0x0104:
						GDI_SetROP2(out);	break;
					case 0x0106:
						GDI_SetPolyFillMode(out);	break;
					case 0x0324:
						GDI_Polygon(out);	break;
					case 0x01F0:
						GDI_DeleteObject(out);	break;
					case 0x0105:
						GDI_SetRelabs(out);	break;
					case 0x0102:
						GDI_SetBkMode(out);	break;
					case 0x0201:
						GDI_SetBkColor(out);	break;
					case 0x012E:
						GDI_SetTextAlign(out);	break;
					case 0x0626:
						GDI_Escape(out);	break;
					case 0x001E:
						GDI_SaveDC(out);	break;
					case 0x0127:
						GDI_RestoreDC(out);	break;
					case 0x0940:
						GDI_DibBitBLT(out);	break;
					case 0x0103:
						GDI_SetMapMode(out);	break;
					case 0x02FB:
						GDI_CreateFontIndirect(out);	break;
					case 0x0209:
						GDI_SetTextColor(out);	break;
					case 0x0107:
						GDI_SetStretchBLTMode(out);	break;
					case 0x0214:
						GDI_MoveTo(out);	break;
					case 0x0538:
						GDI_PolyPolygon(out);	break;
					case 0x0A32:
						GDI_ExtTextOut(out);	break;
					case 0x0418:
						GDI_Ellipse(out);	break;
					case 0x0416:
						GDI_IntersectClipRect(out);	break;
					case 0x0213:
						break;
						//GDI_LineTo(out);	break;
					case 0x0B41:
						GDI_DIBStretchBlt(out);	break;
					case 0x0325:
						GDI_PolyLine(out);	break;
					case 0x0F43:
						GDI_StretchDIB(out);	break;
					case 0x0000:
						return true;
					default:
						break;
					}
					return false;
				}
				
				/* GDI_SetWindowOrg */
				template <class OutStream>
				bool GDI_SetWindowOrg(OutStream& out){
					CrStat_.OrgX = (short)CrFunc_.Parameters[1];
					CrStat_.OrgY = (short)CrFunc_.Parameters[0];
					return true;
				}
				
				/* GDI_SetWindowExt */
				template <class OutStream>
				bool GDI_SetWindowExt(OutStream& out){
					CrStat_.ExtX = (short)CrFunc_.Parameters[1];
					CrStat_.ExtY = (short)CrFunc_.Parameters[0];
					return true;
				}
				
				/* GDI_SelectObject */
				template <class OutStream>
				bool GDI_SelectObject(OutStream& out){
					//CrStat_.ObjectID = (SHORT_type)CrFunc_.Parameters[0];
					if(!BrushSwap_.StoredFlg){
						CrStat_.BrushObjectID = (SHORT_type)CrFunc_.Parameters[0];
						BrushSwap_.StoredFlg = true;
						BrushObjectTable_[CrStat_.BrushObjectID] = BrushSwap_;
					}else if(!PenSwap_.StoredFlg){
						CrStat_.PenObjectID = (SHORT_type)CrFunc_.Parameters[0];
						PenSwap_.StoredFlg = true;
						PenObjectTable_[CrStat_.PenObjectID] = PenSwap_;
					}else if(!FontSwap_.StoredFlg){
						CrStat_.FontObjectID = (SHORT_type)CrFunc_.Parameters[0];
						FontSwap_.StoredFlg = true;
						FontObjectTable_[CrStat_.FontObjectID] = FontSwap_;
					}
					if(BrushObjectTable_[(SHORT_type)CrFunc_.Parameters[0]].StoredFlg){
						CrStat_.BrushObjectID = (SHORT_type)CrFunc_.Parameters[0];
					}else if(PenObjectTable_[(SHORT_type)CrFunc_.Parameters[0]].StoredFlg){
						CrStat_.PenObjectID = (SHORT_type)CrFunc_.Parameters[0];
					}else if(FontObjectTable_[(SHORT_type)CrFunc_.Parameters[0]].StoredFlg){
						CrStat_.FontObjectID = (SHORT_type)CrFunc_.Parameters[0];
					}
					return true;
				}
				
				/* GDI_DeleteObject */
				template <class OutStream>
				bool GDI_DeleteObject(OutStream& out){
					BrushObjectTable_[(SHORT_type)CrFunc_.Parameters[0]].StoredFlg = false;
					PenObjectTable_[(SHORT_type)CrFunc_.Parameters[0]].StoredFlg = false;
					FontObjectTable_[(SHORT_type)CrFunc_.Parameters[0]].StoredFlg = false;
					return true;
				}
				
				/* GDI_CreateBrushIndirect */
				template <class OutStream>
				bool GDI_CreateBrushIndirect(OutStream& out){
					BrushSwap_.Red = xdivide<byte_type>(CrFunc_.Parameters[1],0);
					BrushSwap_.Green = xdivide<byte_type>(CrFunc_.Parameters[1],1);
					BrushSwap_.Blue = xdivide<byte_type>(CrFunc_.Parameters[2],0);
					BrushSwap_.StoredFlg = false;
					
					return true;
				}
				
				/* GDI_CreatePenIndirectt */
				template <class OutStream>
				bool GDI_CreatePenIndirect(OutStream& out){
					PenSwap_.Style = (WORD_type)CrFunc_.Parameters[0];
					PenSwap_.Width = CrFunc_.Parameters[1];
					PenSwap_.Red = xdivide<byte_type>(CrFunc_.Parameters[3],0);
					PenSwap_.Green = xdivide<byte_type>(CrFunc_.Parameters[3],1);
					PenSwap_.Blue = xdivide<byte_type>(CrFunc_.Parameters[4],0);
					PenSwap_.StoredFlg = false;
					
					return true;
				}
				
				/* GDI_SetROP2 */
				template <class OutStream>
				bool GDI_SetROP2(OutStream& out){
					return true;
				}
				
				/* GDI_SetPolyFillMode */
				template <class OutStream>
				bool GDI_SetPolyFillMode(OutStream& out){
					return true;
				}
				
				/* GDI_Polygon */
				template <class OutStream>
				bool GDI_Polygon(OutStream& out){
					out << LITERAL("q") << std::endl;
					out << fmt(LITERAL("%f 0 0 %f %d %d cm")) % scaleh_ % scalev_ % OffsetX_ % OffsetY_ << std::endl;
					double Red = ((double)BrushObjectTable_[CrStat_.BrushObjectID].Red)/255;
					double Green = ((double)BrushObjectTable_[CrStat_.BrushObjectID].Green)/255;
					double Blue = ((double)BrushObjectTable_[CrStat_.BrushObjectID].Blue)/255;
					out << fmt(LITERAL("%f %f %f rg")) % Red % Green % Blue << std::endl;
					
					double weight = PenObjectTable_[CrStat_.PenObjectID].Width * (width_/(double)CrStat_.ExtX);
					if (PenObjectTable_[CrStat_.PenObjectID].Width > 0) {
						out << fmt(LITERAL("%f w")) % weight << std::endl;
						Red = ((double)PenObjectTable_[CrStat_.PenObjectID].Red)/255;
						Green = ((double)PenObjectTable_[CrStat_.PenObjectID].Green)/255;
						Blue = ((double)PenObjectTable_[CrStat_.PenObjectID].Blue)/255;
						out << fmt(LITERAL("%f %f %f RG")) % Red % Green % Blue << std::endl;
					}
					double X = ((SHORT_type)CrFunc_.Parameters[1]-CrStat_.OrgX)*(width_/(double)CrStat_.ExtX);
					double Y = -(((SHORT_type)CrFunc_.Parameters[2]-CrStat_.OrgY-CrStat_.ExtY)*(height_/(double)CrStat_.ExtY));
					out << fmt(LITERAL("%f %f m")) % X % Y << std::endl;
					for(size_type i=3; i<CrFunc_.Parameters.size(); i=i+2){
						X = ((SHORT_type)CrFunc_.Parameters[i]-CrStat_.OrgX)*(width_/(double)CrStat_.ExtX);
						Y = -(((SHORT_type)CrFunc_.Parameters[i+1]-CrStat_.OrgY-CrStat_.ExtY)*(height_/(double)CrStat_.ExtY));
						out << fmt(LITERAL("%f %f l")) % X % Y << std::endl;
					}
					
					if (PenObjectTable_[CrStat_.PenObjectID].Width > 0) out << LITERAL("b") << std::endl;
					else out << LITERAL("f") << std::endl;
					out << LITERAL("Q") << std::endl;
					
					return true;
				}
				
				/* GDI_PolyLine */
				template <class OutStream>
				bool GDI_PolyLine(OutStream& out){
					out << LITERAL("q") << std::endl;
					out << fmt(LITERAL("%f 0 0 %f %d %d cm")) % scaleh_ % scalev_ % OffsetX_ % OffsetY_ << std::endl;
					
					double weight = PenObjectTable_[CrStat_.PenObjectID].Width * (width_/(double)CrStat_.ExtX);
					out << fmt(LITERAL("%f w")) % weight << std::endl;
					double Red = ((double)PenObjectTable_[CrStat_.PenObjectID].Red)/255;
					double Green = ((double)PenObjectTable_[CrStat_.PenObjectID].Green)/255;
					double Blue = ((double)PenObjectTable_[CrStat_.PenObjectID].Blue)/255;
					out << fmt(LITERAL("%f %f %f RG")) % Red % Green % Blue << std::endl;
					double X = ((SHORT_type)CrFunc_.Parameters[1]-CrStat_.OrgX)*(width_/(double)CrStat_.ExtX);
					double Y = -(((SHORT_type)CrFunc_.Parameters[2]-CrStat_.OrgY-CrStat_.ExtY)*(height_/(double)CrStat_.ExtY));
					out << fmt(LITERAL("%f %f m")) % X % Y << std::endl;
					for(size_type i=3; i<CrFunc_.Parameters.size(); i=i+2){
						X = ((SHORT_type)CrFunc_.Parameters[i]-CrStat_.OrgX)*(width_/(double)CrStat_.ExtX);
						Y = -(((SHORT_type)CrFunc_.Parameters[i+1]-CrStat_.OrgY-CrStat_.ExtY)*(height_/(double)CrStat_.ExtY));
						out << fmt(LITERAL("%f %f l")) % X % Y << std::endl;
					}
					out << LITERAL("S") << std::endl;
					out << LITERAL("Q") << std::endl;
					
					return true;
				}
				
				/* GDI_SetRelabs */
				template <class OutStream>
				bool GDI_SetRelabs(OutStream& out){
					return true;
				}
				
				/* GDI_SetBkMode */
				template <class OutStream>
				bool GDI_SetBkMode(OutStream& out){
					return true;
				}
				
				/* GDI_SetBkColor */
				template <class OutStream>
				bool GDI_SetBkColor(OutStream& out){
					/*CrStat_.BGRed = xdivide<byte_type>(CrFunc_.Parameters[0],0);
					CrStat_.BGGreen = xdivide<byte_type>(CrFunc_.Parameters[0],1);
					CrStat_.BGBlue = xdivide<byte_type>(CrFunc_.Parameters[1],0);
					out << LITERAL("q") << std::endl;
					out << fmt(LITERAL("%f 0 0 %f %d %d cm")) % scaleh_ % scalev_ % OffsetX_ % OffsetY_ << std::endl;
					double Red = ((double)CrStat_.BGRed)/255;
					double Green = ((double)CrStat_.BGGreen)/255;
					double Blue = ((double)CrStat_.BGBlue)/255;
					out << fmt(LITERAL("%f %f %f rg")) % Red % Green % Blue << std::endl;
					out << LITERAL("0 0 m") << std::endl;
					out << fmt(LITERAL("%d 0 l")) % width_ << std::endl;
					out << fmt(LITERAL("%d %d l")) % width_ % height_ << std::endl;
					out << fmt(LITERAL("0 %d l")) % height_ << std::endl;
					out << LITERAL("f") << std::endl;
					out << LITERAL("Q") << std::endl;*/
					return true;
				}
				
				/* GDI_SetTextAlign */
				template <class OutStream>
				bool GDI_SetTextAlign(OutStream& out){
					return true;
				}
				
				/* GDI_Escape */
				template <class OutStream>
				bool GDI_Escape(OutStream& out){
					return true;
				}
				
				/* GDI_SaveDC */
				template <class OutStream>
				bool GDI_SaveDC(OutStream& out){
					return true;
				}
				
				/* GDI_RestoreDC */
				template <class OutStream>
				bool GDI_RestoreDC(OutStream& out){
					return true;
				}
				
				/* GDI_DibBitBLT */
				template <class OutStream>
				bool GDI_DibBitBLT(OutStream& out){
					return true;
				}
				
				/* GDI_SetMapMode */
				template <class OutStream>
				bool GDI_SetMapMode(OutStream& out){
					return true;
				}
				
				/* GDI_CreateFontIndirect */
				template <class OutStream>
				bool GDI_CreateFontIndirect(OutStream& out){
					FontSwap_.Height = (SHORT_type)CrFunc_.Parameters[0];
					FontSwap_.Width = (SHORT_type)CrFunc_.Parameters[1];
					FontSwap_.StoredFlg = false;
					
					return true;
				}
				
				/* GDI_SetTextColor */
				template <class OutStream>
				bool GDI_SetTextColor(OutStream& out){
					return true;
				}
				
				/* GDI_SetStretchBLTMode */
				template <class OutStream>
				bool GDI_SetStretchBLTMode(OutStream& out){
					return true;
				}
				
				/* GDI_MoveTo */
				template <class OutStream>
				bool GDI_MoveTo(OutStream& out){
					CrStat_.PosX = (SHORT_type)CrFunc_.Parameters[1];
					CrStat_.PosY = (SHORT_type)CrFunc_.Parameters[0];
					return true;
				}
				
				/* GDI_PolyPolygon */
				template <class OutStream>
				bool GDI_PolyPolygon(OutStream& out){
					int NumberOfPolygons = (int)CrFunc_.Parameters[0];
					int pos = NumberOfPolygons+1;
					double weight = PenObjectTable_[CrStat_.PenObjectID].Width * (width_/(double)CrStat_.ExtX);
					for(int j=1; j<=NumberOfPolygons; j++){
						int aPointsPerPolygon = (int)CrFunc_.Parameters[j];
						out << LITERAL("q") << std::endl;
						out << fmt(LITERAL("%f 0 0 %f %d %d cm")) % scaleh_ % scalev_ % OffsetX_ % OffsetY_ << std::endl;
						double Red = ((double)BrushObjectTable_[CrStat_.BrushObjectID].Red)/255;
						double Green = ((double)BrushObjectTable_[CrStat_.BrushObjectID].Green)/255;
						double Blue = ((double)BrushObjectTable_[CrStat_.BrushObjectID].Blue)/255;
						out << fmt(LITERAL("%f %f %f rg")) % Red % Green % Blue << std::endl;
						
						if (PenObjectTable_[CrStat_.PenObjectID].Width > 0) {
							out << fmt(LITERAL("%f w")) % weight << std::endl;
							Red = ((double)PenObjectTable_[CrStat_.PenObjectID].Red)/255;
							Green = ((double)PenObjectTable_[CrStat_.PenObjectID].Green)/255;
							Blue = ((double)PenObjectTable_[CrStat_.PenObjectID].Blue)/255;
							out << fmt(LITERAL("%f %f %f RG")) % Red % Green % Blue << std::endl;
						}
						
						double X = ((short)CrFunc_.Parameters[pos++]-CrStat_.OrgX)*(width_/(double)CrStat_.ExtX);
						double Y = -((short)CrFunc_.Parameters[pos++]-CrStat_.OrgY-CrStat_.ExtY)*(height_/(double)CrStat_.ExtY);
						out << fmt(LITERAL("%f %f m")) % X % Y << std::endl;
						for(int i=1; i<aPointsPerPolygon; i++){
							X = ((short)CrFunc_.Parameters[pos++]-CrStat_.OrgX)*(width_/(double)CrStat_.ExtX);
							Y = -((short)CrFunc_.Parameters[pos++]-CrStat_.OrgY-CrStat_.ExtY)*(height_/(double)CrStat_.ExtY);
							out << fmt(LITERAL("%f %f l")) % X % Y << std::endl;
						}
						if(j==NumberOfPolygons){
							if (PenObjectTable_[CrStat_.PenObjectID].Width > 0) {
								out << LITERAL("b") << std::endl;
							}
							else out << LITERAL("f") << std::endl;
						}else{
							out << LITERAL("W") << std::endl;
						}
						out << LITERAL("Q") << std::endl;
					}
					return true;
				}
				
				/* GDI_ExtTextOut */
				template <class OutStream>
				bool GDI_ExtTextOut(OutStream& out){

					// make text contents
					int font_size = (int)(((SHORT_type)FontObjectTable_[CrStat_.FontObjectID].Height)*(height_/CrStat_.ExtY)*scalev_);
					if(font_size<0)font_size *= -1;
					double X = OffsetX_ + (((SHORT_type)CrFunc_.Parameters[1]-CrStat_.OrgX)*(width_/(double)CrStat_.ExtX)*scaleh_);
					double Y = OffsetY_ - (((SHORT_type)CrFunc_.Parameters[0]-CrStat_.OrgY-CrStat_.ExtY)*(height_/(double)CrStat_.ExtY)*scalev_);
					std::string str = win32conv(&CrFunc_.Parameters[4],&CrFunc_.Parameters[(int)(CrFunc_.Parameters[2]/2.0)+4]);
					out << LITERAL("BT") << std::endl;
					text_contents txt(str, coordinate(X,Y), *font_, *fontj_, false);
					txt.font_size(font_size);
					txt(out);
					out << LITERAL("ET") << std::endl;
					return true;
				}
				
				/* GDI_Ellipse */
				template <class OutStream>
				bool GDI_Ellipse(OutStream& out){
					double X = ((((SHORT_type)CrFunc_.Parameters[1] + (SHORT_type)CrFunc_.Parameters[3])/2.0)-CrStat_.OrgX)*(width_/(double)CrStat_.ExtX);
					double Y = -(((((SHORT_type)CrFunc_.Parameters[0] + (SHORT_type)CrFunc_.Parameters[2])/2.0)-CrStat_.OrgY-CrStat_.ExtY)*(height_/(double)CrStat_.ExtY));
					double w = ((SHORT_type)CrFunc_.Parameters[1] - (SHORT_type)CrFunc_.Parameters[3])*(width_/(double)CrStat_.ExtX)/2.0;
					double h = ((SHORT_type)CrFunc_.Parameters[0] - (SHORT_type)CrFunc_.Parameters[2])*(height_/(double)CrStat_.ExtY)/2.0;
					double Red = ((double)BrushObjectTable_[CrStat_.BrushObjectID].Red)/255;
					double Green = ((double)BrushObjectTable_[CrStat_.BrushObjectID].Green)/255;
					double Blue = ((double)BrushObjectTable_[CrStat_.BrushObjectID].Blue)/255;
					out << LITERAL("q") << std::endl;
					out << fmt(LITERAL("%f 0 0 %f %d %d cm")) % scaleh_ % scalev_ % OffsetX_ % OffsetY_ << std::endl;
					out << fmt(LITERAL("%f %f %f rg")) % Red % Green % Blue << std::endl;
					detail::draw_ellipse(out, X, Y, w, h);
					out << LITERAL("f") << std::endl;
					out << LITERAL("Q") << std::endl;
					
					return true;
				}
				
				/* GDI_IntersectClipRect */
				template <class OutStream>
				bool GDI_IntersectClipRect(OutStream& out){
					return true;
				}
				
				/* GDI_LineTo */
				template <class OutStream>
				bool GDI_LineTo(OutStream& out){
					double X = ((SHORT_type)CrFunc_.Parameters[1]-CrStat_.OrgX)*(width_/(double)CrStat_.ExtX);
					double Y = -(((SHORT_type)CrFunc_.Parameters[0]-CrStat_.OrgY-CrStat_.ExtY)*(height_/(double)CrStat_.ExtY));
					double Red = ((double)PenObjectTable_[CrStat_.PenObjectID].Red)/255;
					double Green = ((double)PenObjectTable_[CrStat_.PenObjectID].Green)/255;
					double Blue = ((double)PenObjectTable_[CrStat_.PenObjectID].Blue)/255;
					out << LITERAL("q") << std::endl;
					out << fmt(LITERAL("%f w")) % (double)PenObjectTable_[CrStat_.PenObjectID].Width << std::endl;
					out << fmt(LITERAL("%f 0 0 %f %d %d cm")) % scaleh_ % scalev_ % OffsetX_ % OffsetY_ << std::endl;
					out << fmt(LITERAL("%f %f %f RG")) % Red % Green % Blue << std::endl;
					out << fmt(LITERAL("%f %f m")) 
						% ((CrStat_.PosX-CrStat_.OrgX)*(width_/(double)CrStat_.ExtX)) 
						% (-((CrStat_.PosY-CrStat_.OrgY-CrStat_.ExtY)*(height_/(double)CrStat_.ExtY))) << std::endl;
					out << fmt(LITERAL("%f %f l")) % X % Y << std::endl;
					out << LITERAL("s") << std::endl;
					out << LITERAL("Q") << std::endl;
					return true;
				}
				
				/* GDI_DIBStretchBlt */
				template <class OutStream>
				bool GDI_DIBStretchBlt(OutStream& out){
					
					// Record Info
					//DWORD_type RasterOperation = xcombine<DWORD_type>(CrFunc_.Parameters[0],CrFunc_.Parameters[1]);
					//SHORT_type SrcHeight  = (SHORT_type)CrFunc_.Parameters[2];
					//SHORT_type SrcWidth   = (SHORT_type)CrFunc_.Parameters[3];
					//SHORT_type YSrc       = (SHORT_type)CrFunc_.Parameters[4];
					//SHORT_type XSrc       = (SHORT_type)CrFunc_.Parameters[5];
					//SHORT_type DestHeight = (SHORT_type)CrFunc_.Parameters[6];
					//SHORT_type DestWidth  = (SHORT_type)CrFunc_.Parameters[7];
					//SHORT_type YDest      = (SHORT_type)CrFunc_.Parameters[8];
					//SHORT_type XDest      = (SHORT_type)CrFunc_.Parameters[9];
					
					// Bitmap Header Info
					DWORD_type HeaderSize = xcombine<DWORD_type>(CrFunc_.Parameters[10],CrFunc_.Parameters[11]);
					LONG_type Width = 0;
					LONG_type Height = 0;
					DWORD_type Planes = 0;
					DWORD_type BitCount = 0;
					DWORD_type Compression = 0;
					DWORD_type ImageSize = 0;
					DWORD_type XPelsPerMeter = 0;
					DWORD_type YPelsPerMeter = 0;
					DWORD_type ColorUsed = 0;
					DWORD_type ColorImportant = 0;
					bool amp = xcompare(HeaderSize, (DWORD_type)0x0000000C);
					if(amp){
						// BitmapCoreHeader
						return true;
					}else{
						// BitmapInfoHeader
						Width = xcombine<LONG_type>(CrFunc_.Parameters[12],CrFunc_.Parameters[13]);
						Height = xcombine<LONG_type>(CrFunc_.Parameters[14],CrFunc_.Parameters[15]);
						Planes = (DWORD_type)CrFunc_.Parameters[16];
						BitCount = (DWORD_type)CrFunc_.Parameters[17];
						Compression = xcombine<DWORD_type>(CrFunc_.Parameters[18],CrFunc_.Parameters[19]);
						ImageSize = xcombine<DWORD_type>(CrFunc_.Parameters[20],CrFunc_.Parameters[21]);
						XPelsPerMeter = xcombine<DWORD_type>(CrFunc_.Parameters[22],CrFunc_.Parameters[23]);
						YPelsPerMeter = xcombine<DWORD_type>(CrFunc_.Parameters[24],CrFunc_.Parameters[25]);
						ColorUsed = xcombine<DWORD_type>(CrFunc_.Parameters[26],CrFunc_.Parameters[27]);
						ColorImportant = xcombine<DWORD_type>(CrFunc_.Parameters[28],CrFunc_.Parameters[29]);
					}
					
					// Colors
					amp = xcompare(BitCount, (DWORD_type)0x00000018);
					if(!amp){
						// Setting Colors
						return true;
					}
					
					// aData
					if(amp){
						// 24bit RGB
						out << LITERAL("q") << std::endl;
						out << fmt(LITERAL("%f 0 0 %f %d %d cm")) % (width_*scaleh_) % (height_*scalev_) % OffsetX_ % OffsetY_ << std::endl;
						out << LITERAL("BI") << std::endl;
						out << fmt(LITERAL("/W %d")) % Width << std::endl;
						out << fmt(LITERAL("/H %d")) % Height << std::endl;
						out << LITERAL("/BPC 8") << std::endl;
						out << LITERAL("/CS /RGB") << std::endl;
						out << LITERAL("ID") << std::endl;
						DWORD_type WrittenBytes = 0;
						int ArrayNumOfLine = (Width*3 + (4-(Width*3)%4)) / 2;
						double lim = ArrayNumOfLine/3.0 - 1;
						if(ArrayNumOfLine % 3 == 0) lim = ArrayNumOfLine / 3.0;
						for(int i=1; i<=Height; i++){
							int d = CrFunc_.Parameters.size() - i * ArrayNumOfLine;
							for(int j=0; j<lim; j++){
								out << xdivide<byte_type>(CrFunc_.Parameters[d+j*3+1],0);
								out << xdivide<byte_type>(CrFunc_.Parameters[d+j*3],1);
								out << xdivide<byte_type>(CrFunc_.Parameters[d+j*3],0);
								WrittenBytes += 3;
								if(WrittenBytes % (Width*3) == 0) break;
								out << xdivide<byte_type>(CrFunc_.Parameters[d+j*3+2],1);
								out << xdivide<byte_type>(CrFunc_.Parameters[d+j*3+2],0);
								out << xdivide<byte_type>(CrFunc_.Parameters[d+j*3+1],1);
								WrittenBytes += 3;
							}
							if(ArrayNumOfLine % 3 == 2){
								out << xdivide<byte_type>(CrFunc_.Parameters[d+ArrayNumOfLine-1],0);
								out << xdivide<byte_type>(CrFunc_.Parameters[d+ArrayNumOfLine-2],1);
								out << xdivide<byte_type>(CrFunc_.Parameters[d+ArrayNumOfLine-2],0);
								WrittenBytes += 3;
							}
						}
						out << LITERAL(">") << std::endl;
						out << LITERAL("EI") << std::endl;
						out << LITERAL("Q") << std::endl;
					}
					return true;
				}
				
				/* GDI_StretchDIB */
				template <class OutStream>
				bool GDI_StretchDIB(OutStream& out){
					
					// Record Info
					//DWORD_type RasterOperation = xcombine<DWORD_type>(CrFunc_.Parameters[0],CrFunc_.Parameters[1]);
					//SHORT_type ColorUsage = (SHORT_type)CrFunc_.Parameters[2];
					//SHORT_type SrcHeight  = (SHORT_type)CrFunc_.Parameters[3];
					//SHORT_type SrcWidth   = (SHORT_type)CrFunc_.Parameters[4];
					//SHORT_type YSrc       = (SHORT_type)CrFunc_.Parameters[5];
					//SHORT_type XSrc       = (SHORT_type)CrFunc_.Parameters[6];
					SHORT_type DestHeight = (SHORT_type)CrFunc_.Parameters[7];
					SHORT_type DestWidth  = (SHORT_type)CrFunc_.Parameters[8];
					SHORT_type YDest      = (SHORT_type)CrFunc_.Parameters[9];
					SHORT_type XDest      = (SHORT_type)CrFunc_.Parameters[10];
					
					// Bitmap Header Info
					DWORD_type HeaderSize = xcombine<DWORD_type>(CrFunc_.Parameters[11],CrFunc_.Parameters[12]);
					LONG_type Width = 0;
					LONG_type Height = 0;
					DWORD_type Planes = 0;
					DWORD_type BitCount = 0;
					DWORD_type Compression = 0;
					DWORD_type ImageSize = 0;
					DWORD_type XPelsPerMeter = 0;
					DWORD_type YPelsPerMeter = 0;
					DWORD_type ColorUsed = 0;
					DWORD_type ColorImportant = 0;
					bool amp = xcompare(HeaderSize, (DWORD_type)0x0000000C);
					if(amp){
						// BitmapCoreHeader
						return true;
					}else{
						// BitmapInfoHeader
						Width = xcombine<LONG_type>(CrFunc_.Parameters[13],CrFunc_.Parameters[14]);
						Height = xcombine<LONG_type>(CrFunc_.Parameters[15],CrFunc_.Parameters[16]);
						Planes = (DWORD_type)CrFunc_.Parameters[17];
						BitCount = (DWORD_type)CrFunc_.Parameters[18];
						Compression = xcombine<DWORD_type>(CrFunc_.Parameters[19],CrFunc_.Parameters[20]);
						ImageSize = xcombine<DWORD_type>(CrFunc_.Parameters[21],CrFunc_.Parameters[22]);
						XPelsPerMeter = xcombine<DWORD_type>(CrFunc_.Parameters[23],CrFunc_.Parameters[24]);
						YPelsPerMeter = xcombine<DWORD_type>(CrFunc_.Parameters[25],CrFunc_.Parameters[26]);
						ColorUsed = xcombine<DWORD_type>(CrFunc_.Parameters[27],CrFunc_.Parameters[28]);
						ColorImportant = xcombine<DWORD_type>(CrFunc_.Parameters[29],CrFunc_.Parameters[30]);
					}
					
					// Colors
					amp = xcompare(BitCount, (DWORD_type)0x00000018);
					if(!amp){
						// Setting Colors
						return true;
					}
					
					// aData
					if(amp){
						// 24bit RGB
						out << LITERAL("q") << std::endl;
						out << fmt(LITERAL("%f 0 0 %f %d %d cm")) % (DestWidth*scaleh_) % (DestHeight*scalev_) % (XDest*scaleh_) % (-(DestHeight+YDest)*scalev_) << std::endl;
						out << LITERAL("BI") << std::endl;
						out << fmt(LITERAL("/W %d")) % Width << std::endl;
						out << fmt(LITERAL("/H %d")) % Height << std::endl;
						out << LITERAL("/BPC 8") << std::endl;
						out << LITERAL("/CS /RGB") << std::endl;
						out << LITERAL("ID") << std::endl;
						DWORD_type WrittenBytes = 0;
						int ArrayNumOfLine = (Width*3 + (4-(Width*3)%4)) / 2;
						double lim = ArrayNumOfLine/3.0 - 1;
						if(ArrayNumOfLine % 3 == 0) lim = ArrayNumOfLine / 3.0;
						for(int i=1; i<=Height; i++){
							int d = CrFunc_.Parameters.size() - i * ArrayNumOfLine;
							for(int j=0; j<lim; j++){
								out << xdivide<byte_type>(CrFunc_.Parameters[d+j*3+1],0);
								out << xdivide<byte_type>(CrFunc_.Parameters[d+j*3],1);
								out << xdivide<byte_type>(CrFunc_.Parameters[d+j*3],0);
								WrittenBytes += 3;
								if(WrittenBytes % (Width*3) == 0) break;
								out << xdivide<byte_type>(CrFunc_.Parameters[d+j*3+2],1);
								out << xdivide<byte_type>(CrFunc_.Parameters[d+j*3+2],0);
								out << xdivide<byte_type>(CrFunc_.Parameters[d+j*3+1],1);
								WrittenBytes += 3;
							}
							if(ArrayNumOfLine % 3 == 2){
								out << xdivide<byte_type>(CrFunc_.Parameters[d+ArrayNumOfLine-1],0);
								out << xdivide<byte_type>(CrFunc_.Parameters[d+ArrayNumOfLine-2],1);
								out << xdivide<byte_type>(CrFunc_.Parameters[d+ArrayNumOfLine-2],0);
								WrittenBytes += 3;
							}
						}
						out << LITERAL(">") << std::endl;
						out << LITERAL("EI") << std::endl;
						out << LITERAL("Q") << std::endl;
					}
					return true;
				}
			};
		}
	}
}

#endif // FAML_PDF_CONTENTS_WMF_H
