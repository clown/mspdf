/* ------------------------------------------------------------------------- */
/*
 *  code_convert.h
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
 *  Last-modified: Mon 23 Mar 2009 11:29:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PDF_CODE_CONVERT_H
#define FAML_PDF_CODE_CONVERT_H

#include <limits>

typedef unsigned char  byte_type;
typedef unsigned short word_type;

namespace faml {
	namespace pdf {
		/* ----------------------------------------------------------------- */
		//  to_narrow
		/* ----------------------------------------------------------------- */
		template <class InIter>
		inline std::string to_narrow(InIter first, InIter last) {
			static const int shift = ((sizeof(unsigned long) - sizeof(*first)) * 8);
			static const unsigned long mask = std::numeric_limits<unsigned int>::max() >> shift;
			
			std::string dest;
			while (first != last) {
				word_type c = static_cast<word_type>(*first) & mask;
				++first;
				if (c == static_cast<word_type>(-1)) continue;
				if (c == 0x005c) c = 0xffe5;
				dest += static_cast<char>(c >> 8);
				dest += static_cast<char>(c);
			}
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  code_convert
		/* ----------------------------------------------------------------- */
		template <class InIter, class Functor>
		inline std::string code_convert(InIter first, InIter last, Functor f) {
			static const int shift = ((sizeof(unsigned long) - sizeof(*first)) * 8);
			static const unsigned long mask = std::numeric_limits<unsigned int>::max() >> shift;
			
			std::string dest;
			while (first != last) {
				word_type c = f(static_cast<word_type>(*first) & mask);
				++first;
				if (c == static_cast<word_type>(-1)) continue;
				dest += static_cast<char>(c >> 8);
				dest += static_cast<char>(c);
			}
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		/*
		 * Windows-1252 (CP1252) mapping (0x80 -- 0x9f).
		 * Some characters which do not have in Windows-932 (CP932)
		 * are used in Microsoft Word (ja). So, we use the following
		 * mapping when converting UTF-16.
		 */
		/* ----------------------------------------------------------------- */
		static const unsigned char cp1252_1[32] = {
			0x20, 0x00, 0x20, 0x01, 0x20, 0x20, 0x20, 0x20,
			0x02, 0x20, 0x01, 0x20, 0x01, 0x00, 0x01, 0x00,
			0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x02, 0x21, 0x01, 0x20, 0x01, 0x00, 0x01, 0x01
		};
		
		static const unsigned char cp1252_2[32] = {
			0xac, 0x00, 0x1a, 0x92, 0x1e, 0x26, 0x20, 0x21,
			0xc6, 0x30, 0x60, 0x39, 0x52, 0x00, 0x7d, 0x00,
			0x00, 0x18, 0x19, 0x1c, 0x1d, 0x22, 0x13, 0x14,
			0xdc, 0x22, 0x61, 0x3a, 0x53, 0x00, 0x7e, 0x78
		};
		
		/* ----------------------------------------------------------------- */
		//  win32map
		/* ----------------------------------------------------------------- */
		inline word_type win32map(word_type src) {
			byte_type high = static_cast<byte_type>((src & 0xff00) >> 8);
			byte_type low  = static_cast<byte_type>(src & 0x00ff);
			if (high != 0x00 || (high == 0x00 && low <= 0x7f)) return src;
			
			word_type dest = static_cast<word_type>(-1);
			if (low >= 0x80 && low <= 0x9f) {
				int index = static_cast<int>(low & 0xff) - 0x80;
				dest  = static_cast<word_type>(cp1252_1[index] & 0x00ff);
				dest |= static_cast<word_type>(cp1252_2[index] << 8);
			}
			else if (low >= 0xa1 && low <= 0xdf) {
				// Windows-932 (CP932) charset
				dest  = static_cast<word_type>((low - 0x40) & 0x00ff);
				dest |= 0xff00;
			}
			
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  win32conv
		/* ----------------------------------------------------------------- */
		template <class InIter>
		inline std::string win32conv(InIter first, InIter last) {
			return code_convert(first, last, win32map);
		}
	}
}

#endif // FAML_PDF_CODE_CONVERT_H
