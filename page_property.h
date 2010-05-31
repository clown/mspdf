/* ------------------------------------------------------------------------- */
/*
 *  page_property.h
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
 *  Last-modified: Tue 14 Apr 2009 13:06:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PDF_PAGE_PROPERTY_H
#define FAML_PDF_PAGE_PROPERTY_H

namespace faml {
	namespace pdf {
		/* ----------------------------------------------------------------- */
		//  page_size
		/* ----------------------------------------------------------------- */
		template <unsigned int W, unsigned int H>
		struct page_size {
			typedef unsigned int uint_t;
			
			static const uint_t width = W;
			static const uint_t height = H;
		};
		
		typedef page_size< 842, 1191> a3;
		typedef page_size< 595,  842> a4;
		typedef page_size< 420,  595> a5;
		
		typedef page_size< 729, 1032> b4;
		typedef page_size< 516,  729> b5;
		
		typedef page_size< 612,  792> letter;
		typedef page_size< 612, 1008> legal;
		typedef page_size<1223,  792> ledger;
		typedef page_size< 792, 1224> tabloid;
		
		typedef page_size< 720,  540> slide; // Default size of MS PowerPoint
		
		/* ----------------------------------------------------------------- */
		//  margin
		/* ----------------------------------------------------------------- */
		class page_margin {
		public:
			explicit page_margin(int all = 0) :
				top_(all), bottom_(all), left_(all), right_(all) {}
			
			explicit page_margin(int horizon, int vertical) :
				top_(horizon), bottom_(horizon), left_(vertical), right_(vertical) {}
			
			explicit page_margin(int t, int b, int l, int r) :
				top_(t), bottom_(b), left_(l), right_(r) {}
			
			/* ------------------------------------------------------------- */
			//  Access methods (set).
			/* ------------------------------------------------------------- */
			void top(int x) { top_ = x; }
			void bottom(int x) { bottom_ = x; }
			void left(int x) { left_ = x; }
			void right(int x) { right_ = x; }
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			int top() const { return top_; }
			int bottom() const { return bottom_; }
			int left() const { return left_; }
			int right() const { return right_; }
			
		private:
			int top_;
			int bottom_;
			int left_;
			int right_;
		};
		
		/* ----------------------------------------------------------------- */
		//  page_property
		/* ----------------------------------------------------------------- */
		class page_property {
		public:
			page_property() :
				width_(0), height_(0), m_() {}
			
			explicit page_property(int w, int h) :
				width_(w), height_(h), m_() {}
			
			explicit page_property(int w, int h, const page_margin& m) :
				width_(w), height_(h), m_(m) {}
			
			/* ------------------------------------------------------------- */
			//  Access methods (set).
			/* ------------------------------------------------------------- */
			void width(int w) { width_ = w; }
			void height(int h) { height_ = h; }
			void margin(const page_margin& m) { m_ = m; }
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			int width() const { return width_; }
			int height() const { return height_; }
			const page_margin& margin() const { return m_; }
			page_margin& margin() { return m_; }
			
		private:
			int width_;
			int height_;
			page_margin m_;
		};
	}
}

#endif // FAML_PDF_PAGE_PROPERTY_H
