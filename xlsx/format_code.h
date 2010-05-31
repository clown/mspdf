#ifndef FAML_XLSX_FORMAT_CODE_H
#define FAML_XLSX_FORMAT_CODE_H

#include <string>
#include "clx/literal.h"

namespace faml {
	namespace xlsx {
		/* ----------------------------------------------------------------- */
		//  format_code
		/* ----------------------------------------------------------------- */
		template <class Ch, class Tr>
		inline bool format_code(std::basic_string<Ch, Tr>& dest, size_t id) {
			switch (id) {
			case 0:
				dest = LITERAL("");
			case 1:
				dest = LITERAL("0");
				break;
			case 2:
				dest = LITERAL("0.00");
				break;
			case 3:
				dest = LITERAL("#,##0");
				break;
			case 4:
				dest = LITERAL("#,##0.00");
				break;
			case 9:
				dest = LITERAL("0%");
				break;
			case 10:
				dest = LITERAL("0.00%");
				break;
			case 11:
				dest = LITERAL("0.00E+00");
				break;
			case 14:
				dest = LITERAL("yyyy/m/d");
				break;
			case 15:
				dest = LITERAL("d-mmm-yy");
				break;
			case 16:
				dest = LITERAL("d-mmm");
				break;
			case 17:
				dest = LITERAL("mmm-yy");
				break;
			case 18:
				dest = LITERAL("h:mm AM/PM");
				break;
			case 19:
				dest = LITERAL("h:mm:ss AM/PM");
				break;
			case 20:
				dest = LITERAL("h:mm");
				break;
			case 21:
				dest = LITERAL("h:mm:ss");
				break;
			case 22:
				dest = LITERAL("yyyy/m/d h:mm");
				break;
			case 37:
				dest = LITERAL("#,##0 ;(#,##0)");
				break;
			case 38:
				dest = LITERAL("#,##0 ;[Red](#,##0)");
				break;
			case 39:
				dest = LITERAL("#,##0.00;(#,##0.00)");
				break;
			case 40:
				dest = LITERAL("#,#0.00;[Red](#,##0.00)");
				break;
			case 45:
				dest = LITERAL("mm:ss");
				break;
			case 46:
				dest = LITERAL("[h]:mm:ss");
				break;
			case 47:
				dest = LITERAL("mmss.0");
				break;
			case 48:
				dest = LITERAL("##0.0E+0");
				break;
#if 1 // ローカライズ (ja-jp)
			case 27:
				dest = LITERAL("[$-411]ge.m.d");
				break;
			case 28:
			case 29:
				dest = LITERAL("[$-411]ggge\"年\"m\"月\"d\"日\"");
				break;
			case 30:
				dest = LITERAL("m/d/yy");
				break;
			case 31:
				dest = LITERAL("yyyy\"年\"m\"月\"d\"日\"");
				break;
			case 32:
				dest = LITERAL("h\"時\"mm\"分\"");
				break;
			case 33:
				dest = LITERAL("h\"時\"mm\"分\"ss\"秒\"");
				break;
			case 34:
				dest = LITERAL("yyyy\"年\"m\"月\"");
				break;
			case 35:
				dest = LITERAL("m\"月\"d\"日\"");
				break;
			case 36:
			case 50:
				dest = LITERAL("[$-411]ge.m.d");
				break;
			case 51:
			case 54:
				dest = LITERAL("[$-411]ggge\"年\"m\"月\"d\"日\"");
				break;
			case 52:
			case 55:
				dest = LITERAL("yyyy\"年\"m\"月\"");
				break;
			case 53:
			case 56:
				dest = LITERAL("m\"月\"d\"日\"");
				break;
			case 57:
				dest = LITERAL("[$-411]ge.m.d");
				break;
			case 58:
				dest = LITERAL("[$-411]ggge\"年\"m\"月\"d\"日\"");
				break;
#endif // ローカライズ (ja-jp)
			default:
				dest = LITERAL("@");
				break;
			}
			
			return true;
		}
	}
}

#endif // FAML_XLSX_FORMAT_CODE_H
