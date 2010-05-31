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
					0x8183,		// ÅÉ
					0x0028,		// parenthesis: (
					0x8169,		// parenthesis: Åi
					0x8175,		// corner bracket: Åu
					0x00a2,		// corner bracket: ¢
					0x8177,		// white corner bracket: Åw
					0x007b,		// {
					0x816f,		// Åo
					0x005b,		// [
					0x816d,		// Åm
					0x816b,		// Åk
					0x8171,		// Åq
					0x8173,		// Ås
					0x8179,		// with-corner-parenthesis:Åy
					0x8780,		// áÄ
					0x8165,		// Åe
					0x8167,		// Åg
					0x81e1		// Å·
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
					0x00a1,		// °
					0x8142,		// ÅB
					0x00a4,		// §
					0x8141,		// ÅA
					0x003e,		// >
					0x8184,		// ÅÑ
					0x0029,		// closed parenthesis: )
					0x816a,		// closed parenthesis: Åj
					0x00a3,		// £
					0x8176,		// Åv
					0x8178,		// Åx
					0x007d,		// }
					0x8170,		// Åp
					0x005d,		// ]
					0x816e,		// Ån
					0x816c,		// Ål
					0x8172,		// År
					0x8174,		// Åt
					0x817a,		// closed with-corner-parenthesis: Åz
					0x8781,		// áÅ
					0x8164,		// Åd
					0x8163,		// Åc
					0x8145,		// ÅE
					0x00a5,		// •
					0x0021,		// !
					0x8149,		// ÅI
					0x003f,		// ?
					0x815b,		// Å[
					0x00b0,		// ∞
					0x8158,		// ÅX
					0x8154,		// ÅT
					0x8155,		// ÅU
					0x8152,		// ÅR
					0x8153,		// ÅS
					0x8156,		// ÅV
					0x8144,		// ÅD
					0x002e,		// .
					0x8143,		// ÅC
					0x002c,		// ,
					0x8146,		// ÅF
					0x003a,		// :
					0x8147,		// ÅG
					0x003b,		// ;
					0x817c,		// Å|
					0x002d,		// -
					0x815d,		// Å]
					0x002f,		// /
					0x815e,		// Å^
					0x005c,		// backslash
					0x818f,		// full-length backslash
					0x8166,		// Åf
					0x8168,		// Åh
					0x81e2,		// Å‚
					0x8340,		// É@
					0x8342,		// ÉB
					0x8344,		// ÉD
					0x8346,		// ÉF
					0x8348,		// ÉH
					0x8362,		// Éb
					0x8383,		// ÉÉ
					0x8385,		// ÉÖ
					0x8387,		// Éá
					0x838e,		// Éé
					0x8395,		// Éï
					0x8396,		// Éñ
					0x829f,		// Çü
					0x82a1,		// Ç°
					0x82a3,		// Ç£
					0x82a5,		// Ç•
					0x82a7,		// Çß
					0x82c1,		// Ç¡
					0x82e1,		// Ç·
					0x82e3,		// Ç„
					0x82e5,		// ÇÂ
					0x82ec		// ÇÏ
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
