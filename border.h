/* ------------------------------------------------------------------------- */
/*
 *  border.h
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
 *  Last-modified: Thu 09 Jul 2009 16:52:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_BORDER_H
#define FAML_BORDER_H

#include "color.h"

namespace faml {
	using faml::pdf::color;
	
	/* --------------------------------------------------------------------- */
	//  border
	/* --------------------------------------------------------------------- */
	class border {
	public:
		typedef size_t size_type;
		
		enum {
			none			= 0x000,
			solid			= 0x001,
			dash			= 0x002,
			dot				= 0x003,
			hair			= 0x004,
			dash_dot		= 0x005,
			dash_wdot		= 0x006,
			doubled			= 0x010,
			thin_double		= 0x030, // base の線よりも細め
			thick_double	= 0x050  // base の線よりも太め
		};
		
		border() : type_(0), weight_(0.5), fill_() {}
		
		explicit border(const color& clr, double weight, size_type type = 0) :
			type_(type), weight_(weight), fill_(clr) {}
		
		/* ----------------------------------------------------------------- */
		//  Access methods (get).
		/* ----------------------------------------------------------------- */
		size_type type() const { return type_; }
		double weight() const { return weight_; }
		const color& fill() const { return fill_; }
		
		/* ----------------------------------------------------------------- */
		//  Access methods (set).
		/* ----------------------------------------------------------------- */
		size_type& type() { return type_; }
		double& weight() { return weight_; }
		color& fill() { return fill_; }
		
		void type(size_type cp) { type_ = cp; }
		void weight(double cp) { weight_ = cp; }
		void fill(const color& cp) { fill_ = cp; }
		
	private:
		size_type type_;
		double weight_;
		color fill_;
	};
}

#endif // FAML_BORDER_H
