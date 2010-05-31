#ifndef FAML_SHAPE_EFFECT_H
#define FAML_SHAPE_EFFECT_H

#include "color.h"

namespace faml {
	/* --------------------------------------------------------------------- */
	//  basic_shape_effect
	/* --------------------------------------------------------------------- */
	template <
		class CharT,
		class Traits = std::char_traits<CharT>
	>
	class basic_shape_effect {
	public:
		typedef size_t size_type;
		typedef CharT char_type;
		typedef std::basic_string<CharT, Traits> string_type;
		typedef pdf::color color_type;
		
		basic_shape_effect() :
			code_(), distance_(0.0), direction_(0.0), fills_() {}
		
		const string_type& code() const { return code_; }
		double distance() const { return distance_; }
		double direction() const { return direction_; }
		const std::vector<color_type>& fills() const { return fills_; }
		const color_type& fill(size_type pos = 0) const { return fills_.at(pos); }
		
		void code(const string_type& cp) { code_ = cp; }
		void distance(double cp) { distance_ = cp; }
		void direction(double cp) { direction_ = cp; }
		void fill(const color_type& cp) { fills_.push_back(cp); }
		void fill(const color_type& cp, size_type pos) { fills_.at(pos) = cp; }
		
	private:
		string_type code_;
		double distance_;
		double direction_; // degree unit.
		std::vector<color_type> fills_;
	};
}

#endif // FAML_SHAPE_EFFECT_H
