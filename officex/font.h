/* ------------------------------------------------------------------------- */
/*
 *  officex/font.h
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
 *  Last-modified: Sat 13 Jun 2009 00:45:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_OFFICEX_FONT_H
#define FAML_OFFICEX_FONT_H

#include <string>

namespace faml {
	namespace officex {
		/* ----------------------------------------------------------------- */
		//  basic_font
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_font {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			
			enum {
				none		= 0x00,
				bold		= 0x01,
				italic		= 0x02,
				underline	= 0x04,
				strike		= 0x08
			};
			
			basic_font() :
				latin_(), japan_(),
				size_(0.0), decorate_(0), rgb_(0x000000) {}
			
			explicit basic_font(const string_type& s, double n, size_type c) :
				latin_(s), japan_(s), size_(n), decorate_(0), rgb_(c) {}
			
			explicit basic_font(const string_type& f, const string_type& fj, double n, size_type c) :
				latin_(f), japan_(fj), size_(n), decorate_(0), rgb_(c) {}
			
			virtual ~basic_font() throw() {}
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			//const string_type& name() const { return name_; }
			const string_type& latin() const { return latin_; }
			const string_type& japan() const { return japan_; }
			double size() const { return size_; }
			size_type decorate() const { return decorate_; }
			size_type rgb() const { return rgb_; }
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			void latin(const string_type& cp) { latin_ = cp; }
			void japan(const string_type& cp) { japan_ = cp; }
			void name(const string_type& cp) {
				this->latin(cp);
				this->japan(cp);
			}
			
			void size(double n) { size_ = n; }
			void decorate(size_type cp) { decorate_ = cp; }
			void rgb(size_type c) { rgb_ = c; }
			
		private:
			string_type latin_;
			string_type japan_;
			double size_;
			size_type decorate_;
			size_type rgb_;
		};
	}
}

#endif // FAML_OFFICEX_FONT_H
