/* ------------------------------------------------------------------------- */
/*
 *  font_property.h
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
 *  Last-modified: Tue 14 Apr 2009 13:06:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PDF_FONT_PROPERTY_H
#define FAML_PDF_FONT_PROPERTY_H

#include <string>
#include <vector>
#include "coordinate.h"

namespace faml {
	namespace pdf {
		namespace charset {
			/* ------------------------------------------------------------- */
			/*
			 *  charset
			 *
			 *  win  : Windows 95J character set, Shift-JIS
			 *  mac  : Macintosh KanjiTalk ver. 7.x character set, Shift-JIS
			 *  euc  : JIS X 0208-1983 character set, EUC-JP
			 *  utf8 : ISO 10646-1:1993 (Unicode), UTF-8 (Adobe-Japan1-2)
			 *  utf16: ISO 10646-1:2000/ISO 10646-2:2001 (Unicode 3.2),
			 *         UTF-16 (Adobe-Japan1-5)
			 *  utf32: ISO 10646-1:2000/ISO 10646-2:2001 (Unicode 3.2),
			 *         UTF-32 (Adobe-Japan1-5)
			 */
			/* ------------------------------------------------------------- */
			enum { win = 0x01, mac = 0x02, euc = 0x04, utf8 = 0x08, utf16 = 0x10, utf32 = 0x20 };
		}
		
		/* ----------------------------------------------------------------- */
		//  basic_font_property
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_font_property {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef unsigned char byte_type;
			typedef std::basic_string<CharT, Traits> string_type;
			
			basic_font_property() :
				name_(), type_(0), flags_(0), caph_(0), ascent_(0), descent_(0),
				stemv_(0), angle_(0), bbox_(), panose_(), widths_() {}
			
			/* ------------------------------------------------------------- */
			//  Access methods (set).
			/* ------------------------------------------------------------- */
			void name(const string_type& s) { name_ = s; }
			void name(const char_type* s) { name_ = s; }
			void type(int value) { type_ = value; }
			void flags(int x) { flags_ = x; }
			void cap_height(int caph) { caph_ = caph; }
			void ascent(int value) { ascent_ = value; }
			void descent(int value) { descent_ = value; }
			void stemv(int value) { stemv_ = value; }
			void italic_angle(int value) { angle_ = value; }
			void bbox(const square& value) { bbox_ = value; }
			void panose(const string_type& s) { panose_ = s; }
			void panose(const char_type* s) { panose_ = s; }
			
			template <class InIter>
			void widths(InIter first, InIter last) {
				while (first != last) {
					widths_.push_back(*first);
					++first;
				}
			}
			
			void width(int value) { widths_.push_back(value); }
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			const string_type& name() const { return name_; }
			int type() const { return type_; }
			int flags() const { return flags_; }
			int cap_height() const { return caph_; }
			int ascent() const { return ascent_; }
			int descent() const { return descent_; }
			int stemv() const { return stemv_; }
			int italic_angle() const { return angle_; }
			const square& bbox() const { return bbox_; }
			const string_type& panose() const { return panose_; }
			const std::vector<int>& widths() const { return widths_; }
			
		private:
			string_type name_;
			int type_;
			int flags_;
			int caph_;
			int ascent_;
			int descent_;
			int stemv_;
			int angle_;
			square bbox_;
			string_type panose_;
			std::vector<int> widths_;
			
		public:
			/* ------------------------------------------------------------- */
			/*
			 *  These constructors are mainly used when making default
			 *  font variables.
			 */
			/* ------------------------------------------------------------- */
			explicit basic_font_property(const char_type* s, int t,
				int x, int caph, int as, int des, int stv, int angl, const square& bb,
				const char_type* pan) :
				name_(s), type_(t), flags_(x), caph_(caph), ascent_(as), descent_(des),
				stemv_(stv), angle_(angl), bbox_(bb), panose_(pan), widths_() {}
			
			template <class InIter>
			explicit basic_font_property(const char_type* s, int t,
				int x, int caph, int as, int des, int stv, int angl, const square& bb,
				const char_type* pan, InIter first, InIter last) :
				name_(s), type_(t), flags_(x), caph_(caph), ascent_(as), descent_(des),
				stemv_(stv), angle_(angl), bbox_(bb), panose_(pan), widths_() {
				this->widths(first, last);
			}
		};
		
		typedef basic_font_property<char> font_property;
	}
}

#endif // FAML_PDF_FONT_PROPERTY_H
