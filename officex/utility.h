/* ------------------------------------------------------------------------- */
/*
 *  officex/utility.h
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
 *  Last-modified: Fri 05 Jun 2009 15:13:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_OFFICEX_UTILITY_H
#define FAML_OFFICEX_UTILITY_H

#include <string>
#include "clx/literal.h"

namespace faml {
	namespace officex {
		/* ----------------------------------------------------------------- */
		//  getborder
		/* ----------------------------------------------------------------- */
		template <class Ch, class Tr, class Type>
		inline bool getborder(const std::basic_string<Ch, Tr>& s, Type& dest) {
			double weight = 0.75;
			
			if (s == LITERAL("thin") || s == LITERAL("single")) {
				dest.type(0x001);
				dest.weight(weight);
			}
			else if (s == LITERAL("medium")) {
				dest.type(0x001);
				dest.weight(weight * 2.0);
			}
			else if (s == LITERAL("thick")) {
				dest.type(0x001);
				dest.weight(weight * 2.0);
			}
			else if (s == LITERAL("double")) {
				dest.type(0x011);
				dest.weight(weight);
			}
			else if (s == LITERAL("thickThin")) {
				dest.type(0x031);
				dest.weight(weight);
			}
			else if (s == LITERAL("thinThick")) {
				dest.type(0x051);
				dest.weight(weight);
			}
			else if (s == LITERAL("dot") || s == LITERAL("dotted") || s == LITERAL("sysDot")) {
				dest.type(0x003);
				dest.weight(weight);
			}
			else if (s == LITERAL("dashDot")) {
				dest.type(0x005);
				dest.weight(weight);
			}
			else if (s == LITERAL("dashDotDot")) {
				dest.type(0x006);
				dest.weight(weight);
			}
			else if (s == LITERAL("dash") || s == LITERAL("dashed")) {
				dest.type(0x002);
				dest.weight(weight);
			}
			else if (s == LITERAL("slantDashDot")) {
				dest.type(0x002);
				dest.weight(weight);
			}
			else if (s == LITERAL("hair")) {
				dest.type(0x004);
				dest.weight(weight);
			}
			else if (s == LITERAL("mediumDashDot")) {
				dest.type(0x005);
				dest.weight(weight * 2.0);
			}
			else if (s == LITERAL("mediumDashDotDot")) {
				dest.type(0x006);
				dest.weight(weight * 2.0);
			}
			else if (s == LITERAL("mediumDashed")) {
				dest.type(0x002);
				dest.weight(weight * 2.0);
			}
			
			return true;
		}
		
		/* ----------------------------------------------------------------- */
		//  getalign
		/* ----------------------------------------------------------------- */
		template <class Ch, class Tr>
		inline size_t getalign(const std::basic_string<Ch, Tr>& s) {
			if (s == LITERAL("left") || s == LITERAL("l")) return 0;
			if (s == LITERAL("center") || s == LITERAL("ctr")) return 1;
			if (s == LITERAL("centerContinuous")) return 1;
			if (s == LITERAL("right") || s == LITERAL("r")) return 2;
			if (s == LITERAL("justify") || s == LITERAL("both")) return 3;
			if (s == LITERAL("distributed")) return 4;
			if (s == LITERAL("general")) return 5;
			if (s == LITERAL("fill")) return 6;
			return 255;
		}
		
		template <class CharT>
		inline size_t getalign(const CharT* s) {
			std::basic_string<CharT> tmp(s);
			return getalign(tmp);
		}
		
		/* ----------------------------------------------------------------- */
		//  getvalign
		/* ----------------------------------------------------------------- */
		template <class Ch, class Tr>
		size_t getvalign(const std::basic_string<Ch, Tr>& s) {
			if (s == LITERAL("top") || s == LITERAL("t")) return 0;
			if (s == LITERAL("center") || s == LITERAL("ctr")) return 1;
			if (s == LITERAL("bottom") || s == LITERAL("b")) return 2;
			if (s == LITERAL("justify")) return 3;
			if (s == LITERAL("distributed")) return 4;
			return 255;
		}
		
		template <class CharT>
		inline size_t getvalign(const CharT* s) {
			std::basic_string<CharT> tmp(s);
			return getvalign(tmp);
		}
	}
}

#endif // FAML_OFFICEX_UTILITY_H
