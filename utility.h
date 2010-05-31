/* ------------------------------------------------------------------------- */
/*
 *  utility.h
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
 *  Last-modified: Fri 12 Jun 2009 13:10:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PDF_UTILITY_H
#define FAML_PDF_UTILITY_H

#include <string>
#include <sstream>

namespace faml {
	namespace pdf {
		/* ----------------------------------------------------------------- */
		/*
		 *  makelabel
		 *
		 *  Label is identifier to refer objects in PDF stream fields.
		 *  makelabel function generates a unique label by using the
		 *  internal (static) variable and the passed prefix string.
		 */
		/* ----------------------------------------------------------------- */
		template <class Ch, class Tr>
		inline std::basic_string<Ch, Tr> makelabel(const std::basic_string<Ch, Tr>& prefix) {
			static int index = 0;
			std::basic_stringstream<Ch, Tr> ss;
			ss << prefix << index;
			++index;
			return ss.str();
		}
		
		template <class CharT>
		inline std::basic_string<CharT> makelabel(const CharT* prefix) {
			std::basic_string<CharT> tmp(prefix);
			return makelabel(tmp);
		}
		
		/* ----------------------------------------------------------------- */
		/*
		 *  is_half
		 *
		 *  CID 番号を基にして，その文字を半角文字として扱うべきかどうかを
		 *  判断する．
		 *
		 *  Memo: 等幅フォントの場合，CID 番号で 1--97, 327--389,
		 *  634--640, 643--648 を半角（幅 500）として扱うと良いようである．
		 */
		/* ----------------------------------------------------------------- */
		bool is_half(int cid) {
			//if ((cid >= 1 && cid <= 97) ||
			//	(cid >= 327 && cid <= 389) ||
			//	(cid >= 634 && cid <= 640) ||
			//	(cid >= 643 && cid <= 648)) {
			if (cid >= 1 && cid <= 632) {
				return true;
			}
			return false;
		}
	}
	
	
	/* --------------------------------------------------------------------- */
	//  cyg2win
	/* --------------------------------------------------------------------- */
	inline std::basic_string<char> cyg2win(const std::basic_string<char>& path) {
		static const std::basic_string<char> cygroot("/cygdrive/");
		std::basic_string<char> dest;
		
		if (path.size() >= cygroot.size() && path.compare(0, cygroot.size(), cygroot) == 0) {
			std::basic_string<char>::size_type pos = cygroot.size();
			if (pos < path.size()) {
				dest += std::basic_string<char>(1, path.at(pos)) + ":\\";
				pos += 2;
				if (pos < path.size()) dest += path.substr(pos);
			}
		}
		else dest = path;
		
		std::replace(dest.begin(), dest.end(), '/', '\\');
		clx::logger::debug(DEBUGF("%s -> %s", path.c_str(), dest.c_str()));
		return dest;
	}
}

#endif // FAML_PDF_UTILITY_H
