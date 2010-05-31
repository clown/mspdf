/* ------------------------------------------------------------------------- */
/*
 *  color.h
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
 *  Last-modified: Sun 19 Apr 2009 17:27:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PDF_COLOR_H
#define FAML_PDF_COLOR_H

namespace faml {
	namespace pdf {
		/* ----------------------------------------------------------------- */
		//  color
		/* ----------------------------------------------------------------- */
		class color {
		public:
			typedef size_t size_type;
			
			color() :
				red_(0.0), green_(0.0), blue_(0.0), valid_(false) {}
			
			explicit color(double red, double green, double blue) :
				red_(red), green_(green), blue_(blue), valid_(true) {}
			
			explicit color(size_type value) :
				red_(0.0), green_(0.0), blue_(0.0) {
				this->rgb(value);
			}
			
			void rgb(size_type value) {
				red_ = ((value >> 16) & 0xff) / 255.0;
				green_ = ((value >> 8) & 0xff) / 255.0;
				blue_ = (value & 0xff) / 255.0;
				valid_ = true;
			}
			
			void rgb(double red, double green, double blue) {
				red_ = red;
				green_ = green;
				blue_ = blue;
				valid_ = true;
			}
			
			double red() const { return red_; }
			double green() const { return green_; }
			double blue() const { return blue_; }
			bool is_valid() const { return valid_; }
			void validate() { valid_ = true; }
			void invalidate() { valid_ = false; }
			
		private:
			double red_;
			double green_;
			double blue_;
			bool valid_;
		};
	}
}

#endif // FAML_PDF_COLOR_H
