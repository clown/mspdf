#ifndef FAML_PDF_ALPHA_H
#define FAML_PDF_ALPHA_H

#include <string>
#include <vector>
#include "clx/format.h"
#include "coordinate.h"
#include "color.h"

namespace faml {
	namespace pdf {
		/* ----------------------------------------------------------------- */
		//  basic_alpha
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_alpha {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			
			basic_alpha() :
				index_(0), label_() {
				label_ = makelabel(LITERAL("GS"));
			}
			
			template <class OutStream, class PDFGenerator>
			bool operator()(OutStream& out, PDFGenerator& gen) {
				index_ = gen.newindex();
				this->xput_alpha(out, gen);
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			size_type index() const { return index_; }
			const string_type& label() const { return label_; }
			
			/* ------------------------------------------------------------- */
			//  Access methods (set).
			/* ------------------------------------------------------------- */
			void value(const double cp) { value_ = cp; }
			
		private:
			typedef clx::basic_format<CharT, Traits> fmt;
			
			size_type index_;
			string_type label_;
			double value_;
			
			/* ------------------------------------------------------------- */
			//  xput_alpha
			/* ------------------------------------------------------------- */
			template <class OutStream, class PDFGenerator>
			bool xput_alpha(OutStream& out, PDFGenerator& gen) {
				out << fmt(LITERAL("%d 0 obj")) % index_ << std::endl;
				out << LITERAL("<<") << std::endl;
				out << "/Type /ExtGState" << std::endl;
				out << fmt(LITERAL("/CA %f")) % value_ << std::endl;
				out << fmt(LITERAL("/ca %f")) % value_ << std::endl;
				out << LITERAL(">>") << std::endl;
				out << LITERAL("endobj") << std::endl;
				out << std::endl;
				
				return true;
			}
		};
	}
}

#endif // FAML_PDF_ALPHA_H
