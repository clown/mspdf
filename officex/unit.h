#ifndef FAML_OFFICEX_UNIT_H
#define FAML_OFFICEX_UNIT_H

#include "clx/unit.h"

namespace faml {
	namespace officex {
		typedef clx::unit<72, 1>    inch;
		typedef clx::unit<1, 12700> emu;
		typedef clx::unit<1, 60000> deg60k;
	}
}

#endif // FAML_OFFICEX_UNIT_H
