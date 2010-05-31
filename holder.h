/* ------------------------------------------------------------------------- */
/*
 *  holder.h
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
 *  Last-modified: Sun 19 Apr 2009 19:55:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PDF_HOLDER_H
#define FAML_PDF_HOLDER_H

#include <ostream>
#include <vector>
#include <string>
#include "clx/shared_ptr.h"

namespace faml {
	namespace pdf {
		template <class CharT, class Traits> class basic_generator;
		
		namespace detail {
			/* ------------------------------------------------------------- */
			//  base_holder
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits
			>
			class base_holder {
			public:
				typedef size_t size_type;
				typedef std::basic_ostream<CharT, Traits> ostream_type;
				typedef std::basic_string<CharT, Traits> string_type;
				typedef basic_generator<CharT, Traits> generator;
				
				base_holder() {}
				virtual ~base_holder() {}
				virtual size_type index() const = 0;
				virtual const string_type& label() const = 0;
				virtual bool run(ostream_type& out, generator& pm) = 0;
			};
		};
		
		/* ----------------------------------------------------------------- */
		//  basic_holder
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_holder {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			
			basic_holder() : v_() {}
			
			template <class Object>
			void add(Object& f) {
				object_ptr elem(new any_object<Object>(f));
				v_.push_back(elem);
			}
			
			template <class OutStream, class PDFGenerator>
			bool operator()(OutStream& out, PDFGenerator& gen) {
				bool status = true;
				for (size_type i = 0; i < v_.size(); ++i) {
					//if (v_.at(i)->index() > 0) continue;
					status &= v_.at(i)->run(out, gen);
				}
				return status;
			}
			
			void clear() { v_.clear(); }
			
			bool empty() const { return v_.empty(); }
			size_type size() const { return v_.size(); }
			size_type index(size_type pos) const { return v_.at(pos)->index(); }
			const string_type& label(size_type pos) const { return v_.at(pos)->label(); }
			
		private:
			typedef detail::base_holder<CharT, Traits> base_type;
			typedef clx::shared_ptr<base_type> object_ptr;
			typedef std::vector<object_ptr> object_array;
			
			template <class F>
			class any_object : public base_type {
			public:
				typedef typename base_type::size_type size_type;
				typedef typename base_type::ostream_type ostream_type;
				typedef typename base_type::string_type string_type;
				typedef typename base_type::generator generator;
				
				any_object(F f) : f_(f) {}
				virtual ~any_object() {}
				virtual size_type index() const { return f_.index(); }
				virtual const string_type& label() const { return f_.label(); }
				virtual bool run(ostream_type& out, generator& gen) { return f_(out, gen); }
				
			private:
				F f_;
			};
			
			object_array v_;
		};
	}
}

#endif // FAML_PDF_HOLDER_H
