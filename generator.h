/* ------------------------------------------------------------------------- */
/*
 *  generator.h
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
#ifndef FAML_PDF_GENERATOR_H
#define FAML_PDF_GENERATOR_H

#include <iostream>
#include <ostream>
#include <fstream>
#include <iomanip>
#include <string>
#include "clx/literal.h"
#include "clx/format.h"
#include "catalog.h"
#include "pagelist.h"
#include "font_factory.h"

namespace faml {
	namespace pdf {
		/* ----------------------------------------------------------------- */
		//  basic_generator
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_generator {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef std::basic_ostream<CharT, Traits> ostream_type;
			
			typedef basic_catalog<CharT, Traits> catalog_type;
			typedef basic_pagelist<CharT, Traits> pagelist_type;
			
			basic_generator(ostream_type& out = std::cout, double ver = 1.7) :
				out_(out), index_(2),
				version_(ver), catalog_(1), pages_(2),
				pos_(3, 0), finish_(false) {
				out_ << fmt(LITERAL("%%PDF-%3.1f")) % version_ << std::endl;
			}
			
			virtual ~basic_generator() throw() {
				this->finish();
			}
			
			template <class PDFObject>
			bool add(PDFObject obj) {
				return obj(out_, *this);
			}
			
			/* ------------------------------------------------------------- */
			/*
			 *  newindex
			 *
			 *  新しい index（オブジェクト ID と呼ばれる）を取得する際に
			 *  使用するメソッドです．後述する setp() メソッドも参照して
			 *  下さい．
			 */
			/* ------------------------------------------------------------- */
			size_type newindex() {
				pos_.push_back(static_cast<size_type>(out_.tellp()));
				++index_;
				return index_;
			}
			
			/* ------------------------------------------------------------- */
			/*
			 *  setp
			 *
			 *  index のオブジェクトが出力ファイルのどの位置に出力された
			 *  のかを設定するためのメソッドです．
			 *  
			 *  通常，このメソッドを使用する必要はありません．index だけ
			 *  先に確保したいなどの理由で，newindex() を呼んだ直後に
			 *  オブジェクトの定義（"index 0 obj" で始まる記述）を行わない
			 *  場合に，上記のオブジェクトの定義を行う直前に使用して下さい．
			 */
			/* ------------------------------------------------------------- */
			void setp(size_type index) {
				pos_.at(index) = static_cast<size_type>(out_.tellp());
			}
			
			void finish() {
				if (!finish_) {
					catalog_(out_, *this);
					pages_(out_, *this);
					this->xref();
					out_ << LITERAL("%%EOF") << std::endl;
				}
				
				finish_ = true;
			}
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			size_type index() const { return index_; }
			double version() const { return version_; }
			const catalog_type& catalog() const { return catalog_; }
			const pagelist_type& pages() const { return pages_; }
			pagelist_type& pages() { return pages_; }
			font_factory& font() { return fonts_; }
			
		private:
			typedef clx::basic_format<CharT, Traits> fmt;
			
			ostream_type& out_;		// Target stream
			size_type index_;		// Object counter
			double version_;		// PDF version
			catalog_type catalog_;	// Catalog object
			pagelist_type pages_;	// Pages object
			std::vector<size_type> pos_;
			bool finish_;			// Is finish() already called ?
			font_factory fonts_;
			
			/* ------------------------------------------------------------- */
			/*
			 *  xref
			 *
			 *  The method prints the cross reference table.
			 */
			/* ------------------------------------------------------------- */
			void xref() {
				size_type pos = static_cast<size_type>(out_.tellp());
				
				out_ << LITERAL("xref") << std::endl;
				out_ << fmt(LITERAL("0 %d")) % (index_ + 1) << std::endl;
				out_ << LITERAL("0000000000 65535 f ") << std::endl;
				
				for (size_type i = 1; i < pos_.size(); ++i) {
					out_ << fmt(LITERAL("%010d 00000 n ")) % pos_.at(i) << std::endl;
				}
				
				out_ << LITERAL("trailer") << std::endl;
				out_ << LITERAL("<<") << std::endl;
				out_ << fmt(LITERAL("/Size %d")) % (index_ + 1) << std::endl;
				out_ << fmt(LITERAL("/Root %d 0 R")) % catalog_.index() << std::endl;
				out_ << LITERAL(">>") << std::endl;
				out_ << LITERAL("startxref") << std::endl;
				out_ << pos << std::endl;
			}
		};
		
		typedef basic_generator<char> generator;
	}
}

#endif // FAML_PDF_GENERATOR_H
