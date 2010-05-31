#ifndef FAML_XLSX_FORMAT_H
#define FAML_XLSX_FORMAT_H

#include <ios>
#include <iomanip>
#include <string>
#include <sstream>
#include "clx/literal.h"
#include "clx/predicate.h"
#include "clx/date_time.h"
#include "../officex/color.h"
#include "format_code.h"
#include "date_format.h"

namespace faml {
	namespace xlsx {
		namespace detail {
			/* ----------------------------------------------------------------- */
			//  numpunct
			/* ----------------------------------------------------------------- */
			template <class Ch, class Tr>
			inline bool numpunct(std::basic_string<Ch, Tr>& dest) {
				if (dest.empty()) return true;
				
				std::basic_string<Ch, Tr> tmp;
				std::basic_string<Ch, Tr> frac;
				size_t pos = dest.find(LITERAL("."));
				if (pos != std::basic_string<Ch, Tr>::npos) {
					frac = dest.substr(pos);
					dest.erase(pos);
				}
				
				pos = 0;
				while (pos < dest.size()) {
					if (std::isdigit(dest.at(pos))) break;
					tmp += dest.at(pos);
					++pos;
				}
				
				if (dest.size() > 0) {
					size_t n = (dest.size() - pos) % 3;
					if (n == 0) n += 3;
					tmp += dest.substr(pos, n);
					pos += n;
					
					for (; pos < dest.size(); pos += 3) {
						tmp += LITERAL(",") + dest.substr(pos, 3);
					}
				}
				
				dest = tmp + frac;
				
				return true;
			}
			
			/* ----------------------------------------------------------------- */
			//  numfrac
			/* ----------------------------------------------------------------- */
			template <class Ch, class Tr>
			inline bool numfrac(std::basic_ostream<Ch, Tr>& out,
				const std::basic_string<Ch, Tr>& fmt, size_t& pos, double value) {
				size_t frac = 0;
				bool fixed = true;
				if (pos < fmt.size() && fmt.at(pos) == LITERAL('.')) {
					++pos;
					while (pos < fmt.size() && fmt.at(pos) == LITERAL('0')) {
						++frac;
						++pos;
					}
					
					if (pos < fmt.size() && fmt.at(pos) == LITERAL('E')) {
						fixed = false;
						pos += 4;
					}
				}
				
				std::basic_stringstream<Ch, Tr> ss;
				ss.setf(std::ios_base::uppercase);
				if (fixed) {
					ss << std::setiosflags(std::ios::fixed);
					ss << std::setprecision(frac) << value;
				}
				else ss << std::showpoint << std::setprecision(frac + 1) << value;
				out << ss.str();
				
				return true;
			}
			
			/* ----------------------------------------------------------------- */
			//  numfmt
			/* ----------------------------------------------------------------- */
			template <class Ch, class Tr>
			inline bool numfmt(std::basic_ostream<Ch, Tr>& out,
				const std::basic_string<Ch, Tr>& fmt, size_t& pos, double value) {
				bool punct = true;
				while (pos < fmt.size() && fmt.at(pos) == LITERAL('#')) ++pos;
				if (pos < fmt.size() && fmt.at(pos) == LITERAL(',')) pos += 4;
				else {
					punct = false;
					if (pos < fmt.size() && fmt.at(pos) == LITERAL('0')) ++pos;
				}
				
				size_t frac = 0;
				bool fixed = true;
				if (pos < fmt.size() && fmt.at(pos) == LITERAL('.')) {
					++pos;
					while (pos < fmt.size() && fmt.at(pos) == LITERAL('0')) {
						++frac;
						++pos;
					}
					
					if (pos < fmt.size() && fmt.at(pos) == LITERAL('E')) {
						fixed = false;
						pos += 4;
					}
				}
				
				std::basic_stringstream<Ch, Tr> ss;
				ss.setf(std::ios_base::uppercase);
				if (fixed) {
					ss << std::setiosflags(std::ios::fixed);
					ss << std::setprecision(frac) << value;
				}
				else ss << std::showpoint << std::setprecision(frac + 1) << value;
				
				std::basic_string<Ch, Tr> num = ss.str();
				if (punct) faml::xlsx::detail::numpunct(num);
				out << num;
				
				return true;
			}
		}
		
		/* ----------------------------------------------------------------- */
		//  format
		/* ----------------------------------------------------------------- */
		template <class Ch, class Tr>
		inline bool format(std::basic_string<Ch, Tr>& dest, size_t& rgb,
			const std::basic_string<Ch, Tr>& fmt, double value) {
			typedef std::basic_string<Ch, Tr> string_type;
			std::basic_stringstream<Ch, Tr> ss;
			if (fmt.empty()) {
				ss << std::setiosflags(std::ios::fixed) << value;
				dest = ss.str();
				return true;
			}
			
			if (is_datefmt(fmt)) return date_format::put(dest, fmt, value);
			
			size_t pos = 0;
			double x = (fmt.find(LITERAL("%")) != string_type::npos) ? value * 100 : value;
			if (x < 0.0) {
				size_t next = fmt.find_first_of(LITERAL(";"));
				if (next != string_type::npos && next + 1 < fmt.size()) {
					pos = next + 1;
					x = -x;
				}
			}
			
			while (pos < fmt.size()) {
				Ch c = fmt.at(pos);
				++pos;
				if (c == LITERAL('"')) {
					while (pos < fmt.size() && fmt.at(pos) != LITERAL('"')) {
						
						if (fmt.at(pos) == 0x5c) ss << LITERAL("￥");
						else if (static_cast<unsigned char>(fmt.at(pos)) == 0xc2 &&
							static_cast<unsigned char>(fmt.at(pos + 1)) == 0xa5) {
							ss << LITERAL("￥");
							++pos;
						}
						else ss << fmt.at(pos);
						++pos;
					}
					if (pos < fmt.size()) ++pos;
				}
				else if (c == LITERAL('[')) {
					string_type elem;
					while (pos < fmt.size() && fmt.at(pos) != LITERAL(']')) elem += fmt.at(pos++);
					if (pos < fmt.size()) ++pos;
					if (!elem.empty()) rgb = faml::officex::getrgb(elem);
				}
				else if (c == LITERAL('\\')) ss << fmt.at(pos++);
				else if (c == LITERAL('@')) ss << x;
				else if (c == LITERAL('#')) detail::numfmt(ss, fmt, pos, x);
				else if (c == LITERAL('0')) detail::numfrac(ss, fmt, pos, x);
				else if (clx::is_any_of(LITERAL("$-+/():!^&`~{}<>=% "))(c)) ss << c;
				else if (c == LITERAL(';')) break;
			}
			
			dest = ss.str();
			return true;
		}
		
		/* ----------------------------------------------------------------- */
		/*
		 *  format
		 *
		 *  SharedString (SST) に対して書式が指定されている場合．
		 */
		/* ----------------------------------------------------------------- */
		template <class Ch, class Tr>
		inline bool format(std::basic_string<Ch, Tr>& dest,
			const std::basic_string<Ch, Tr>& fmt, const std::basic_string<Ch, Tr>& value) {
			typedef std::basic_string<Ch, Tr> string_type;
			if (fmt.empty()) return false;
			std::basic_stringstream<Ch, Tr> ss;
			size_t pos = 0;
			while (pos < fmt.size()) {
				Ch c = fmt.at(pos);
				++pos;
				if (c == LITERAL('"')) {
					while (pos < fmt.size() && fmt.at(pos) != LITERAL('"')) {
						
						if (fmt.at(pos) == 0x5c) ss << LITERAL("￥");
						else if (static_cast<unsigned char>(fmt.at(pos)) == 0xc2 &&
							static_cast<unsigned char>(fmt.at(pos + 1)) == 0xa5) {
							ss << LITERAL("￥");
							++pos;
						}
						else ss << fmt.at(pos);
						++pos;
					}
					if (pos < fmt.size()) ++pos;
				}
				else if (c == LITERAL('[')) {
					while (pos < fmt.size() && fmt.at(pos) != LITERAL(']')) ++pos;
					if (pos < fmt.size()) ++pos;
				}
				else if (c == LITERAL('\\')) ss << fmt.at(pos++);
				else if (c == LITERAL('@')) ss << value;
				else if (clx::is_any_of(LITERAL("$-+/():!^&`~{}<>=% "))(c)) ss << c;
			}
			
			dest = ss.str();
			return true;
		}
	}
}

#endif // FAML_XLSX_FORMAT_H
