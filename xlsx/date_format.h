#ifndef FAML_XLSX_DATE_FORMAT_H
#define FAML_XLSX_DATE_FORMAT_H

#include <ios>
#include <iomanip>
#include <string>
#include <sstream>
#include "clx/literal.h"
#include "clx/predicate.h"
#include "clx/date_time.h"

namespace faml {
	namespace xlsx {
		/* ----------------------------------------------------------------- */
		//  is_datefmt
		/* ----------------------------------------------------------------- */
		template <class Ch, class Tr>
		inline bool is_datefmt(const std::basic_string<Ch, Tr>& fmt) {
			typedef std::basic_string<Ch, Tr> string_type;
			
			if (fmt.find(LITERAL("yyyy")) != string_type::npos ||
				fmt.find(LITERAL("yy")) != string_type::npos ||
				fmt.find(LITERAL("ggg")) != string_type::npos ||
				fmt.find(LITERAL("gg")) != string_type::npos ||
				fmt.find(LITERAL("ee")) != string_type::npos ||
				fmt.find(LITERAL("mmmmm")) != string_type::npos ||
				fmt.find(LITERAL("mmmm")) != string_type::npos ||
				fmt.find(LITERAL("mmm")) != string_type::npos ||
				fmt.find(LITERAL("mm")) != string_type::npos ||
				fmt.find(LITERAL("dd")) != string_type::npos ||
				fmt.find(LITERAL("hh")) != string_type::npos ||
				fmt.find(LITERAL("ss")) != string_type::npos ||
				fmt.find(LITERAL("AM/PM")) != string_type::npos ||
				fmt.find(LITERAL("午前/午後")) != string_type::npos) {
				// 1文字の場合は，誤判定の可能性が大きくなるので後述．
				return true;
			}
			
			if ((fmt.find(LITERAL("y")) != string_type::npos &&
				 fmt.find(LITERAL("m")) != string_type::npos) ||
				(fmt.find(LITERAL("e")) != string_type::npos &&
				 fmt.find(LITERAL("m")) != string_type::npos) ||
				(fmt.find(LITERAL("m")) != string_type::npos &&
				 fmt.find(LITERAL("d")) != string_type::npos) ||
				(fmt.find(LITERAL("h")) != string_type::npos &&
				 fmt.find(LITERAL("m")) != string_type::npos) ||
				(fmt.find(LITERAL("m")) != string_type::npos &&
				 fmt.find(LITERAL("s")) != string_type::npos)) {
				return true;
			}
			
			return false;
		}
		
		/* ----------------------------------------------------------------- */
		//  date_format
		/* ----------------------------------------------------------------- */
		struct date_format {
		public:
			/* ------------------------------------------------------------- */
			//  main operator
			/* ------------------------------------------------------------- */
			template <class Ch, class Tr>
			static bool put(std::basic_string<Ch, Tr>& dest,
				const std::basic_string<Ch, Tr>& fmt, double value) {
				typedef std::basic_string<Ch, Tr> string_type;
				
				if (value < 1) value += 30000; // 1970年1月1日以降の適当な日付を選択する．
				std::time_t tmp = static_cast<std::time_t>((value - 25569.0) * 86400.0) - 32400;
				clx::date_time t(tmp);
				std::basic_stringstream<Ch, Tr> ss;
				
				size_t pos = 0;
				bool mon = true; // m が month か minute かの判定用．
				bool half = false; // 12/24 時間制の判定 (AM/PM 表記だと多くの場合，意味なし・・・）．
				while (pos < fmt.size()) {
					Ch c = fmt.at(pos);
					++pos;
					if (c == LITERAL('"')) {
						while (pos < fmt.size() && fmt.at(pos) != LITERAL('"')) ss << fmt.at(pos++);
						if (pos < fmt.size()) ++pos;
					}
					else if (c == LITERAL('[')) {
						string_type sys;
						while (pos < fmt.size() && fmt.at(pos) != LITERAL(']')) sys += fmt.at(pos++);
						if (pos < fmt.size()) ++pos;
						if (sys.at(0) == LITERAL('$')) {
							size_t last = sys.find_first_of(LITERAL("-"));
							if (last != string_type::npos && last + 1 < fmt.size()) ++last;
							sys.erase(0, last);
							try { // $-F800 と $-F400 は特殊コード．
								size_t lang = clx::lexical_cast<size_t>(sys, std::ios::hex);
								if (lang == 0xf800) {
									ss << clx::basic_format<Ch, Tr>(LITERAL("%d年%d月%d日")) %
										t.year() % t.month() % t.day();
									break;
								}
								else if (lang == 0xf400) {
									ss << clx::basic_format<Ch, Tr>(LITERAL("%d:%02d:%02d")) %
										t.hour() % t.minute() % t.second();
									break;
								}
							}
							catch (clx::bad_lexical_cast&) {}
						}
					}
					else if (c == LITERAL('\\')) ss << fmt.at(pos++);
					else if (c == LITERAL('y')) xput_year(ss, fmt, pos, t);
					else if (c == LITERAL('g')) xput_era(ss, fmt, pos, t);
					else if (c == LITERAL('e')) xput_jyear(ss, fmt, pos, t);
					else if (c == LITERAL('m')) { // TODO: どうやって month/minute の判別を行うか．
						if (!mon) xput_minute(ss, fmt, pos, t);
						else {
							if (fmt.find(LITERAL("h")) < pos ||
								fmt.find(LITERAL("s"), pos) != string_type::npos) {
								xput_minute(ss, fmt, pos, t);
								mon = true;
							}
							else {
								xput_month(ss, fmt, pos, t);
								mon = false;
							}
						}
					}
					else if (c == LITERAL('d')) xput_day(ss, fmt, pos, t);
					else if (c == LITERAL('h')) xput_hour(ss, fmt, pos, t, half);
					else if (c == LITERAL('s')) xput_second(ss, fmt, pos, t);
					else if (c == LITERAL('A')) {
						half = true;
						xput_ampm(ss, fmt, pos, t);
					}
					else if (c == 0xe5) {
						half = true;
						xput_jampm(ss, fmt, pos, t);
					}
					else if (clx::is_any_of(LITERAL(":-/ "))(c)) ss << c;
					else if (c == LITERAL(';')) break;
					// skip if others
				}
				
				dest = ss.str();
				return true;
			}
			
		private:
			/* ------------------------------------------------------------- */
			//  xput_era
			/* ------------------------------------------------------------- */
			template <class Ch, class Tr, class DateT>
			static bool xput_era(std::basic_ostream<Ch, Tr>& out,
				const std::basic_string<Ch, Tr>& fmt, size_t& pos, const DateT& t) {
				static const Ch fullname[][12] = {
					LITERAL("明治"), LITERAL("大正"),
					LITERAL("昭和"), LITERAL("平成")
				};
				
				static const Ch halfname[][12] = {
					LITERAL("明"), LITERAL("大"),
					LITERAL("昭"), LITERAL("平")
				};
				
				static const Ch initial[][12] = {
					LITERAL("M"), LITERAL("T"),
					LITERAL("S"), LITERAL("H")
				};
				
				size_t n = 1;
				while (pos < fmt.size() && fmt.at(pos) == LITERAL('g')) {
					++n;
					++pos;
				}
				
				size_t era = 0;
				if (t.year() < 1912) era = 0;
				else if (t.year() < 1926) era = 1;
				else if (t.year() < 1989) era = 2;
				else era = 3;
				
				switch (n) {
				case 1:
					out << initial[era];
					break;
				case 2:
					out << halfname[era];
					break;
				case 3:
					out << fullname[era];
					break;
				default:
					out << fullname[era];
					break;
				}
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xput_jyear
			/* ------------------------------------------------------------- */
			template <class Ch, class Tr, class DateT>
			static bool xput_jyear(std::basic_ostream<Ch, Tr>& out,
				const std::basic_string<Ch, Tr>& fmt, size_t& pos, const DateT& t) {
				size_t n = 1;
				while (pos < fmt.size() && fmt.at(pos) == LITERAL('e')) {
					++n;
					++pos;
				}
				
				size_t y = t.year();
				if (t.year() < 1912) y -= 1867;
				else if (t.year() < 1926) y -= 1911;
				else if (t.year() < 1989) y -= 1925;
				else y -= 1988;
				
				switch (n) {
				case 1:
					out << y;
					break;
				case 2:
					out << std::setw(2) << std::setfill(LITERAL('0')) << y;
					break;
				default:
					out << y;
					break;
				}
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xput_year
			/* ------------------------------------------------------------- */
			template <class Ch, class Tr, class DateT>
			static bool xput_year(std::basic_ostream<Ch, Tr>& out,
				const std::basic_string<Ch, Tr>& fmt, size_t& pos, const DateT& t) {
				size_t n = 1;
				while (pos < fmt.size() && fmt.at(pos) == LITERAL('y')) {
					++n;
					++pos;
				}
				
				switch (n) {
				case 2:
				{
					int cent = (t.year() < 2000) ? 1900 : 2000;
					out << std::setw(2) << std::setfill('0') << (t.year() - cent);
					break;
				}
				case 4:
					out << t.year();
					break;
				default:
					out << t.year();
					break;
				}
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xput_month
			/* ------------------------------------------------------------- */
			template <class Ch, class Tr, class DateT>
			static bool xput_month(std::basic_ostream<Ch, Tr>& out,
				const std::basic_string<Ch, Tr>& fmt, size_t& pos, const DateT& t) {
				static const Ch initial[][12] = {
					LITERAL("J"), LITERAL("F"),
					LITERAL("M"), LITERAL("A"),
					LITERAL("M"), LITERAL("J"),
					LITERAL("J"), LITERAL("A"),
					LITERAL("S"), LITERAL("O"),
					LITERAL("N"), LITERAL("D")
				};
				
				size_t n = 1;
				while (pos < fmt.size() && fmt.at(pos) == LITERAL('m')) {
					++n;
					++pos;
				}
				
				switch (n) {
				case 1:
					out << t.month();
					break;
				case 2:
					out << std::setw(2) << std::setfill(LITERAL('0')) << t.month();
					break;
				case 3:
					out << t.to_string(LITERAL("%b"));
					break;
				case 4:
					out << t.to_string(LITERAL("%B"));
					break;
				case 5:
					out << initial[t.month() - 1];
					break;
				default:
					out << t.month();
					break;
				}
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xput_day
			/* ------------------------------------------------------------- */
			template <class Ch, class Tr, class DateT>
			static bool xput_day(std::basic_ostream<Ch, Tr>& out,
				const std::basic_string<Ch, Tr>& fmt, size_t& pos, const DateT& t) {
				
				size_t n = 1;
				while (pos < fmt.size() && fmt.at(pos) == LITERAL('d')) {
					++n;
					++pos;
				}
				
				switch (n) {
				case 1:
					out << t.day();
					break;
				case 2:
					out << std::setw(2) << std::setfill(LITERAL('0')) << t.day();
					break;
				case 3:
					out << t.to_string(LITERAL("%a"));
					break;
				case 4:
					out<< t.to_string(LITERAL("%A"));
					break;
				default:
					out << t.day();
					break;
				}
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xput_hour
			/* ------------------------------------------------------------- */
			template <class Ch, class Tr, class DateT>
			static bool xput_hour(std::basic_ostream<Ch, Tr>& out,
				const std::basic_string<Ch, Tr>& fmt, size_t& pos, const DateT& t, bool half) {
				size_t n = 1;
				while (pos < fmt.size() && fmt.at(pos) == LITERAL('h')) {
					++n;
					++pos;
				}
				
				if (!half) {
					if (fmt.find(LITERAL("AM/PM")) != std::basic_string<Ch, Tr>::npos) half = true;
				}
				size_t h = t.hour();
				if (half) {
					if (h > 12) h -= 12;
					else if (h == 0) h = 12;
				}
				
				switch (n) {
				case 1:
					out << h;
					break;
				case 2:
					out << std::setw(2) << std::setfill(LITERAL('0')) << h;
				default:
					out << h;
					break;
				}
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xput_minute
			/* ------------------------------------------------------------- */
			template <class Ch, class Tr, class DateT>
			static bool xput_minute(std::basic_ostream<Ch, Tr>& out,
				const std::basic_string<Ch, Tr>& fmt, size_t& pos, const DateT& t) {
				size_t n = 1;
				while (pos < fmt.size() && fmt.at(pos) == LITERAL('m')) {
					++n;
					++pos;
				}
				
				switch (n) {
				case 1:
					out << t.minute();
					break;
				case 2:
					out << std::setw(2) << std::setfill(LITERAL('0')) << t.minute();
					break;
				default:
					out << t.minute();
					break;
				}
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xput_second
			/* ------------------------------------------------------------- */
			template <class Ch, class Tr, class DateT>
			static bool xput_second(std::basic_ostream<Ch, Tr>& out,
				const std::basic_string<Ch, Tr>& fmt, size_t& pos, const DateT& t) {
				size_t n = 1;
				while (pos < fmt.size() && fmt.at(pos) == LITERAL('s')) {
					++n;
					++pos;
				}
				
				switch (n) {
				case 1:
					out << t.second();
					break;
				case 2:
					out << std::setw(2) << std::setfill(LITERAL('0')) << t.second();
					break;
				default:
					out << t.minute();
					break;
				}
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xput_ampm
			/* ------------------------------------------------------------- */
			template <class Ch, class Tr, class DateT>
			static bool xput_ampm(std::basic_ostream<Ch, Tr>& out,
				const std::basic_string<Ch, Tr>& fmt, size_t& pos, const DateT& t) {
				if (t.hour() < 12) out << LITERAL("AM");
				else out << LITERAL("PM");
				pos += 5;
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  xput_jampm
			/* ------------------------------------------------------------- */
			template <class Ch, class Tr, class DateT>
			static bool xput_jampm(std::basic_ostream<Ch, Tr>& out,
				const std::basic_string<Ch, Tr>& fmt, size_t& pos, const DateT& t) {
				if (t.hour() < 12) out << LITERAL("午前");
				else out << LITERAL("午後");
				pos += 13;
				return true;
			}
		};
	}
}
#endif // FAML_XLSX_DATE_FORMAT_H
