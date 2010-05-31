#ifndef FAML_OFFICEX_BULLET_H
#define FAML_OFFICEX_BULLET_H

#include <string>

namespace faml {
	namespace officex {
		inline std::string getbullet(const std::string& mark) {
			std::string dest = "■";
			size_t c0 = static_cast<size_t>(mark.at(0)) & 0xff;
			if (mark.size() == 1) {
				switch (c0) {
				case 0x6c:
					dest = "●";
					break;
				case 0x6d:
					dest = "○";
					break;
				case 0x6e:
					dest = "■";
					break;
				case 0x6f:
				case 0x70:
				case 0x71:
				case 0x72:
				case 0x77:
				case 0x78:
					dest = "□";
					break;
				case 0x73:
				case 0x74:
				case 0x75:
				case 0x76:
					dest = "◆";
					break;
				default:
					break;
				}
				return dest;
			}
			else if (mark.size() < 3) return dest;
			
			size_t c1 = static_cast<size_t>(mark.at(1)) & 0xff;
			size_t c2 = static_cast<size_t>(mark.at(2)) & 0xff;
			if (c0 != 0xef) return dest;
			
			if (c1 == 0x81 && c2 == 0xac) dest = "●";
			else if (c1 == 0x82 && c2 == 0xa1) dest = "○";
			else if (c1 == 0x82 && c2 == 0xa7) dest = "・";
			else if (c1 == 0x82 && c2 == 0x92) dest = "・";
			else if (c1 == 0x82 && c2 == 0x9f) dest = "・";
			else if (c1 == 0x81 && c2 == 0xae) dest = "■";
			else if (c1 == 0x81 && c2 == 0xaf) dest = "□";
			else if (c1 == 0x81 && c2 == 0xb5) dest = "◆";
			//else if (c1 == 0x82 && c2 == 0x9f) dest = "◇";
			else if (c1 == 0x83 && c2 == 0xbc) dest = "－"; // "✓"
			else if (c1 == 0x83 && c2 == 0x98) dest = "≫"; // "➢"
			
			return dest;
		}
	}
}

#endif // FAML_OFFICEX_BULLET_H
