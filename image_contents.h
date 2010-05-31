/* ------------------------------------------------------------------------- */
/*
 *  image_contents.h
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
 *  Last-modified: Tue 21 Apr 2009 15:36:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PDF_IMAGE_CONTENTS_H
#define FAML_PDF_IMAGE_CONTENTS_H

#include <string>
#include "clx/format.h"
#include "clx/literal.h"

namespace faml {
	namespace pdf {
		/* ----------------------------------------------------------------- */
		//  basic_image_contents
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits
		>
		class basic_image_contents {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			
			explicit basic_image_contents(const string_type& label, const coordinate& origin,
				double width, double height, bool entag = true) :
				label_(label), origin_(origin), width_(width), height_(height), tag_(entag) {}
			
			template <class OutStream, class PDFManager>
			bool operator()(OutStream& out, PDFManager& pm) {
				return (*this)(out);
			}
			
			template <class OutStream>
			bool operator()(OutStream& out) {
				if (tag_) out << LITERAL("q") << std::endl;
				out << fmt(LITERAL("%f 0 0 %f %f %f cm")) % width_ % height_ % origin_.x() % origin_.y() << std::endl;
				out << fmt(LITERAL("/%s Do")) % label_ << std::endl;
				if (tag_) out << LITERAL("Q") << std::endl;
				return true;
			}
			
		private:
			typedef clx::basic_format<CharT, Traits> fmt;
			
			string_type label_;
			coordinate origin_;
			double width_;
			double height_;
			bool tag_;
		};
	}
}

#endif // FAML_PDF_IMAGE_CONTENTS_H
