#ifndef FAML_PDF_SHADING_H
#define FAML_PDF_SHADING_H

#include <string>
#include <vector>
#include "clx/format.h"
#include "coordinate.h"
#include "color.h"

namespace faml {
	namespace pdf {
		/* ----------------------------------------------------------------- */
		//  basic_shading
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_shading {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			
			basic_shading() :
				index_(0), label_(),
				first_(0.0, 0.0), last_(1.0, 1.0), clr_() {
				label_ = makelabel(LITERAL("SH"));
			}
			
			basic_shading(const basic_shading& cp) :
				index_(cp.index_), label_(cp.label_),
				first_(cp.first_), last_(cp.last_),
				clr_(cp.clr_) {}
			
			basic_shading& operator=(const basic_shading& cp) {
				index_ = cp.index_;
				label_ = cp.label_;
				first_ = cp.first_;
				last_ = cp.last;
				clr_.assign(cp.clr_.begin(), cp.clr_.end());
			}
			
			explicit basic_shading(const coordinate& first, const coordinate& last) :
				index_(0), label_(),
				first_(first), last_(last), clr_() {
				label_ = makelabel(LITERAL("SH"));
			}
			
			template <class OutStream, class PDFGenerator>
			bool operator()(OutStream& out, PDFGenerator& gen) {
				if (clr_.size() < 2) return false;
				index_ = gen.newindex();
				this->xput_shading(out, gen);
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
			void first(const coordinate& cp) { first_ = cp; }
			void last(const coordinate& cp) { last_ = cp; }
			void add(const color& elem) { clr_.push_back(elem); }
			
		private:
			typedef clx::basic_format<CharT, Traits> fmt;
			
			size_type index_;
			string_type label_;
			coordinate first_;
			coordinate last_;
			std::vector<color> clr_;
			
			/* ------------------------------------------------------------- */
			//  xput_shading
			/* ------------------------------------------------------------- */
			template <class OutStream, class PDFGenerator>
			bool xput_shading(OutStream& out, PDFGenerator& gen) {
				out << fmt(LITERAL("%d 0 obj")) % index_ << std::endl;
				out << LITERAL("<<") << std::endl;
				out << fmt(LITERAL("/ShadingType %d")) % 2 << std::endl;
				out << LITERAL("/ColorSpace /DeviceRGB") << std::endl;
				out << fmt(LITERAL("/Coords [ %f %f %f %f ]"))
					% first_.x() % first_.y() % last_.x() % last_.y() << std::endl;
				out << fmt(LITERAL("/Function %d 0 R")) % (index_ + 1) << std::endl;
				out << LITERAL("/Extend [ true true ]") << std::endl;
				out << LITERAL(">>") << std::endl;
				out << LITERAL("endobj") << std::endl;
				out << std::endl;
				
				this->xput_function(out, gen);
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xput_function
			/* ------------------------------------------------------------- */
			template <class OutStream, class PDFGenerator>
			bool xput_function(OutStream& out, PDFGenerator& gen) {
				out << fmt(LITERAL("%d 0 obj")) % gen.newindex() << std::endl;
				out << LITERAL("<<") << std::endl;
				out << fmt(LITERAL("/FunctionType %d")) % 2 << std::endl;
				out << LITERAL("/Domain [ 0.0 1.0 ]") << std::endl;
				out << fmt(LITERAL("/C0 [ %f %f %f ]"))
					% clr_.at(0).red() % clr_.at(0).green() % clr_.at(0).blue() << std::endl;
				out << fmt(LITERAL("/C1 [ %f %f %f ]"))
					% clr_.at(1).red() % clr_.at(1).green() % clr_.at(1).blue() << std::endl;
				out << LITERAL("/N 1.0") << std::endl;
				out << LITERAL(">>") << std::endl;
				out << LITERAL("endobj") << std::endl;
				out << std::endl;
				
				return true;
			}
		};
	}
}

#endif // FAML_PDF_SHADING_H
