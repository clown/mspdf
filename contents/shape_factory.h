#ifndef FAML_PDF_CONTENTS_SHAPE_FACTORY_H
#define FAML_PDF_CONTENTS_SHAPE_FACTORY_H

#include "shape.h"
#include "polygon.h"
#include "circle.h"
#include "arrow.h"
#include "bracket.h"
#include "cube.h"
#include "bevel.h"
#include "cylinder.h"
#include "parallelogram.h"
#include "trapezoid.h"
#include "diamond.h"
#include "octagon.h"
#include "round_rect.h"
#include "snip1_rect.h"
#include "snip2_same_rect.h"
#include "snip2_diag_rect.h"
#include "snip_round_rect.h"
#include "round1_rect.h"
#include "round2_same_rect.h"
#include "round2_diag_rect.h"
#include "plus.h"
#include "folded_corner.h"
#include "donut.h"
#include "no_smoking.h"
#include "block_arc.h"
#include "heart.h"
#include "lightning_bolt.h"
#include "sun.h"
#include "moon.h"
#include "cloud.h"
#include "arc.h"
#include "bracket_pair.h"
#include "brace_pair.h"
#include "plaque.h"
#include "bent_arrow.h"
#include "uturn_arrow.h"
#include "left_up_arrow.h"
#include "bent_up_arrow.h"
#include "curved_right_arrow.h"
#include "curved_left_arrow.h"
#include "curved_up_arrow.h"
#include "curved_down_arrow.h"
#include "striped_right_arrow.h"
#include "home_plate.h"
#include "chevron.h"
#include "right_arrow_callout.h"
#include "down_arrow_callout.h"
#include "left_arrow_callout.h"
#include "up_arrow_callout.h"
#include "left_right_arrow_callout.h"
#include "up_down_arrow_callout.h"
#include "quad_arrow_callout.h"
#include "flow_chart_process.h"
#include "flow_chart_alternate_process.h"
#include "flow_chart_decision.h"
#include "flow_chart_input_output.h"
#include "flow_chart_predefined_process.h"
#include "flow_chart_internal_storage.h"
#include "flow_chart_document.h"
#include "flow_chart_multidocument.h"
#include "flow_chart_terminator.h"
#include "flow_chart_preparation.h"
#include "flow_chart_manual_input.h"
#include "flow_chart_manual_operation.h"
#include "flow_chart_connector.h"
#include "flow_chart_offpage_connector.h"
#include "flow_chart_punched_card.h"
#include "flow_chart_punched_tape.h"
#include "flow_chart_summing_junction.h"
#include "flow_chart_or.h"
#include "flow_chart_collate.h"
#include "flow_chart_sort.h"
#include "flow_chart_extract.h"
#include "flow_chart_merge.h"
#include "flow_chart_online_storage.h"
#include "flow_chart_delay.h"
#include "flow_chart_magnetic_tape.h"
#include "flow_chart_magnetic_disk.h"
#include "flow_chart_magnetic_drum.h"
#include "flow_chart_display.h"
#include "heptagon.h"
#include "decagon.h"
#include "dodecagon.h"
#include "pie.h"
#include "chord.h"
#include "teardrop.h"
#include "frame.h"
#include "half_frame.h"
#include "diag_stripe.h"
#include "corner.h"
#include "star4.h"
#include "star5.h"
#include "star6.h"
#include "star7.h"
#include "star8.h"
#include "star10.h"
#include "star12.h"
#include "star16.h"
#include "star24.h"
#include "star32.h"
#include "math_plus.h"
#include "math_minus.h"
#include "math_multiply.h"
#include "math_divide.h"
#include "math_equal.h"
#include "math_not_equal.h"
#include "wedge_rect_callout.h"
#include "wedge_round_rect_callout.h"
#include "border_callout1.h"
#include "border_callout2.h"
#include "border_callout3.h"
#include "accent_callout1.h"
#include "accent_callout2.h"
#include "accent_callout3.h"
#include "callout1.h"
#include "callout2.h"
#include "callout3.h"
#include "accent_border_callout1.h"
#include "accent_border_callout2.h"
#include "accent_border_callout3.h"
#include "irregular_seal1.h"
#include "irregular_seal2.h"
#include "action_button_blank.h"
#include "action_button_back_previous.h"
#include "action_button_forward_next.h"
#include "action_button_beginning.h"
#include "action_button_end.h"
#include "line.h"
#include "wedge_ellipse_callout.h"
#include "ribbon2.h"
#include "ribbon.h"
#include "ellipse_ribbon2.h"
#include "ellipse_ribbon.h"

namespace faml {
	namespace pdf {
		namespace contents {
			/* ------------------------------------------------------------- */
			//  makeshape
			/* ------------------------------------------------------------- */
			template <class Ch, class Tr>
			inline clx::shared_ptr<basic_shape<Ch, Tr> > makeshape(const std::basic_string<Ch, Tr>& name,
				const coordinate& o, double w, double h, bool entag = true) {
				typedef clx::shared_ptr<basic_shape<Ch, Tr> > shape_ptr;
				if (name == LITERAL("line")) return shape_ptr(new basic_line<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("straightConnector1")) return shape_ptr(new basic_line<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("ellipse")) return shape_ptr(new basic_ellipse<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("rect")) return shape_ptr(new basic_rectangle<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("roundRect")) return shape_ptr(new basic_round_rect<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("snip1Rect")) return shape_ptr(new basic_snip1_rect<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("snip2SameRect")) return shape_ptr(new basic_snip2_same_rect<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("snip2DiagRect")) return shape_ptr(new basic_snip2_diag_rect<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("snipRoundRect")) return shape_ptr(new basic_snip_round_rect<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("round1Rect")) return shape_ptr(new basic_round1_rect<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("round2SameRect")) return shape_ptr(new basic_round2_same_rect<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("round2DiagRect")) return shape_ptr(new basic_round2_diag_rect<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("triangle")) return shape_ptr(new basic_triangle<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("rtTriangle")) return shape_ptr(new basic_right_triangle<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("can")) return shape_ptr(new basic_cylinder<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("parallelogram")) return shape_ptr(new basic_parallelogram<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("pentagon")) return shape_ptr(new basic_pentagon<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("hexagon")) return shape_ptr(new basic_hexagon<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("trapezoid")) return shape_ptr(new basic_trapezoid<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("diamond")) return shape_ptr(new basic_diamond<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("heptagon")) return shape_ptr(new basic_heptagon<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("octagon")) return shape_ptr(new basic_octagon<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("decagon")) return shape_ptr(new basic_decagon<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("dodecagon")) return shape_ptr(new basic_dodecagon<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("frame")) return shape_ptr(new basic_frame<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("halfFrame")) return shape_ptr(new basic_half_frame<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("corner")) return shape_ptr(new basic_corner<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("diagStripe")) return shape_ptr(new basic_diag_stripe<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("pie")) return shape_ptr(new basic_pie<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("chord")) return shape_ptr(new basic_chord<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("teardrop")) return shape_ptr(new basic_teardrop<Ch, Tr>(o, w, h, entag)); // under construction
				if (name == LITERAL("plus")) return shape_ptr(new basic_plus<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("cube")) return shape_ptr(new basic_cube<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("bevel")) return shape_ptr(new basic_bevel<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("foldedCorner")) return shape_ptr(new basic_folded_corner<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("donut")) return shape_ptr(new basic_donut<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("noSmoking")) return shape_ptr(new basic_no_smoking<Ch, Tr>(o, w, h, entag)); // under construction
				if (name == LITERAL("blockArc")) return shape_ptr(new basic_block_arc<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("heart")) return shape_ptr(new basic_heart<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("lightningBolt")) return shape_ptr(new basic_lightning_bolt<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("sun")) return shape_ptr(new basic_sun<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("moon")) return shape_ptr(new basic_moon<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("cloud")) return shape_ptr(new basic_cloud<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("arc")) return shape_ptr(new basic_arc<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("bracketPair")) return shape_ptr(new basic_bracket_pair<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("bracePair")) return shape_ptr(new basic_brace_pair<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("plaque")) return shape_ptr(new basic_plaque<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("leftBracket")) return shape_ptr(new basic_left_bracket<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("rightBracket")) return shape_ptr(new basic_right_bracket<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("leftBrace")) return shape_ptr(new basic_left_brace<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("rightBrace")) return shape_ptr(new basic_right_brace<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("rightArrow")) return shape_ptr(new basic_right_arrow<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("leftArrow")) return shape_ptr(new basic_left_arrow<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("upArrow")) return shape_ptr(new basic_up_arrow<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("downArrow")) return shape_ptr(new basic_down_arrow<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("leftRightArrow")) return shape_ptr(new basic_left_right_arrow<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("upDownArrow")) return shape_ptr(new basic_up_down_arrow<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("quadArrow")) return shape_ptr(new basic_quad_arrow<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("leftRightUpArrow")) return shape_ptr(new basic_left_right_up_arrow<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("bentArrow")) return shape_ptr(new basic_bent_arrow<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("uturnArrow")) return shape_ptr(new basic_uturn_arrow<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("leftUpArrow")) return shape_ptr(new basic_left_up_arrow<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("bentUpArrow")) return shape_ptr(new basic_bent_up_arrow<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("curvedRightArrow")) return shape_ptr(new basic_curved_right_arrow<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("curvedLeftArrow")) return shape_ptr(new basic_curved_left_arrow<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("curvedUpArrow")) return shape_ptr(new basic_curved_up_arrow<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("curvedDownArrow")) return shape_ptr(new basic_curved_down_arrow<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("stripedRightArrow")) return shape_ptr(new basic_striped_right_arrow<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("notchedRightArrow")) return shape_ptr(new basic_notched_right_arrow<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("homePlate")) return shape_ptr(new basic_home_plate<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("chevron")) return shape_ptr(new basic_chevron<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("rightArrowCallout")) return shape_ptr(new basic_right_arrow_callout<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("downArrowCallout")) return shape_ptr(new basic_down_arrow_callout<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("leftArrowCallout")) return shape_ptr(new basic_left_arrow_callout<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("upArrowCallout")) return shape_ptr(new basic_up_arrow_callout<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("leftRightArrowCallout")) return shape_ptr(new basic_left_right_arrow_callout<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("upDownArrowCallout")) return shape_ptr(new basic_up_down_arrow_callout<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("quadArrowCallout")) return shape_ptr(new basic_quad_arrow_callout<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("mathPlus")) return shape_ptr(new basic_math_plus<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("mathMinus")) return shape_ptr(new basic_math_minus<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("mathMultiply")) return shape_ptr(new basic_math_multiply<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("mathDivide")) return shape_ptr(new basic_math_divide<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("mathEqual")) return shape_ptr(new basic_math_equal<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("mathNotEqual")) return shape_ptr(new basic_math_not_equal<Ch, Tr>(o, w, h, entag)); // under construction
				if (name == LITERAL("flowChartProcess")) return shape_ptr(new basic_flow_chart_process<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("flowChartAlternateProcess")) return shape_ptr(new basic_flow_chart_alternate_process<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("flowChartDecision")) return shape_ptr(new basic_flow_chart_decision<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("flowChartInputOutput")) return shape_ptr(new basic_flow_chart_input_output<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("flowChartPredefinedProcess")) return shape_ptr(new basic_flow_chart_predefined_process<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("flowChartInternalStorage")) return shape_ptr(new basic_flow_chart_internal_storage<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("flowChartDocument")) return shape_ptr(new basic_flow_chart_document<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("flowChartMultidocument")) return shape_ptr(new basic_flow_chart_multidocument<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("flowChartTerminator")) return shape_ptr(new basic_flow_chart_terminator<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("flowChartPreparation")) return shape_ptr(new basic_flow_chart_preparation<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("flowChartManualInput")) return shape_ptr(new basic_flow_chart_manual_input<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("flowChartManualOperation")) return shape_ptr(new basic_flow_chart_manual_operation<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("flowChartConnector")) return shape_ptr(new basic_flow_chart_connector<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("flowChartOffpageConnector")) return shape_ptr(new basic_flow_chart_offpage_connector<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("flowChartPunchedCard")) return shape_ptr(new basic_flow_chart_punched_card<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("flowChartPunchedTape")) return shape_ptr(new basic_flow_chart_punched_tape<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("flowChartSummingJunction")) return shape_ptr(new basic_flow_chart_summing_junction<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("flowChartOr")) return shape_ptr(new basic_flow_chart_or<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("flowChartCollate")) return shape_ptr(new basic_flow_chart_collate<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("flowChartSort")) return shape_ptr(new basic_flow_chart_sort<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("flowChartExtract")) return shape_ptr(new basic_flow_chart_extract<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("flowChartMerge")) return shape_ptr(new basic_flow_chart_merge<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("flowChartOnlineStorage")) return shape_ptr(new basic_flow_chart_online_storage<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("flowChartDelay")) return shape_ptr(new basic_flow_chart_delay<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("flowChartMagneticTape")) return shape_ptr(new basic_flow_chart_magnetic_tape<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("flowChartMagneticDisk")) return shape_ptr(new basic_flow_chart_magnetic_disk<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("flowChartMagneticDrum")) return shape_ptr(new basic_flow_chart_magnetic_drum<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("flowChartDisplay")) return shape_ptr(new basic_flow_chart_display<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("irregularSeal1")) return shape_ptr(new basic_irregular_seal1<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("irregularSeal2")) return shape_ptr(new basic_irregular_seal2<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("star4")) return shape_ptr(new basic_star4<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("star5")) return shape_ptr(new basic_star5<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("star6")) return shape_ptr(new basic_star6<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("star7")) return shape_ptr(new basic_star7<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("star8")) return shape_ptr(new basic_star8<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("star10")) return shape_ptr(new basic_star10<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("star12")) return shape_ptr(new basic_star12<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("star16")) return shape_ptr(new basic_star16<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("star24")) return shape_ptr(new basic_star24<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("star32")) return shape_ptr(new basic_star32<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("ribbon2")) return shape_ptr(new basic_ribbon2<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("ribbon")) return shape_ptr(new basic_ribbon<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("ellipseRibbon2")) return shape_ptr(new basic_ellipse_ribbon2<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("ellipseRibbon")) return shape_ptr(new basic_ellipse_ribbon<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("wedgeRectCallout")) return shape_ptr(new basic_wedge_rect_callout<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("wedgeRoundRectCallout")) return shape_ptr(new basic_wedge_round_rect_callout<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("wedgeEllipseCallout")) return shape_ptr(new basic_wedge_ellipse_callout<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("borderCallout1")) return shape_ptr(new basic_border_callout1<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("borderCallout2")) return shape_ptr(new basic_border_callout2<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("borderCallout3")) return shape_ptr(new basic_border_callout3<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("accentCallout1")) return shape_ptr(new basic_accent_callout1<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("accentCallout2")) return shape_ptr(new basic_accent_callout2<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("accentCallout3")) return shape_ptr(new basic_accent_callout3<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("callout1")) return shape_ptr(new basic_callout1<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("callout2")) return shape_ptr(new basic_callout2<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("callout3")) return shape_ptr(new basic_callout3<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("accentBorderCallout1")) return shape_ptr(new basic_accent_border_callout1<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("accentBorderCallout2")) return shape_ptr(new basic_accent_border_callout2<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("accentBorderCallout3")) return shape_ptr(new basic_accent_border_callout3<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("actionButtonBlank")) return shape_ptr(new basic_action_button_blank<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("actionButtonBackPrevious")) return shape_ptr(new basic_action_button_back_previous<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("actionButtonForwardNext")) return shape_ptr(new basic_action_button_forward_next<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("actionButtonBeginning")) return shape_ptr(new basic_action_button_beginning<Ch, Tr>(o, w, h, entag));
				if (name == LITERAL("actionButtonEnd")) return shape_ptr(new basic_action_button_end<Ch, Tr>(o, w, h, entag));
				return shape_ptr();
			}
			
			template <class CharT>
			clx::shared_ptr<basic_shape<CharT> > makeshape(const CharT* name,
				const coordinate& o, double w, double h, bool entag = true) {
				std::basic_string<CharT> tmp(name);
				return makeshape(tmp, o, w, h, entag);
			}
		}
	}
}

#endif // FAML_PDF_CONTENTS_SHAPE_FACTORY_H
