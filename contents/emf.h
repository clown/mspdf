/* ------------------------------------------------------------------------- */
/*
 *  contents/emf.h
 *
 *  Copyright (c) 2009, Four and More. All rights reserved.
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PDF_CONTENTS_EMF_H
#define FAML_PDF_CONTENTS_EMF_H

#include "../config.h"
#include "../coordinate.h"
#include <algorithm>
#include <string>
#include <iterator>
#include <istream>
#include <sstream>
#include "clx/literal.h"
#include "clx/format.h"
#include "clx/utility.h"

namespace faml {
	namespace pdf {
		namespace contents {
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
				
				basic_emf(istream_type& in) :
					in_(in), origin_(), width_(0), height_(0), OffsetX_(0), OffsetY_(0),
					scaleh_(1.0), scalev_(1.0), rot_(0.0), flip_(0) {}
				
				virtual ~basic_emf() throw() {}
				
				template <class OutStream>
				bool operator()(OutStream& out) {
					return this->xmakestream(out);
				}
				
				/* ------------------------------------------------------------- */
				//  read_header
				/* ------------------------------------------------------------- */
				bool read_header() {
					
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
						
				 		margin_Left_ = (double)(FrameLeft * 0.032);
				 		margin_Top_ = (double)(FrameTop * 0.032);
				 		margin_Right_ = (double)(FrameRight * 0.032);
				 		margin_Bottom_ = (double)(FrameBottom * 0.032);
				 		//width_ = (size_type)(BoundsRight-BoundsLeft+1);
				 		//height_ = (size_type)(BoundsBottom-BoundsTop+1);
				 		width_ = (margin_Right_ - margin_Left_);
				 		height_ = (margin_Bottom_ - margin_Top_);
				 		
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
				 		throw std::runtime_error("Exception : This file is not Enhanced Metafile Format File.\n");
				 		return false;
				 	}
					
					return true;
				}
				
				void origin(const coordinate& cp) {
					origin_ = cp;
					//OffsetX_ = static_cast<int>(cp.x());
					//OffsetY_ = static_cast<int>(cp.y());
				}
				
				void width(double cp) {
					if (width_ > 0.0) scaleh_ = cp / static_cast<double>(width_);
					else width_ = cp;
				}
				
				void height(double cp) {
					if (height_ > 0.0) scalev_ = cp / static_cast<double>(height_);
					else height_ = cp;
				}
				
				void scale(double cp) {
					scaleh_ = cp;
					scalev_ = cp;
				}
				
				void scale(double h, double v) {
					scaleh_ = h;
					scalev_ = v;
				}
				
				void rotation(double cp) {
					rot_ = cp;
				}
				
				void flip(int cp) {
					flip_ = cp;
				}
				
				double width() const { return width_; }
				double height() const { return height_; }
				
			private:
				typedef clx::basic_format<CharT, Traits> fmt;
				
				istream_type& in_;
				coordinate origin_;
				double width_;
				double height_;
				double OffsetX_;
				double OffsetY_;
				double margin_Left_;
				double margin_Top_;
				double margin_Right_;
				double margin_Bottom_;
				double scaleh_;
				double scalev_;
				double rot_;
				int flip_;
				
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
					//LONG_type PosX;
					//LONG_type PosY;
					double PosX;
					double PosY;
					SHORT_type ExtX;
					SHORT_type ExtY;
					SHORT_type ObjectID;
					byte_type BGRed;
					byte_type BGGreen;
					byte_type BGBlue;
					bool MultiPath;
					int Mode;
					current_status() : OrgX(0),OrgY(0),PosX(0),PosY(0),ExtX(0),ExtY(0),
						ObjectID(0),BGRed(0),BGGreen(0),BGBlue(0),MultiPath(false),Mode(0) {}
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
				struct PenObject {
					byte_type Red;
					byte_type Green;
					byte_type Blue;
					DWORD_type PenStyle;
					LONG_type WidthX;
					LONG_type WidthY;
					bool StoredFlg;
					PenObject() : Red(0),Green(0),Blue(0),PenStyle(0),WidthX(0),WidthY(0),StoredFlg(false){}
				};
				PenObject PenSwap_;
				std::vector<PenObject> PenObjectTable_;
				
				/* ------------------------------------------------------------- */
				//  setorigin
				/* ------------------------------------------------------------- */
				template <class OutStream>
				void setorigin(OutStream& out) {
					static const double pi = 3.141592653589793;
					
					double x = (rot_ > 0.0) ? 2.0 * pi - rot_ : 0.0;
					int h = (flip_ & 0x01) ? -1 : 1;
					int v = (flip_ & 0x02) ? -1 : 1;
					double ox = origin_.x();
					double oy = origin_.y() + height_ * scalev_;
					out << fmt(LITERAL("%f %f %f %f %f %f cm"))
						% (h * std::cos(x)) % (h * std::sin(x))
						% (v * (-std::sin(x))) % (v * std::cos(x))
						% ox % oy
					<< std::endl;
					
					OffsetX_ = (flip_ & 0x01) ? -width_ : 0.0;
					OffsetY_ = (flip_ & 0x02) ? 0.0 : -height_;
					
					out << fmt(LITERAL("%f 0 0 %f 0 0 cm"))
						% scaleh_ % scalev_
					<< std::endl;
				}
				
				/* ------------------------------------------------------------- */
				//  xmakestream
				/* ------------------------------------------------------------- */
				template <class OutStream>
				bool xmakestream(OutStream& out) {
					out << LITERAL("q") << std::endl;
					this->setorigin(out);
					//out << fmt(LITERAL("%f 0 0 %f %d %d cm")) % scaleh_ % scalev_ % OffsetX_ % OffsetY_ << std::endl;
					
					while(!in_.fail()){
						xgetdata<DWORD_type>(CrFunc_.FunctionNumber);
						xgetdata<DWORD_type>(CrFunc_.RecordSize);
					 	CrFunc_.Parameters.clear();
						for(int i=0; i<(int)CrFunc_.RecordSize-8; i+=4){
							DWORD_type Parameter;
							xgetdata<DWORD_type>(Parameter);
							CrFunc_.Parameters.push_back(Parameter);
						}
						xGDI(out);
					}
					
					out << LITERAL("Q") << std::endl;
					
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
					clx::logger::trace(TRACEF("GDI Function Record: 0x%08x (%d Bytes)",
						CrFunc_.FunctionNumber, CrFunc_.RecordSize));
					
					switch (CrFunc_.FunctionNumber) {
					//case 0x00000046: 
					//	GDI_Comment(out);	break;
					case 0x00000021:
						GDI_SaveDC(out);	break;
					//case 0x00000062:
					//	GDI_SetICMMode(out);	break;
					//case 0x00000015:
					//	GDI_SetStretchBLTMode(out);	break;
					//case 0x00000051:
					//	GDI_StretchedBits(out);	break;
					case 0x00000022:
						GDI_RestoreDC(out);	break;
					//case 0x00000073:
					//	GDI_SetLayout(out);	break;
					//case 0x0000001C:
					//	GDI_SetMetaRGN(out);	break;
					case 0x00000025:
						GDI_SelectObject(out);	break;
					case 0x00000028:
						GDI_DeleteObject(out);	break;
					//case 0x00000030:
					//	GDI_SelectPalette(out);	break;
					//case 0x00000019:
					//	GDI_SetBkColor(out);	break;
					//case 0x00000018:
					//	GDI_SetTextColor(out);	break;
					//case 0x00000011:
					//	GDI_SetMapMode(out);	break;
					//case 0x00000012:
					//	GDI_SetBkMode(out);	break;
					//case 0x00000013:
					//	GDI_SetPolyFillMode(out);	break;
					//case 0x00000014:
					//	GDI_SetROP2(out);	break;
					//case 0x00000016:
					//	GDI_SetTextAlign(out);	break;
					case 0x0000000D:
						GDI_SetBrushOrgEx(out);	break;
					//case 0x0000003A:
					//	GDI_SetMiterLimit(out);	break;
					case 0x0000001B:
						GDI_MoveToEx(out);	break;
					case 0x00000023:
						GDI_SetWorldTransform(out);	break;
					case 0x00000024:
						GDI_ModifyWorldTransform(out);	break;
					//case 0x00000052:
					//	GDI_SetCreateFontIndirectW(out);	break;
					case 0x00000027:
						GDI_CreateBrushIndirect(out);	break;
					case 0x00000056:
						GDI_Polygon16(out);	break;
					//case 0x0000004C:
					//	GDI_BitBLT(out);	break;
					case 0x00000026:
						GDI_CreatePen(out);	break;
					case 0x0000005B:
						GDI_PolyPolygon16(out);	break;
					//case 0x0000004B:
					//	GDI_ExtSelectClipRGN(out);	break;
					case 0x0000000A:
						GDI_SetWindowOrgEx(out);	break;
					case 0x00000009:
						GDI_SetWindowExtEx(out);	break;
					case 0x0000000C:
						GDI_SetViewPortOrgEx(out);	break;
					case 0x0000000B:
						GDI_SetViewPortExtEx(out);	break;
					case 0x0000005F:
						GDI_ExtCreatePen(out);	break;
					case 0x0000003B:
						GDI_BeginPath(out);	break;
					case 0x0000003D:
						GDI_CloseFigure(out);	break;
					case 0x0000003C:
						GDI_EndPath(out);	break;
					case 0x0000003E:
						GDI_FillPath(out);	break;
					case 0x0000003F:
						GDI_StrokeFillPath(out); break;
					case 0x00000040:
						GDI_StrokePath(out); break;
					case 0x00000058:
						GDI_PolyBezierTo16(out);	break;
					case 0x0000000E:
						return true;
					case 0x00000066:
					case 0x00000067:
						break;
					default:
						break;
					}
					return false;
				}
				
				/* GDI_Comment */
				template <class OutStream>
				bool GDI_Comment(OutStream& out){
					return true;
				}
				
				/* GDI_SaveDC */
				template <class OutStream>
				bool GDI_SaveDC(OutStream& out){
					return true;
				}
				
				/* GDI_SetICMMode */
				template <class OutStream>
				bool GDI_SetICMMode(OutStream& out){
					return true;
				}
				
				/* GDI_SetStretchBLTMode */
				template <class OutStream>
				bool GDI_SetStretchBLTMode(OutStream& out){
					return true;
				}
				
				/* GDI_StretchedBits */
				template <class OutStream>
				bool GDI_StretchedBits(OutStream& out){
					return true;
				}
				
				/* GDI_RestoreDC */
				template <class OutStream>
				bool GDI_RestoreDC(OutStream& out){
					return true;
				}
				
				/* GDI_SetLayout */
				template <class OutStream>
				bool GDI_SetLayout(OutStream& out){
					return true;
				}
				
				/* GDI_SetMetaRGN */
				template <class OutStream>
				bool GDI_SetMetaRGN(OutStream& out){
					return true;
				}
				
				/* GDI_ExtSelectClipRGN */
				template <class OutStream>
				bool GDI_ExtSelectClipRGN(OutStream& out){
					return true;
				}
				
				/* GDI_SelectObject */
				template <class OutStream>
				bool GDI_SelectObject(OutStream& out){
					return true;
				}
				
				/* GDI_SelectPalette */
				template <class OutStream>
				bool GDI_SelectPalette(OutStream& out){
					return true;
				}
				
				/* GDI_SetBkColor */
				template <class OutStream>
				bool GDI_SetBkColor(OutStream& out){
					return true;
				}
				
				/* GDI_SetTextColor */
				template <class OutStream>
				bool GDI_SetTextColor(OutStream& out){
					return true;
				}
				
				/* GDI_SetBkMode */
				template <class OutStream>
				bool GDI_SetBkMode(OutStream& out){
					return true;
				}
				
				/* GDI_SetPolyFillMode */
				template <class OutStream>
				bool GDI_SetPolyFillMode(OutStream& out){
					return true;
				}
				
				/* GDI_SetROP2 */
				template <class OutStream>
				bool GDI_SetROP2(OutStream& out){
					return true;
				}
				
				/* GDI_SetTextAlign */
				template <class OutStream>
				bool GDI_SetTextAlign(OutStream& out){
					return true;
				}
				
				/* GDI_SetBrushOrgEx */
				template <class OutStream>
				bool GDI_SetBrushOrgEx(OutStream& out){
					//BrushSwap_.OrgX = (LONG_type)((LONG_type)CrFunc_.Parameters[0]*WTMat_.M[0][0]);
					//BrushSwap_.OrgY = (LONG_type)((LONG_type)CrFunc_.Parameters[1]*WTMat_.M[1][1]);
					BrushSwap_.OrgX = (LONG_type)CrFunc_.Parameters[0];
					BrushSwap_.OrgY = (LONG_type)CrFunc_.Parameters[1];
					return true;
				}
				
				/* GDI_SetMiterLimit */
				template <class OutStream>
				bool GDI_SetMiterLimit(OutStream& out){
					return true;
				}
				
				/* GDI_MoveToEx */
				template <class OutStream>
				bool GDI_MoveToEx(OutStream& out){
					CrStat_.PosX = (LONG_type)CrFunc_.Parameters[0];
					CrStat_.PosY = (LONG_type)CrFunc_.Parameters[1];
					return true;
				}
				
				/* GDI_SetWorldTransform */
				template <class OutStream>
				bool GDI_SetWorldTransform(OutStream& out){
					WTMat_.M[0][0] = clx::float_cast(CrFunc_.Parameters[0]);
					WTMat_.M[0][1] = clx::float_cast(CrFunc_.Parameters[1]);
					WTMat_.M[1][0] = clx::float_cast(CrFunc_.Parameters[2]);
					WTMat_.M[1][1] = clx::float_cast(CrFunc_.Parameters[3]);
					return true;
				}
				
				/* GDI_ModifyWorldTransform */
				template <class OutStream>
				bool GDI_ModifyWorldTransform(OutStream& out){
					//if(CrFunc_.Parameters[6] & 0x0002){
						WTMat_.M[0][0] = clx::float_cast(CrFunc_.Parameters[0]);
						WTMat_.M[0][1] = clx::float_cast(CrFunc_.Parameters[1]);
						WTMat_.M[1][0] = clx::float_cast(CrFunc_.Parameters[2]);
						WTMat_.M[1][1] = clx::float_cast(CrFunc_.Parameters[3]);
					/*} else if(CrFunc_.Parameters[6] & 0x0004){
						WTMat_.M[0][0] = clx::float_cast(CrFunc_.Parameters[0])*WTMat_.M[0][0];
						WTMat_.M[0][1] = clx::float_cast(CrFunc_.Parameters[1])*WTMat_.M[0][1];
						WTMat_.M[1][0] = clx::float_cast(CrFunc_.Parameters[2])*WTMat_.M[1][0];
						WTMat_.M[1][1] = clx::float_cast(CrFunc_.Parameters[3])*WTMat_.M[1][1];
					}*/
					return true;
				}
				
				/* GDI_SetCreateFontIndirectW */
				template <class OutStream>
				bool GDI_SetCreateFontIndirectW(OutStream& out){
					return true;
				}
				
				/* GDI_CreateBrushIndirect */
				template <class OutStream>
				bool GDI_CreateBrushIndirect(OutStream& out){
					BrushSwap_.Red = xdivide<byte_type>(CrFunc_.Parameters[2],0);
					BrushSwap_.Green = xdivide<byte_type>(CrFunc_.Parameters[2],1);
					BrushSwap_.Blue = xdivide<byte_type>(CrFunc_.Parameters[2],2);
					double Red = ((double)BrushSwap_.Red)/255;
					double Green = ((double)BrushSwap_.Green)/255;
					double Blue = ((double)BrushSwap_.Blue)/255;
					out << fmt(LITERAL("%f %f %f rg")) % Red % Green % Blue << std::endl;
					CrStat_.Mode |= 0x02;
					
					return true;
				}
				
				/* GDI_Polygon16 */
				template <class OutStream>
				bool GDI_Polygon16(OutStream& out){
					double ox = OffsetX_;
					double oy = OffsetY_;
					
					LONG_type Bounds_Left   = (LONG_type)CrFunc_.Parameters[0];
					LONG_type Bounds_Top    = (LONG_type)CrFunc_.Parameters[1];
					LONG_type Bounds_Right  = (LONG_type)CrFunc_.Parameters[2];
					LONG_type Bounds_Bottom = (LONG_type)CrFunc_.Parameters[3];
					SHORT_type minX = xdivide<SHORT_type>(CrFunc_.Parameters[5],0);
					SHORT_type minY = xdivide<SHORT_type>(CrFunc_.Parameters[5],2);
					SHORT_type maxX = xdivide<SHORT_type>(CrFunc_.Parameters[5],0);
					SHORT_type maxY = xdivide<SHORT_type>(CrFunc_.Parameters[5],2);
					for(size_t i=6; i<CrFunc_.Parameters.size(); i++){
						minX = std::min(minX,xdivide<SHORT_type>(CrFunc_.Parameters[i],0));
						minY = std::min(minY,xdivide<SHORT_type>(CrFunc_.Parameters[i],2));
						maxX = std::max(maxX,xdivide<SHORT_type>(CrFunc_.Parameters[i],0));
						maxY = std::max(maxY,xdivide<SHORT_type>(CrFunc_.Parameters[i],2));
					}
					double point_scaleX = (maxX-minX != 0) ? (double)(Bounds_Right-Bounds_Left)/(double)(maxX-minX) : 0.0;
					double point_scaleY = (maxY-minY != 0) ? (double)(Bounds_Bottom-Bounds_Top)/(double)(maxY-minY) : 0.0;
#if 0
					double Red = ((double)BrushSwap_.Red)/255;
					double Green = ((double)BrushSwap_.Green)/255;
					double Blue = ((double)BrushSwap_.Blue)/255;
					//out << LITERAL("q") << std::endl;
					//out << fmt(LITERAL("%f 0 0 %f %d %d cm")) % scaleh_ % scalev_ % OffsetX_ % OffsetY_ << std::endl;
					//std::cout << "Polygon: blush: " << BrushSwap_.StoredFlg << ", pen: " << PenSwap_.StoredFlg << std::endl;
					out << fmt(LITERAL("%f %f %f rg")) % Red % Green % Blue << std::endl;
#endif
					double X = (double)((xdivide<SHORT_type>(CrFunc_.Parameters[5],0)-minX)*point_scaleX);
					double Y = (double)((xdivide<SHORT_type>(CrFunc_.Parameters[5],2)-minY)*point_scaleY);
					out << fmt(LITERAL("%f %f m"))
						% ((X+Bounds_Left-margin_Left_)+ox)
						% ((height_-Y-Bounds_Top+margin_Top_)+oy)
					<< std::endl;
					for(size_t i=6; i<CrFunc_.Parameters.size(); i++){
						X = (double)((xdivide<SHORT_type>(CrFunc_.Parameters[i],0)-minX)*point_scaleX);
						Y = (double)((xdivide<SHORT_type>(CrFunc_.Parameters[i],2)-minY)*point_scaleY);
						out << fmt(LITERAL("%f %f l"))
							% ((X+Bounds_Left-margin_Left_)+ox)
							% ((height_-Y-Bounds_Top+margin_Top_)+oy)
						<< std::endl;
					}
					
					// Memo: close かどうかの判定．
					if (!CrStat_.MultiPath) {
						if (PenSwap_.WidthX > 0) out << LITERAL("b") << std::endl;
						else out << LITERAL("f") << std::endl;
						CrStat_.Mode = 0;
					}
					
					//out << LITERAL("Q") << std::endl;
					
					return true;
				}
				
				/* GDI_PolyPolygon16 */
				template <class OutStream>
				bool GDI_PolyPolygon16(OutStream& out){
					double ox = OffsetX_;
					double oy = OffsetY_;
					
					LONG_type Bounds_Left      = (LONG_type)CrFunc_.Parameters[0];
					LONG_type Bounds_Top       = (LONG_type)CrFunc_.Parameters[1];
					LONG_type Bounds_Right     = (LONG_type)CrFunc_.Parameters[2];
					LONG_type Bounds_Bottom    = (LONG_type)CrFunc_.Parameters[3];
					DWORD_type NumberOfPolygons = (LONG_type)CrFunc_.Parameters[4];
					LONG_type Count            = (LONG_type)CrFunc_.Parameters[5];
					SHORT_type minX = xdivide<SHORT_type>(CrFunc_.Parameters[6 + NumberOfPolygons],0);
					SHORT_type minY = xdivide<SHORT_type>(CrFunc_.Parameters[6 + NumberOfPolygons],2);
					SHORT_type maxX = xdivide<SHORT_type>(CrFunc_.Parameters[6 + NumberOfPolygons],0);
					SHORT_type maxY = xdivide<SHORT_type>(CrFunc_.Parameters[6 + NumberOfPolygons],2);
					for(size_t i=6 + NumberOfPolygons; i<6 + NumberOfPolygons + Count; i++){
						minX = std::min(minX,xdivide<SHORT_type>(CrFunc_.Parameters[i],0));
						minY = std::min(minY,xdivide<SHORT_type>(CrFunc_.Parameters[i],2));
						maxX = std::max(maxX,xdivide<SHORT_type>(CrFunc_.Parameters[i],0));
						maxY = std::max(maxY,xdivide<SHORT_type>(CrFunc_.Parameters[i],2));
					}
					double point_scaleX = (maxX-minX != 0) ? (double)(Bounds_Right-Bounds_Left)/(double)(maxX-minX) : 0.0;
					double point_scaleY = (maxY-minY != 0) ? (double)(Bounds_Bottom-Bounds_Top)/(double)(maxY-minY) : 0.0;
#if 0
					double Red = ((double)BrushSwap_.Red)/255;
					double Green = ((double)BrushSwap_.Green)/255;
					double Blue = ((double)BrushSwap_.Blue)/255;
#endif
					LONG_type CurPointsRecord = 6 + NumberOfPolygons;
					//std::cout << "PolyPolygon: blush: " << BrushSwap_.StoredFlg << ", pen: " << PenSwap_.StoredFlg  << std::endl;
					//out << fmt(LITERAL("%f %f %f rg")) % Red % Green % Blue << std::endl;
					for(size_t i=0; i<NumberOfPolygons; i++){
						//out << LITERAL("q") << std::endl;
						//out << fmt(LITERAL("%f 0 0 %f %d %d cm")) % scaleh_ % scalev_ % OffsetX_ % OffsetY_ << std::endl;
						double X = (double)((xdivide<SHORT_type>(CrFunc_.Parameters[CurPointsRecord],0)-minX)*point_scaleX);
						double Y = (double)((xdivide<SHORT_type>(CrFunc_.Parameters[CurPointsRecord],2)-minY)*point_scaleY);
						out << fmt(LITERAL("%f %f m"))
							% ((X+Bounds_Left-margin_Left_)+ox)
							% ((height_-Y-Bounds_Top+margin_Top_)+oy)
						<< std::endl;
						DWORD_type PolygonPointCount = CrFunc_.Parameters[6 + i];
						for(size_t j=CurPointsRecord + 1; j<CurPointsRecord + PolygonPointCount; j++){
							X = (double)((xdivide<SHORT_type>(CrFunc_.Parameters[j],0)-minX)*point_scaleX);
							Y = (double)((xdivide<SHORT_type>(CrFunc_.Parameters[j],2)-minY)*point_scaleY);
							out << fmt(LITERAL("%f %f l"))
								% ((X+Bounds_Left-margin_Left_)+ox)
								% ((height_-Y-Bounds_Top+margin_Top_)+oy)
							<< std::endl;
						}
						CurPointsRecord += PolygonPointCount;
						//out << LITERAL("f") << std::endl;
						//out << LITERAL("Q") << std::endl;
					}
					
					// Memo: close かどうかの判定．
					if (!CrStat_.MultiPath) {
						if (PenSwap_.WidthX > 0) out << LITERAL("b") << std::endl;
						else out << LITERAL("f") << std::endl;
						CrStat_.Mode = 0;
					}
					return true;
				}
				
				/* GDI_DeleteObject */
				template <class OutStream>
				bool GDI_DeleteObject(OutStream& out){
					return true;
				}
				
				/* GDI_BitBLT */
				template <class OutStream>
				bool GDI_BitBLT(OutStream& out){
					return true;
				}
				
				/* GDI_CreatePen */
				template <class OutStream>
				bool GDI_CreatePen(OutStream& out){
					PenSwap_.Red = xdivide<byte_type>(CrFunc_.Parameters[4],0);
					PenSwap_.Green = xdivide<byte_type>(CrFunc_.Parameters[4],1);
					PenSwap_.Blue = xdivide<byte_type>(CrFunc_.Parameters[4],2);
					PenSwap_.PenStyle = (DWORD_type)CrFunc_.Parameters[1];
					PenSwap_.WidthX = (LONG_type)CrFunc_.Parameters[2];
					//PenSwap_.WidthY = (LONG_type)CrFunc_.Parameters[3];
					PenSwap_.WidthY = (LONG_type)CrFunc_.Parameters[2]; // Parameters[3] は reserved．
					if(PenSwap_.PenStyle == 0){
						PenSwap_.WidthX = 1;
					}
					PenSwap_.StoredFlg = true;
					
					if (PenSwap_.WidthX > 0) {
						double Red = ((double)PenSwap_.Red)/255;
						double Green = ((double)PenSwap_.Green)/255;
						double Blue = ((double)PenSwap_.Blue)/255;
						out << fmt(LITERAL("%d w")) % PenSwap_.WidthX << std::endl;
						out << fmt(LITERAL("%f %f %f RG")) % Red % Green % Blue << std::endl;
						CrStat_.Mode |= 0x01;
					}
					
					return true;
				}
				
				/* GDI_ExtCreatePen */
				template <class OutStream>
				bool GDI_ExtCreatePen(OutStream& out){
					PenSwap_.PenStyle = (DWORD_type)CrFunc_.Parameters[5];
					PenSwap_.WidthX = (LONG_type)CrFunc_.Parameters[6];
					PenSwap_.WidthY = (LONG_type)CrFunc_.Parameters[6];
					PenSwap_.Red = xdivide<byte_type>(CrFunc_.Parameters[8],0);
					PenSwap_.Green = xdivide<byte_type>(CrFunc_.Parameters[8],1);
					PenSwap_.Blue = xdivide<byte_type>(CrFunc_.Parameters[8],2);
					PenSwap_.StoredFlg = true;
					
					if (PenSwap_.WidthX > 0) {
						double Red = ((double)PenSwap_.Red)/255;
						double Green = ((double)PenSwap_.Green)/255;
						double Blue = ((double)PenSwap_.Blue)/255;
						out << fmt(LITERAL("%d w")) % PenSwap_.WidthX << std::endl;
						out << fmt(LITERAL("%f %f %f RG")) % Red % Green % Blue << std::endl;
						CrStat_.Mode |= 0x01;
					}
					
					
					return true;
				}
				
				/* GDI_SetMapMode */
				template <class OutStream>
				bool GDI_SetMapMode(OutStream& out){
					return true;
				}
				
				/* GDI_SetWindowOrgEx */
				template <class OutStream>
				bool GDI_SetWindowOrgEx(OutStream& out){
					//LONG_type OrgX = (LONG_type)CrFunc_.Parameters[0];
					//LONG_type OrgY = (LONG_type)CrFunc_.Parameters[1];
					return true;
				}
				
				/* GDI_SetWindowExtEx */
				template <class OutStream>
				bool GDI_SetWindowExtEx(OutStream& out){
					//LONG_type ExtX = (LONG_type)CrFunc_.Parameters[0];
					//LONG_type ExtY = (LONG_type)CrFunc_.Parameters[1];
					return true;
				}
				
				/* GDI_SetViewPortOrgEx */
				template <class OutStream>
				bool GDI_SetViewPortOrgEx(OutStream& out){
					//LONG_type OrgX = (LONG_type)CrFunc_.Parameters[0];
					//LONG_type OrgY = (LONG_type)CrFunc_.Parameters[1];
					return true;
				}
				
				/* GDI_SetViewPortExtEx */
				template <class OutStream>
				bool GDI_SetViewPortExtEx(OutStream& out){
					//LONG_type ExtX = (LONG_type)CrFunc_.Parameters[0];
					//LONG_type ExtY = (LONG_type)CrFunc_.Parameters[1];
					return true;
				}
				
				/* GDI_BeginPath */
				template <class OutStream>
				bool GDI_BeginPath(OutStream& out){
					CrStat_.MultiPath = true;
					return true;
				}
				
				/* GDI_CloseFigure */
				template <class OutStream>
				bool GDI_CloseFigure(OutStream& out){
					out << LITERAL("h") << std::endl;
					return true;
				}
				
				/* GDI_EndPath */
				template <class OutStream>
				bool GDI_EndPath(OutStream& out){
					CrStat_.MultiPath = false;
					return true;
				}
				
				/* GDI_PolyBezierTo16 */
				template <class OutStream>
				bool GDI_PolyBezierTo16(OutStream& out){
					double ox = OffsetX_;
					double oy = OffsetY_;
					
					LONG_type Bounds_Left      = (LONG_type)CrFunc_.Parameters[0];
					LONG_type Bounds_Top       = (LONG_type)CrFunc_.Parameters[1];
					LONG_type Bounds_Right     = (LONG_type)CrFunc_.Parameters[2];
					LONG_type Bounds_Bottom    = (LONG_type)CrFunc_.Parameters[3];
					DWORD_type Count = CrFunc_.Parameters[4];
					
					SHORT_type minX = xdivide<SHORT_type>(CrFunc_.Parameters[5],0);
					SHORT_type minY = xdivide<SHORT_type>(CrFunc_.Parameters[5],2);
					SHORT_type maxX = xdivide<SHORT_type>(CrFunc_.Parameters[5],0);
					SHORT_type maxY = xdivide<SHORT_type>(CrFunc_.Parameters[5],2);
					for(size_t i = 6; i < 5+Count; i++){
						minX = std::min(minX,xdivide<SHORT_type>(CrFunc_.Parameters[i],0));
						minY = std::min(minY,xdivide<SHORT_type>(CrFunc_.Parameters[i],2));
						maxX = std::max(maxX,xdivide<SHORT_type>(CrFunc_.Parameters[i],0));
						maxY = std::max(maxY,xdivide<SHORT_type>(CrFunc_.Parameters[i],2));
					}
					double point_scaleX = (maxX-minX != 0) ? (double)(Bounds_Right-Bounds_Left)/(double)(maxX-minX) : 0.0;
					double point_scaleY = (maxY-minY != 0) ? (double)(Bounds_Bottom-Bounds_Top)/(double)(maxY-minY) : 0.0;
					//std::cout << "Bezier: blush: " << BrushSwap_.StoredFlg << ", pen: " << PenSwap_.StoredFlg << std::endl;
#if 0
					double Red = ((double)BrushSwap_.Red)/255;
					double Green = ((double)BrushSwap_.Green)/255;
					double Blue = ((double)BrushSwap_.Blue)/255;
#endif
					
					double X0 = (double)((CrStat_.PosX - minX)*point_scaleX);
					double Y0 = (double)((CrStat_.PosY - minY)*point_scaleY);
					//out << LITERAL("q") << std::endl;
					//out << fmt(LITERAL("%f 0 0 %f %d %d cm")) % scaleh_ % scalev_ % OffsetX_ % OffsetY_ << std::endl;
					//out << fmt(LITERAL("%f %f %f rg")) % Red % Green % Blue << std::endl;
					out << fmt(LITERAL("%f %f m"))
						% (X0+(Bounds_Left-margin_Left_)+ox)
						% (height_-Y0+(-Bounds_Top+margin_Top_)+oy)
					<< std::endl;
					for(size_t i=5; i < Count + 5; i += 3){
						double X1 = (double)((xdivide<SHORT_type>(CrFunc_.Parameters[i],0) - minX)*point_scaleX);
						double Y1 = (double)((xdivide<SHORT_type>(CrFunc_.Parameters[i],2) - minY)*point_scaleY);
						double X2 = (double)((xdivide<SHORT_type>(CrFunc_.Parameters[i+1],0) - minX)*point_scaleX);
						double Y2 = (double)((xdivide<SHORT_type>(CrFunc_.Parameters[i+1],2) - minY)*point_scaleY);
						double X3 = (double)((xdivide<SHORT_type>(CrFunc_.Parameters[i+2],0) - minX)*point_scaleX);
						double Y3 = (double)((xdivide<SHORT_type>(CrFunc_.Parameters[i+2],2) - minY)*point_scaleY);
						out << fmt(LITERAL("%f %f %f %f %f %f c")) 
							% (X1+(Bounds_Left-margin_Left_)+ox)
							% (height_-Y1+(-Bounds_Top+margin_Top_)+oy)
							% (X2+(Bounds_Left-margin_Left_)+ox)
							% (height_-Y2+(-Bounds_Top+margin_Top_)+oy)
							% (X3+(Bounds_Left-margin_Left_)+ox)
							% (height_-Y3+(-Bounds_Top+margin_Top_)+oy)
						<< std::endl;
						CrStat_.PosX = X3;
						CrStat_.PosY = Y3;
					}
					
					// Memo: close するかどうかが分からない場合がある．
					if (!CrStat_.MultiPath) {
						if (CrStat_.Mode == 0x03) out << LITERAL("b") << std::endl;
						else if (CrStat_.Mode == 0x02) out << LITERAL("f") << std::endl;
						else out << LITERAL("s") << std::endl;
						CrStat_.Mode = 0;
					}
					//out << LITERAL("Q") << std::endl;
					return true;
				}
				
				/* GDI_FillPath */
				template <class OutStream>
				bool GDI_FillPath(OutStream& out){
					out << LITERAL("F") << std::endl;
					CrStat_.Mode = 0;
					
					return true;
				}
				
				template <class OutStream>
				bool GDI_StrokeFillPath(OutStream& out) {
					out << LITERAL("B") << std::endl;
					CrStat_.Mode = 0;
					
					return true;
				}
				
				template <class OutStream>
				bool GDI_StrokePath(OutStream& out) {
					out << LITERAL("S") << std::endl;
					CrStat_.Mode = 0;
					
					return true;
				}
			};
		}
	}
}

#endif // FAML_PDF_CONTENTS_EMF_H
