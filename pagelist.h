/* ------------------------------------------------------------------------- */
/*
 *  pagelist.h
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
 *  Last-modified: Wed 21 Jan 2009 16:41:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PDF_PAGELIST_H
#define FAML_PDF_PAGELIST_H

#include <string>
#include <vector>

#include "clx/literal.h"
#include "clx/format.h"

namespace faml {
	namespace pdf {
		/* ----------------------------------------------------------------- */
		//  basic_pagelist
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_pagelist {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			
			explicit basic_pagelist(size_type index = 0) :
				index_(index), kids_() {}
			
			template <class OutStream, class PDFManager>
			bool operator()(OutStream& out, PDFManager& pm) {
				if (index_ == 0) index_ = pm.newindex();
				else pm.setp(index_);
				
				out << fmt(LITERAL("%d 0 obj")) % index_ << std::endl;
				out << LITERAL("<<") << std::endl;
				out << LITERAL("/Type /Pages") << std::endl;
				out << LITERAL("/Kids [ ");
				for (size_type i = 0; i < kids_.size(); ++i) {
					out << fmt(LITERAL("%d 0 R ")) % kids_.at(i);
				}
				out << LITERAL("]") << std::endl;
				out << LITERAL("/Count ") << kids_.size() << std::endl;
				out << LITERAL(">>") << std::endl;
				out << LITERAL("endobj") << std::endl;
				out << std::endl;
				
				return true;
			}
			
			void clear() {
				index_ = 0;
				kids_.clear();
			}
			
			void add(size_type index) {
				kids_.push_back(index);
			}
			
			size_type index() const { return index_; }
			
		private:
			typedef clx::basic_format<CharT, Traits> fmt;
			
			size_type index_;
			std::vector<size_type> kids_;
		};
	}
}

#endif // FAML_PDF_OBJECT_PAGELIST_H
