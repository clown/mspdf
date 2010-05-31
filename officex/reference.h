#ifndef FAML_OFFICEX_REFERENCE_H
#define FAML_OFFICEX_REFERENCE_H

#include <stdexcept>
#include <string>
#include <vector>
#include "rapidxml/rapidxml.hpp"
#include "clx/utility.h"
#include "clx/literal.h"

namespace faml {
	namespace officex {
		template <class Ch, class Tr, class MapT>
		inline bool read_reference(std::basic_istream<Ch, Tr>& in, MapT& dest) {
			typedef std::basic_string<Ch, Tr> string_type;
			typedef rapidxml::xml_node<Ch>* node_ptr;
			typedef rapidxml::xml_attribute<Ch>* attr_ptr;
			
			std::vector<Ch> s;
			clx::read(in, s);
			s.push_back(0);
			
			//rapidxml::xml_document<Ch> doc; // <- ??? compile error!!
			rapidxml::xml_document<char> doc;
			doc.parse<0>(reinterpret_cast<Ch*>(&s.at(0)));
			
			node_ptr root = doc.first_node(LITERAL("Relationships"));
			if (!root) throw std::runtime_error("cannot find <Relationships> tag");
			for (node_ptr child = root->first_node(); child; child = child->next_sibling()) {
				if (string_type(child->name()) != LITERAL("Relationship")) continue;
				attr_ptr attr = child->first_attribute(LITERAL("Id"));
				if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find <Relationship Id>");
				string_type key(attr->value());
				
				attr = child->first_attribute(LITERAL("Target"));
				if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find <Relationship Target>");
				string_type val(attr->value());
				val.erase(0, val.find_last_of(LITERAL('/')) + 1);
				dest[key] = val;
			}
			
			return true;
		}
	}
}

#endif // FAML_OFFICEX_REFERENCE_H
