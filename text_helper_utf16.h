/* ------------------------------------------------------------------------- */
/*
 *  text_helper_utf16.h
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
 *  Last-modified: Mon 11 May 2009 16:38:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PDF_TEXT_HELPER_UTF16_H
#define FAML_PDF_TEXT_HELPER_UTF16_H

#include <cctype>
#include <deque>
#include "code_convert.h"
#include "utf16.h"
#include "utility.h"

namespace faml {
	namespace pdf {
		namespace utf16 {
			/* ------------------------------------------------------------- */
			//  is_alnum
			/* ------------------------------------------------------------- */
			bool is_alnum(word_type c) {
				if (c >= 0x007f) return false;
				return std::isalnum(static_cast<char>(c)) != 0;
			}
			
			/* ------------------------------------------------------------- */
			//  is_space
			/* ------------------------------------------------------------- */
			bool is_space(word_type c) {
				if (c == 0x0020 || c == 0x00a0 || c == 0x3000) return true;
				return false;
			}
			
			/* ------------------------------------------------------------- */
			//  is_break
			/* ------------------------------------------------------------- */
			bool is_break(word_type c) {
				if (c == 0x000c || c == 0x000a) return true;
				return false;
			}
			
			/* ------------------------------------------------------------- */
			//  is_lpunct
			/* ------------------------------------------------------------- */
			bool is_lpunct(word_type c) {
				static const word_type codes[] = {
					0xff5f,		// double parentheses
					0x003c,		// <
					0xff1c,		// ��
					0x0028,		// parenthesis: (
					0xff08,		// parenthesis: �i
					0x300c,		// corner bracket: �u
					0xff62,		// corner bracket: �
					0x300e,		// white corner bracket: �w
					0x007b,		// {
					0xff5b,		// �o
					0x005b,		// [
					0xff3b,		// �m
					0x3014,		// �k
					0x3818,		// double turtle parentheses
					0x3008,		// �q
					0x300a,		// �s
					0x3010,		// with-corner-parenthesis:�y
					0x3016,		// white with-corner-parenthesis
					0x301d,		// ��
					0x2018,		// �e
					0x201c,		// �g
					0x00ab		// ��
				};
				
				for (size_t i = 0; i < sizeof(codes) / sizeof(word_type); ++i) {
					if (c == codes[i]) return true;
				}
				return false;
			}
			
			/* ------------------------------------------------------------- */
			//  is_tail
			/* ------------------------------------------------------------- */
			bool is_tail(word_type c) {
				static const word_type codes[] = {
					0xff61,		// �
					0x3002,		// �B
					0xff64,		// �
					0x3001,		// �A
					0xff63,		// �
					0x300d,		// �v
					0x007b,		// �x
					0x007d,		// }
					0xff5d,		// �p
					0x005d,		// ]
					0xff3d,		// �n
					0x3015,		// �l
					0x3019,		// double closed turtle parentheses
					0x3009,		// �r
					0x300b,		// �t
					0x3011,		// closed with-corner-parenthesis: �z
					0x3017,		// white closed with-corner-parenthesis
					0x0021,		// !
					0xff01,		// �I
					0x003f,		// ?
					0xff0e,		// �D
					0x002e,		// .
					0xff0c,		// �C
					0x002c,		// ,
					0x00bf,		// inverted question mark
					0x00a1,		// inverted exclamation mark
					0x203d,		// interrobang
					0xff1a,		// �F
					0x003a,		// :
					0xff1b,		// �G
					0x003b		// ;
				};
				
				if (is_space(c) || is_break(c)) return true;
				for (size_t i = 0; i < sizeof(codes) / sizeof(word_type); ++i) {
					if (c == codes[i]) return true;
				}
				
				return false;
			}
			
			/* ------------------------------------------------------------- */
			//  is_rpunct
			/* ------------------------------------------------------------- */
			bool is_rpunct(word_type c) {
				static const word_type codes[] = {
					0x003e,		// >
					0xff1e,		// ��
					0xff60,		// doble closed parentheses
					0x0029,		// closed parenthesis: )
					0xff09,		// closed parenthesis: �j
					0x301f,		// ��
					0x2025,		// �d
					0x2026,		// �c
					0x30fb,		// �E
					0xff65,		// �
					0x203c,		// double exclamation marks
					0x2047,		// double question marks
					0x2048,		// question and exclamation marks
					0x2049,		// exclamation and question marks
					0x30fc,		// �[
					0xff70,		// �
					0x2014,		// dash mark
					0x2013,		// dash mark
					0x301c,		// wave dash mark
					0x3005,		// �X
					0x309d,		// �T
					0x309e,		// �U
					0x30fd,		// �R
					0x30fe,		// �S
					0x3003,		// �V
					0xff0d,		// �|
					0x002d,		// -
					0x2010,		// �]
					0xff0f,		// �^
					0x002f,		// /
					0xff3c,		// full-length backslash
					0x005c,		// backslash
					0x2019,		// �f
					0x201d,		// �h
					0x00bb,		// ��
					0x30a1,		// �@
					0x30a3,		// �B
					0x30a5,		// �D
					0x30a7,		// �F
					0x30a9,		// �H
					0x30c3,		// �b
					0x30e3,		// ��
					0x30e5,		// ��
					0x30e7,		// ��
					0x30ee,		// ��
					0x30f5,		// ��
					0x30f6,		// ��
					0x3041,		// ��
					0x3043,		// ��
					0x3045,		// ��
					0x3047,		// ��
					0x3049,		// ��
					0x3063,		// ��
					0x3083,		// ��
					0x3085,		// ��
					0x3087,		// ��
					0x308e		// ��
				};
				
				if (is_space(c) || is_break(c)) return true;
				for (size_t i = 0; i < sizeof(codes) / sizeof(word_type); ++i) {
					if (c == codes[i]) return true;
				}
				
				return false;
			}
			
			/* ------------------------------------------------------------- */
			//  chomp
			/* ------------------------------------------------------------- */
			template <class String>
			String& chomp(String& src) {
				size_t pos = src.size();
				while (pos > 1) {
					unsigned char c = src.at(pos - 2);
					unsigned char c2 = src.at(pos - 1);
					if (!(c == 0x00 && (c2 == 0x0a || c2 == 0x0c || c2 == 0x0d))) break;
					pos -= 2;
				}
				
				src.erase(pos);
				return src;
			}
			
			/* ------------------------------------------------------------- */
			//  count
			/* ------------------------------------------------------------- */
			template <class InIter>
			size_t count(InIter first, InIter last) {
				size_t n = 0;
				while (first != last) {
					++first;
					if (first == last) break;
					++n;
					++first;
				}
				return n;
			}
			
			/* ------------------------------------------------------------- */
			//  width
			/* ------------------------------------------------------------- */
			template <class InIter, class Property>
			size_t width(InIter first, InIter last, const Property& info) {
				size_t n = 0;
				while (first != last) {
					byte_type c = static_cast<byte_type>(*first);
					++first;
					if (first == last) break;
					byte_type c2 = static_cast<byte_type>(*first);
					++first;
					
					int code = (static_cast<int>(c) << 8) | static_cast<int>(c2);
					if (info.type() == 1) {
						if (code <= 0x007f) {
							if (static_cast<size_t>(code) < info.widths().size()) n += info.widths().at(code);
							else n += 500;
						}
						else n += 1000;
					}
					else { // Type0 �t�H���g
						code = to_cid(code);
						if (code > 0 && static_cast<size_t>(code) < info.widths().size()) n += info.widths().at(code);
						else if (is_half(code)) n += 500;
						else n += 1000;
					}
				}
				return n;
			}
			
			/* ------------------------------------------------------------- */
			//  getc
			/* ------------------------------------------------------------- */
			template <class InIter>
			word_type getc(InIter& first, InIter last) {
				if (first == last) return word_type(-1);
				
				byte_type c1 = static_cast<byte_type>(*first);
				++first;
				if (first == last) return word_type(-1);
				
				byte_type c2 = static_cast<byte_type>(*first);
				++first;
				return ((c1 << 8) | c2);
			}
			
			/* ------------------------------------------------------------- */
			//  getw
			/* ------------------------------------------------------------- */
			template <class Property>
			size_t getw(int code, const Property& info) {
				size_t dest = 0;
				if (info.type() == 1) {
					if (code <= 0x007f) {
						if (static_cast<size_t>(code) < info.widths().size()) {
							dest = info.widths().at(code);
						}
						else dest = 500;
					}
					else dest = 1000;
				}
				else { // Type0 �t�H���g
					code = to_cid(code);
					if (code > 0 && static_cast<size_t>(code) < info.widths().size()) {
						dest = info.widths().at(code);
					}
					else if (is_half(code)) dest = 500;
					else dest = 1000;
				}
				
				return dest;
			}
			
			/* ------------------------------------------------------------- */
			//  getline
			/* ------------------------------------------------------------- */
			template <class InIter, class OutIter, class Property>
			size_t getline(InIter& first, InIter last, OutIter out,
				size_t limit, bool wrap, const Property& info) {
				InIter pbeg = first;
				InIter pback = first;
				size_t m = 0, n = 0;
				while (first != last && n <= limit) {
					word_type wc = getc(first, last);
					if (wc == word_type(-1)) throw std::runtime_error("bad character code");
					
					size_t l = getw(static_cast<int>(wc), info);
					if (n + m + l > limit) {
						if (is_tail(wc)) {
							out = std::copy(pbeg, first, out);
							pbeg = first;
							pback = first;
						}
						else if (is_rpunct(wc) || (wrap && is_alnum(wc))) first = pbeg;
						else {
							out = std::copy(pbeg, pback, out);
							first = pback;
							pbeg = pback;
						}
						break;
					}
					
					m += l;
					pback = first;
					
					if (!is_lpunct(wc) && (!wrap || !is_alnum(wc))) {
						out = std::copy(pbeg, pback, out);
						n += m;
						m = 0;
						pbeg = pback;
					}
					if (is_break(wc)) break;
				}
				
				if (first == last && pbeg != pback) {
					out = std::copy(pbeg, pback, out);
					n += m;
				}
				
				return n;
			}
		}
	}
}

#endif // FAML_PDF_TEXT_HELPER_UTF16_H
