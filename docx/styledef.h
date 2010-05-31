/* ------------------------------------------------------------------------- */
/*
 *  docx/styledef.h
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
 *  Last-modified: Wed 26 Aug 2009 22:18:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_DOCX_STYLEDEF_H
#define FAML_DOCX_STYLEDEF_H

#include <string>

namespace faml {
	namespace docx {
		namespace styles {
			/* ------------------------------------------------------------- */
			/*
			 *  paragraph
			 *
			 *  Style definition of paragraph.
			 */
			/* ------------------------------------------------------------- */
			class paragraph {
			public:
				typedef size_t size_type;
				typedef char char_type;
				typedef std::basic_string<char> string_type;
				
				enum { none = 0, bold = 0x01, italic = 0x02, underline = 0x04, strike = 0x08 };
				
				paragraph() :
					latin_("Century"), japan_("ＭＳ 明朝"),
					align_(0), rgb_(0), decorate_(0), scale_(100),
					size_(10.5), indent_(0.0), indent1st_(0.0) {}
				
				// set all values at once.
				explicit paragraph(const string_type& f, const string_type& fj,
					size_type h, size_type clr, size_type w,
					double sz, double idt, double idt1) :
					latin_(f), japan_(fj),
					align_(h), rgb_(clr), decorate_(0), scale_(100),
					size_(sz), indent_(idt), indent1st_(idt1) {}
				
				// set all values at once.
				explicit paragraph(size_type h, size_type clr, size_type w,
					double sz, double idt, double idt1) :
					latin_("Century"), japan_("ＭＳ 明朝"),
					align_(h), rgb_(clr), decorate_(0), scale_(100),
					size_(sz), indent_(idt), indent1st_(idt1) {}
				
				/* --------------------------------------------------------- */
				//  Access methods (get).
				/* --------------------------------------------------------- */
				const string_type& latin() const { return latin_; }
				const string_type& japan() const { return japan_; }
				size_type align() const { return align_; }
				size_type rgb() const { return rgb_; }
				size_type decorate() const { return decorate_; }
				size_type scale() const { return scale_; }
				double size() const { return size_; }
				double indent() const { return indent_; }
				double indent1st() const { return indent1st_; }
				
				/* --------------------------------------------------------- */
				//  Access methods (set).
				/* --------------------------------------------------------- */
				void latin(const string_type& cp) { latin_ = cp; }
				void japan(const string_type& cp) { japan_ = cp; }
				void align(size_type cp) { align_ = cp; }
				void rgb(size_type cp) { rgb_ = cp; }
				void decorate(size_type cp) { decorate_ = cp; }
				void scale(size_type cp) { scale_ = cp; }
				void size(double cp) { size_ = cp; }
				void indent(double cp) { indent_ = cp; }
				void indent1st(double cp) { indent1st_ = cp; }
				
			private:
				string_type latin_;		// Latin font name.
				string_type japan_;		// Japanese font name.
				size_type align_;		// text align.
				size_type rgb_;			// font color.
				size_type decorate_;	// font decoration (bold, italic).
				size_type scale_;		// font horizontal scaling [%]
				double size_;			// font size.
				double indent_;			// indent size (twip unit).
				double indent1st_;		// indent size of the first line (twip unit).
			};
		}
	}
}

#endif // FAML_DOCX_STYLEDEF_H
