/* ------------------------------------------------------------------------- */
/*
 *  page.h
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
 *  Last-modified: Wed 15 Apr 2009 20:18:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PDF_PAGE_H
#define FAML_PDF_PAGE_H

#include <string>
#include <vector>
#include "clx/format.h"
#include "clx/literal.h"
#include "page_property.h"

namespace faml {
	namespace pdf {
		/* ----------------------------------------------------------------- */
		//  basic_page
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_page {
		public:
			typedef size_t size_type;
			
			static const size_type nsize = static_cast<size_type>(-1);
			
			basic_page() :
				index_(0), resources_(0), contents_(0), prop_() {}
			
			explicit basic_page(const page_property& prop) :
				index_(0), resources_(0), contents_(0), prop_(prop) {}
			
			virtual ~basic_page() throw() {}
			
			template <class OutStream, class PDFManager>
			bool operator()(OutStream& out, PDFManager& pm) {
				index_ = pm.newindex();
				
				out << fmt(LITERAL("%d 0 obj")) % index_ << std::endl;
				out << LITERAL("<<") << std::endl;
				out << LITERAL("/Type /Page") << std::endl;
				out << fmt(LITERAL("/Parent %d 0 R")) % pm.pages().index() << std::endl;
				if (resources_ == 0) out << LITERAL("/Resources << >>") << std::endl;
				else out << fmt(LITERAL("/Resources %d 0 R")) % resources_ << std::endl;
				out << fmt(LITERAL("/Contents %d 0 R")) % contents_ << std::endl;
				out << fmt(LITERAL("/MediaBox [ 0 0 %d %d ]")) % prop_.width() % prop_.height() << std::endl;
				out << LITERAL(">>") << std::endl;
				out << LITERAL("endobj") << std::endl;
				out << std::endl;
				
				pm.pages().add(index_);
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  Access methods (set).
			/* ------------------------------------------------------------- */
			void resources(size_type n) { resources_ = n; }
			void contents(size_type n) { contents_ = n; }
			void property(const page_property& cp) { prop_ = cp; }
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			size_type index() const { return index_; }
			size_type resources() const { return resources_; }
			size_type contents() const { return contents_; }
			const page_property& property() const { return prop_; }
			
		private:
			typedef clx::basic_format<CharT, Traits> fmt;
			
			size_type index_;
			size_type resources_;
			size_type contents_;
			page_property prop_;
		};
		
		typedef basic_page<char> page;
	}
}

#endif // FAML_PDF_PAGE_H
