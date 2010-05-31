/* ------------------------------------------------------------------------- */
/*
 *  text_helper_mbcs.h
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
#ifndef FAML_PDF_TEXT_HELPER_MBCS_H
#define FAML_PDF_TEXT_HELPER_MBCS_H

#include <cctype>
#include <vector>
#include "code_convert.h"

namespace faml {
	namespace pdf {
		namespace mbcs {
			/* ------------------------------------------------------------- */
			//  is_alnum
			/* ------------------------------------------------------------- */
			bool is_alnum(word_type c) {
				if (c >= 0x007f) return false;
				return std::isalnum(static_cast<char>(c)) != 0;
			}
			
			/* ------------------------------------------------------------- */
			//  is_break
			/* ------------------------------------------------------------- */
			bool is_break(word_type c) {
				if (c == 0x000a || c == 0x000d || c == 0x000c) return true;
				return false;
			}
			
			/* ------------------------------------------------------------- */
			//  is_space
			/* ------------------------------------------------------------- */
			bool is_space(word_type c) {
				if (c == 0x0020) return true;
				return false;
			}
			
			/* ------------------------------------------------------------- */
			//  is_lpunct
			/* ------------------------------------------------------------- */
			bool is_lpunct(word_type c) {
				static const word_type codes[] = {
					0x003c,		// <
					0x8183,		// ��
					0x0028,		// parenthesis: (
					0x8169,		// parenthesis: �i
					0x8175,		// corner bracket: �u
					0x00a2,		// corner bracket: �
					0x8177,		// white corner bracket: �w
					0x007b,		// {
					0x816f,		// �o
					0x005b,		// [
					0x816d,		// �m
					0x816b,		// �k
					0x8171,		// �q
					0x8173,		// �s
					0x8179,		// with-corner-parenthesis:�y
					0x8780,		// ��
					0x8165,		// �e
					0x8167,		// �g
					0x81e1		// ��
				};
				
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
					0x00a1,		// �
					0x8142,		// �B
					0x00a4,		// �
					0x8141,		// �A
					0x003e,		// >
					0x8184,		// ��
					0x0029,		// closed parenthesis: )
					0x816a,		// closed parenthesis: �j
					0x00a3,		// �
					0x8176,		// �v
					0x8178,		// �x
					0x007d,		// }
					0x8170,		// �p
					0x005d,		// ]
					0x816e,		// �n
					0x816c,		// �l
					0x8172,		// �r
					0x8174,		// �t
					0x817a,		// closed with-corner-parenthesis: �z
					0x8781,		// ��
					0x8164,		// �d
					0x8163,		// �c
					0x8145,		// �E
					0x00a5,		// �
					0x0021,		// !
					0x8149,		// �I
					0x003f,		// ?
					0x815b,		// �[
					0x00b0,		// �
					0x8158,		// �X
					0x8154,		// �T
					0x8155,		// �U
					0x8152,		// �R
					0x8153,		// �S
					0x8156,		// �V
					0x8144,		// �D
					0x002e,		// .
					0x8143,		// �C
					0x002c,		// ,
					0x8146,		// �F
					0x003a,		// :
					0x8147,		// �G
					0x003b,		// ;
					0x817c,		// �|
					0x002d,		// -
					0x815d,		// �]
					0x002f,		// /
					0x815e,		// �^
					0x005c,		// backslash
					0x818f,		// full-length backslash
					0x8166,		// �f
					0x8168,		// �h
					0x81e2,		// ��
					0x8340,		// �@
					0x8342,		// �B
					0x8344,		// �D
					0x8346,		// �F
					0x8348,		// �H
					0x8362,		// �b
					0x8383,		// ��
					0x8385,		// ��
					0x8387,		// ��
					0x838e,		// ��
					0x8395,		// ��
					0x8396,		// ��
					0x829f,		// ��
					0x82a1,		// ��
					0x82a3,		// ��
					0x82a5,		// ��
					0x82a7,		// ��
					0x82c1,		// ��
					0x82e1,		// ��
					0x82e3,		// ��
					0x82e5,		// ��
					0x82ec		// ��
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
				while (pos > 0) {
					unsigned char c = src.at(pos - 1);
					if (c == 0x0a || c == 0x0c || c == 0x0d) {
						if (pos > 1) {
							c = src.at(pos - 2);
							if ((c >= 0x81 && c <= 0x9f) || (c >= 0xe0 && c <= 0xfc)) break;
						}
						--pos;
					}
					else break;
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
					byte_type c = static_cast<byte_type>(*first);
					if ((c >= 0x81 && c <= 0x9f) || (c >= 0xe0 && c <= 0xfc)) {
						++first;
						if (first == last) break;
					}
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
					if ((c >= 0x81 && c <= 0x9f) || (c >= 0xe0 && c <= 0xfc)) {
						if (first == last) break;
						n += 1000;
						++first;
					}
					else {
						int code = static_cast<byte_type>(c);
						if (static_cast<size_t>(code) < info.widths().size()) n += info.widths().at(code);
						else n += 500;
					}
				}
				return n;
			}
			
			/* ------------------------------------------------------------- */
			//  getline
			/* ------------------------------------------------------------- */
			template <class InIter, class OutIter, class Property>
			size_t getline(InIter& first, InIter last, OutIter out,
				size_t limit, bool wrap, const Property& info) {
				std::vector<byte_type> v;
				InIter cur = first;
				size_t m = 0, n = 0;
				while (first != last && n <= limit) {
					byte_type c2 = 0x00;
					byte_type c = static_cast<byte_type>(*first);
					word_type wc = c;
					++first;
					if ((c >= 0x81 && c <= 0x9f) || (c >= 0xe0 && c <= 0xfc)) {
						c2 = static_cast<byte_type>(*first);
						wc <<= 8;
						wc |= c2;
						++first;
					}
					
					size_t l = 0;
					if (c2 == 0x00) {
						int code = static_cast<int>(c);
						if (static_cast<size_t>(code) < info.widths().size()) l = info.widths().at(code);
						else l = 500;
					}
					else l = 1000;
					
					if (n + m + l > limit) {
						if (is_rpunct(wc)) {
							v.push_back(c);
							if (c2 != 0x00) v.push_back(c2);
							m += l;
							
							cur = first;
							while (first != last) {
								c2 = 0x00;
								c = static_cast<byte_type>(*first);
								wc = c;
								++first;
								if ((c >= 0x81 && c <= 0x9f) || (c >= 0xe0 && c <= 0xfc)) {
									c2 = static_cast<byte_type>(*first);
									wc <<= 8;
									wc |= c2;
									++first;
								}
								
								if (c2 == 0x00) {
									int code = static_cast<int>(c);
									if (static_cast<size_t>(code) < info.widths().size()) l = info.widths().at(code);
									else l = 500;
								}
								else l = 1000;
								if (!is_rpunct(wc)) {
									first = cur;
									break;
								}
								
								v.push_back(c);
								if (c2 != 0x00) v.push_back(c2);
								m += l;
								cur = first;
							}
							
							out = std::copy(v.begin(), v.end(), out);
							n += m;
						}
						else {
							--first;
							if (c2 != 0x00) --first;
							while (!v.empty()) {
								word_type right = v.at(v.size() - 1);
								byte_type r2 = (v.size() > 1) ? v.at(v.size() - 2) : 0x00;
								if ((r2 >= 0x81 && r2 <= 0x9f) || (r2 >= 0xe0 && r2 <= 0xfc)) {
									right |= (r2 << 8);
								}
								if (!is_lpunct(right)) break;
								v.pop_back();
								--first;
								if ((r2 >= 0x81 && r2 <= 0x9f) || (r2 >= 0xe0 && r2 <= 0xfc)) {
									v.pop_back();
									--first;
								}
							}
							
							if (!v.empty()) {
								word_type right = v.at(v.size() - 1);
								byte_type r2 = (v.size() > 1) ? v.at(v.size() - 2) : 0x00;
								if ((r2 >= 0x81 && r2 <= 0x9f) || (r2 >= 0xe0 && r2 <= 0xfc)) {
									right |= (r2 << 8);
								}
								if (is_alnum(wc) && is_alnum(right)) first = cur;
								else {
									out = std::copy(v.begin(), v.end(), out);
									n += faml::pdf::mbcs::width(v.begin(), v.end(), info);
								}
							}
						}
						break;
					}
					
					m += l;
					v.push_back(c);
					if (c2 != 0x00) v.push_back(c2);
					
					if (!is_alnum(wc) && !is_lpunct(wc)) {
						out = std::copy(v.begin(), v.end(), out);
						n += m;
						m = 0;
						v.clear();
						cur = first;
					}
					
					if (is_break(wc)) {
						if (first != last && is_break(*first)) ++first;
						break;
					}
				}
				
				if (first == last && !v.empty()) {
					out = std::copy(v.begin(), v.end(), out);
					n += m;
				}
				
				return n;
			}
		}
	}
}

#endif // FAML_PDF_TEXT_HELPER_MBCS_H
