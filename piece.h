/* ------------------------------------------------------------------------- */
/*
 *  piece.h
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
 *  Last-modified: Tue 04 Aug 2009 16:10:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PIECE_H
#define FAML_PIECE_H

#include <string>
#include "color.h"

namespace faml {
	/* --------------------------------------------------------------------- */
	/*
	 *  basic_piece
	 *
	 *  FAML における文字列の最少単位を表すクラス．データと描画に
	 *  必要な情報（フォント関連情報）を格納するためのクラス．
	 */
	/* --------------------------------------------------------------------- */
	template <
		class CharT,
		class Traits = std::char_traits<CharT>
	>
	class basic_piece {
	public:
		typedef size_t size_type;
		typedef CharT char_type;
		typedef std::basic_string<CharT, Traits> string_type;
		typedef faml::pdf::color color_type;
		
		enum {
			none		= 0x00,
			bold		= 0x01,
			italic		= 0x02,
			underline	= 0x04,
			strike		= 0x08
		};
		
		basic_piece() :
			data_(), name_(), size_(-1.0), scale_(100.0),
			decorate_(0), fill_() {}
		
		explicit basic_piece(const string_type& data) :
			data_(data), name_(), size_(-1.0), scale_(100.0),
			decorate_(0), fill_(0) {}
		
		virtual ~basic_piece() throw() {}
		
		/* ----------------------------------------------------------------- */
		//  Access methods (get).
		/* ----------------------------------------------------------------- */
		const string_type& data() const { return data_; }
		const string_type& name() const { return name_; }
		double size() const { return size_; }
		double scale() const { return scale_; }
		size_type decorate() const { return decorate_; }
		const color_type& fill() const { return fill_; }
		
		/* ----------------------------------------------------------------- */
		//  Access methods (set).
		/* ----------------------------------------------------------------- */
		void data(const string_type& cp) { data_ = cp; }
		void name(const string_type& cp) { name_ = cp; }
		void size(double cp) { size_ = cp; }
		void scale(double cp) { scale_ = cp; }
		void decorate(size_type cp) { decorate_ = cp; }
		void fill(const color_type& cp) { fill_ = cp; }
		
	private:
		string_type data_;				// 文字列
		string_type name_;				// フォント名
		double size_;					// フォントサイズ
		double scale_;					// フォントサイズの横幅の縮尺率 [%]
		size_type decorate_;			// フォント装飾
		color_type fill_;				// フォント色
	};
	
	namespace pdf {
		/* ----------------------------------------------------------------- */
		/*
		 *  basic_extpiece
		 *
		 *  faml::pdf 用に必要な情報を追加したクラス．具体的には，
		 *  faml::basic_piece の情報に加えて PDF のフォントオブジェクト
		 *  の情報を保持する．
		 */
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_extpiece : public basic_piece<CharT, Traits> {
		public:
			typedef basic_piece<CharT, Traits> super;
			typedef typename super::size_type size_type;
			typedef typename super::char_type char_type;
			typedef typename super::string_type string_type;
			typedef typename super::color_type color_type;
			typedef basic_font<CharT, Traits> font_type;
			
			basic_extpiece() :
				super(), font_(NULL) {}
			
			basic_extpiece(const basic_extpiece& cp) :
				super(cp), font_(cp.font_) {}
			
			explicit basic_extpiece(const basic_piece<CharT, Traits>& cp) :
				super(cp), font_(NULL) {}
			
			explicit basic_extpiece(const font_type& f) :
				super(), font_(&f) {}
			
			explicit basic_extpiece(const string_type& data, const font_type& f) :
				super(data), font_(&f) {}
			
			virtual ~basic_extpiece() throw() {}
			
			const font_type& font() const {
				if (font_ == NULL) throw std::runtime_error("cannot find font object");
				return *font_;
			}
			
			void font(const font_type& cp) {
				font_ = &cp;
				this->name(font_->label());
			}
			
		private:
			const font_type* font_;
		};
	}
}

#endif // FAML_PIECE_H
