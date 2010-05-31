#ifndef FAML_PDF_CONTENTS_MSSHAPE_H
#define FAML_PDF_CONTENTS_MSSHAPE_H

#include "shape.h"
#include "shape_factory.h"

namespace faml {
	namespace pdf {
		namespace contents {
			/* ------------------------------------------------------------- */
			//  make_msshape
			/* ------------------------------------------------------------- */
			template <class Ch, class Tr>
			inline clx::shared_ptr<basic_shape<Ch, Tr> > make_msshape(size_t type,
				const coordinate& o, double w, double h, bool entag = true) {
				typedef clx::shared_ptr<basic_shape<Ch, Tr> > shape_ptr;
				switch (type) {
				case   3: return shape_ptr(new basic_ellipse<Ch, Tr>(o, w, h, entag));
				case   1: return shape_ptr(new basic_rectangle<Ch, Tr>(o, w, h, entag));
				case   5: return shape_ptr(new basic_triangle<Ch, Tr>(o, w, h, entag));
				case   6: return shape_ptr(new basic_right_triangle<Ch, Tr>(o, w, h, entag));
				case  56: return shape_ptr(new basic_pentagon<Ch, Tr>(o, w, h, entag));
				case   9: return shape_ptr(new basic_hexagon<Ch, Tr>(o, w, h, entag));
				case  13: return shape_ptr(new basic_right_arrow<Ch, Tr>(o, w, h, entag));
				case  66: return shape_ptr(new basic_left_arrow<Ch, Tr>(o, w, h, entag));
				case  67: return shape_ptr(new basic_down_arrow<Ch, Tr>(o, w, h, entag));
				case  68: return shape_ptr(new basic_up_arrow<Ch, Tr>(o, w, h, entag));
				case  85: return shape_ptr(new basic_left_bracket<Ch, Tr>(o, w, h, entag));
				case  86: return shape_ptr(new basic_right_bracket<Ch, Tr>(o, w, h, entag));
				case  87: return shape_ptr(new basic_left_brace<Ch, Tr>(o, w, h, entag));
				case  88: return shape_ptr(new basic_right_brace<Ch, Tr>(o, w, h, entag));
				case  16: return shape_ptr(new basic_cube<Ch, Tr>(o, w, h, entag));
				case  22: return shape_ptr(new basic_cylinder<Ch, Tr>(o, w, h, entag));
				case  84: return shape_ptr(new basic_bevel<Ch, Tr>(o, w, h, entag));
				case 202: return shape_ptr(new basic_rectangle<Ch, Tr>(o, w, h, entag));
				default:
					break;
				}
				return shape_ptr();
			}
		}
	}
}

#endif // FAML_PDF_CONTENTS_MSSHAPE_H
