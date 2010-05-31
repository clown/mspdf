/* ------------------------------------------------------------------------- */
/*
 *  coordinate.h
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
 *  Last-modified: Tue 14 Apr 2009 13:06:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PDF_COORDINATE_H
#define FAML_PDF_COORDINATE_H

namespace faml {
	namespace pdf {
		/* ----------------------------------------------------------------- */
		//  coordinate
		/* ----------------------------------------------------------------- */
		class coordinate {
		public:
			explicit coordinate(double x = 0, double y = 0) :
				x_(x), y_(y) {}
			
			void x(double cp) { x_ = cp; }
			void y(double cp) { y_ = cp; }
			
			double x() const { return x_; }
			double y() const { return y_; }
			
		private:
			double x_;
			double y_;
		};
		
		/* ----------------------------------------------------------------- */
		//  square
		/* ----------------------------------------------------------------- */
		class square {
		public:
			square() :
				ll_(0), ur_(0) {}
			
			explicit square(const coordinate& ll, const coordinate& ur) :
				ll_(ll), ur_(ur) {}
			
			void lower_left(const coordinate& ll) { ll_ = ll; }
			void upper_right(const coordinate& ur) { ur_ = ur; }
			
			const coordinate& lower_left() const { return ll_; }
			const coordinate& upper_right() const { return ur_; }
			
		private:
			coordinate ll_;
			coordinate ur_;
		};
	}
}

#endif // FAML_PDF_COORDINATE_H
