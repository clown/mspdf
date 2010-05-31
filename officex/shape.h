/* ------------------------------------------------------------------------- */
/*
 *  officex/shape.h
 *
 *  Copyright (c) 2009, Four and More, Inc. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    - Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    - Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    - No names of its contributors may be used to endorse or promote
 *      products derived from this software without specific prior written
 *      permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  Last-modified: Fri 05 Jun 2009 15:13:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_OFFICEX_SHAPE_H
#define FAML_OFFICEX_SHAPE_H

#include <cassert>
#include <stdexcept>
#include <string>
#include "rapidxml/rapidxml.hpp"
#include "clx/lexical_cast.h"
#include "clx/literal.h"
#include "clx/utility.h"
#include "../coordinate.h"
#include "../shape_effect.h"
#include "unit.h"
#include "color.h"
#include "theme.h"
#include "txbox.h"
#include "custom_shape.h"

namespace faml {
	namespace officex {
		using faml::pdf::coordinate;
		
		/* ----------------------------------------------------------------- */
		//  basic_shape
		/* ----------------------------------------------------------------- */
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class basic_shape {
		public:
			typedef size_t size_type;
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef basic_shape_path<CharT, Traits> path_type;
			typedef basic_shape_path_container<CharT, Traits> path_subcontainer;
			typedef std::vector<path_subcontainer> path_container;
			typedef basic_txbox<CharT, Traits> txbox;
			typedef basic_theme<CharT, Traits> theme_type;
			typedef std::map<size_type, int> adjust_map;
			typedef faml::basic_shape_effect<CharT, Traits> effect_type;
			typedef std::map<string_type, effect_type> effect_map;
			typedef std::map<string_type, string_type> color_map;
			
			explicit basic_shape(const color_map& clr = color_map()) :
				origin_(coordinate(0.0, 0.0)),
				name_(), type_(0), paths_(), texts_(),
				width_(0.0), height_(0.0),
				angle_(0.0), flip_(0), weight_(0.75),
				border_(-1), border_type_(0), bg_(), bg_type_(0),
				alpha_(0.0), dir_(0.0), opt_(0), effects_(),
				theme_(NULL), style_(), clrmap_(clr) {}
			
			template <class XMLNode>
			basic_shape(XMLNode* root, const string_type& ns, const color_map& clr = color_map()) :
				origin_(coordinate(0.0, 0.0)),
				name_(), type_(0), texts_(),
				width_(0.0), height_(0.0),
				angle_(0.0), flip_(0), weight_(0.75),
				border_(-1), border_type_(0), bg_(),
				alpha_(0.0), dir_(0.0), opt_(0), effects_(),
				theme_(NULL), style_(), clrmap_(clr) {
				this->read(root, ns);
			}
			
			virtual ~basic_shape() throw() {}
			
			template <class XMLNode>
			basic_shape& read(XMLNode* root, const string_type& ns) {
				if (!root) {
					clx::logger::warn(WARNF("cannot find shape's root tag"));
					return *this;
				}
				
				// 1. type.
				std::basic_stringstream<CharT, Traits> ss;
				ss << ns << LITERAL(":nvSpPr");
				node_ptr pos = root->first_node(ss.str().c_str());
				if (pos && pos->first_node()) this->xread_type(pos, ns);
				
				// 2. style.
				ss.str(LITERAL(""));
				ss << ns << LITERAL(":style");
				pos = root->first_node(ss.str().c_str());
				if (pos && pos->first_node()) this->xread_style(pos);
				
				// 3. shape data.
				ss.str(LITERAL(""));
				ss << ns << LITERAL(":spPr");
				pos = root->first_node(ss.str().c_str());
				if (pos && pos->first_node()) this->xread_shape(pos);
				
				// 4. text box.
				ss.str(LITERAL(""));
				ss << ns << LITERAL(":txBody");
				if (theme_) texts_.theme(theme_);
				if (!style_.empty()) texts_.style(style_);
				texts_.clrmap(clrmap_);
				pos = root->first_node(ss.str().c_str());
				if (pos && pos->first_node()) texts_.read(pos);
				
				// 5. Image data.
				ss.str(LITERAL(""));
				ss << ns << LITERAL(":blipFill");
				pos = root->first_node(ss.str().c_str());
				if (pos && pos->first_node()) this->xread_reference(pos);
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			const coordinate& origin() const { return origin_; }
			const string_type& name() const { return name_; }
			size_type type() const { return type_; }
			const path_container& paths() const { return paths_; }
			path_container& paths() { return paths_; }
			const txbox& texts() const { return texts_; }
			txbox& texts() { return texts_; }
			double width() const { return width_; }
			double height() const { return height_; }
			double angle() const { return angle_; }
			size_type flip() const { return flip_; }
			double weight() const { return weight_; }
			size_type option() const { return opt_; }
			size_type border() const { return border_; }
			size_type border_type() const { return border_type_; }
			size_type bg_type() const { return bg_type_; }
			
			const std::vector<size_type>& backgrounds() const { return bg_; }
			std::vector<size_type>& backgrounds() { return bg_; }
			size_type background(size_type pos = 0) const {
				return (pos < bg_.size()) ? bg_.at(pos) : size_type(-1);
			}
			const double alpha() const { return alpha_; }
			double direction() const { return dir_; }
			
			const adjust_map& adjusts() const { return adjust_; }
			adjust_map& adjusts() { return adjust_; }
			const string_type& reference() const { return reference_; }
			const effect_map& effects() const { return effects_; }
			effect_map& effects() { return effects_; }
			
			/* ------------------------------------------------------------- */
			//  Access methods (set).
			/* ------------------------------------------------------------- */
			void origin(const coordinate& cp) { origin_ = cp; }
			void name(const string_type& cp) { name_ = cp; }
			void type(size_type cp) { type_ = cp; }
			void width(double cp) { width_ = cp; }
			void height(double cp) { height_ = cp; }
			void angle(double cp) { angle_ = cp; }
			void weight(double cp) { weight_ = cp; }
			void border(size_type cp) { border_ = cp; }
			void border_type(size_type cp) { border_type_ = cp; }
			void background(size_type cp) { bg_.push_back(cp); }
			void background(size_type cp, size_type pos) { bg_.at(pos) = cp; }
			void direction(double cp) { dir_ = cp; }
			void reference(const string_type& cp) { reference_ = cp; }
			void theme(const theme_type* cp) { theme_ = cp; }
			void alpha(const string_type& cp) { 
				std::string tmp(cp);
				if (tmp.find("%") != std::string::npos){
					tmp = tmp.erase(tmp.length() - 1);
					alpha_ = clx::lexical_cast<size_t>(tmp) / 100.0;
				}
				else{
					alpha_ = clx::lexical_cast<size_t>(tmp) / 1000.0 / 100.0;
				}
			}
			
			void effect(const string_type& name, const effect_type& cp) { effects_[name] = cp; }
			
			template <class Container>
			void style(const Container& cp) {
				style_.assign(cp.begin(), cp.end());
			}
			
		private:
			typedef basic_txstyle<CharT, Traits> style_type;
			typedef rapidxml::xml_node<CharT>* node_ptr;
			typedef rapidxml::xml_attribute<CharT>* attr_ptr;
			
			coordinate origin_;
			string_type name_;
			size_type type_;
			path_container paths_;
			txbox texts_;
			double width_;
			double height_;
			double angle_;
			size_type flip_;
			double weight_;
			size_type border_;
			size_type border_type_;
			std::vector<size_type> bg_;
			size_type bg_type_;
			double alpha_;
			double dir_;
			size_type font_;
			size_type opt_;
			adjust_map adjust_;
			string_type reference_;
			effect_map effects_;
			
			// optional data.
			const theme_type* theme_;
			std::vector<style_type> style_;
			color_map clrmap_;
			
			/* ------------------------------------------------------------- */
			//  xread_type
			/* ------------------------------------------------------------- */
			basic_shape& xread_type(node_ptr root, const string_type& ns) {
				assert(!root);
				
				std::basic_stringstream<CharT, Traits> ss;
				ss << ns << LITERAL(":nvPr");
				node_ptr parent = root->first_node(ss.str().c_str());
				if (!parent || !parent->first_node()) return *this;
				
				ss.str(LITERAL(""));
				ss << ns << LITERAL(":ph");
				node_ptr pos = parent->first_node(ss.str().c_str());
				if (!pos) return *this;
				
				/*
				 * What is txstyle type is determined ?
				 * We currently determine the type based on the ph index.
				 */
				int idx = 0;
				attr_ptr attr = pos->first_attribute(LITERAL("idx"));
				if (attr && attr->value_size() > 0) {
					try {
						idx = clx::lexical_cast<int>(attr->value());
					}
					catch (clx::bad_lexical_cast&) {
						clx::logger::warn(DEBUGF("%s: failed to lexical_cast", attr->value()));
						idx = 0;
					}
				}
				
				if (idx == 0) type_ = 1;
				else if (idx == 1) type_ = 2;
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_shape
			/* ------------------------------------------------------------- */
			basic_shape& xread_shape(node_ptr root) {
				assert(!root);
				
				// 1. coorinate, width, and height.
				node_ptr pos = root->first_node(LITERAL("a:xfrm"));
				if (pos) this->xread_form(pos);
				else clx::logger::warn(WARNF("cannot find <a:xfrm> (coordinate/width/height)"));
				
				// 2. shape type
				pos = root->first_node(LITERAL("a:prstGeom"));
				if (pos) {
					attr_ptr attr = pos->first_attribute(LITERAL("prst"));
					if (attr && attr->value_size() > 0) {
						this->name(attr->value());
						node_ptr adj = pos->first_node(LITERAL("a:avLst"));
						if (adj) this->xread_adjust(adj);
					}
					else {
						clx::logger::warn(WARNF("cannot find shape type"));
						this->name(LITERAL("rect"));
					}
				}
				else {
					pos = root->first_node(LITERAL("a:custGeom"));
					if (pos) this->xread_custom_shape(pos);
				}
				
				// 3. fill
				if ((pos = root->first_node(LITERAL("a:gradFill")))) {
					if (!this->backgrounds().empty()) this->backgrounds().clear();
					this->xread_grad(pos);
				}
				else if ((pos = root->first_node(LITERAL("a:pattFill")))) {
					if (!this->backgrounds().empty()) this->backgrounds().clear();
					this->xread_pattern(pos);
				}
				else if ((pos = root->first_node(LITERAL("a:solidFill")))) {
					bg_type_ = 0x01;
					size_type rgb = size_type(-1);
					if (theme_) rgb = getrgb(pos, theme_->palette(), clrmap_);
					else rgb = getrgb(pos);
					if (rgb <= 0x00ffffff && !this->backgrounds().empty()) {
						this->background(rgb, 0);
					}
					else if (rgb <= 0x00ffffff) this->background(rgb);
					node_ptr p1 = pos->first_node(LITERAL("a:schemeClr"));
					if (!p1) p1 = pos->first_node(LITERAL("a:srgbClr"));
					if(p1){
						node_ptr p2 = p1->first_node(LITERAL("a:alpha"));
						if(p2){
							attr_ptr a1 = p2->first_attribute(LITERAL("val"));
							if (a1 && a1->value_size() > 0) {
								this->alpha(a1->value());
							}
						}
					}
				}
				else if ((pos = root->first_node(LITERAL("a:noFill")))) {
					bg_type_ = 0x00;
					this->background(-1);
				}
				
				// 4. line
				pos = root->first_node(LITERAL("a:ln"));
				if (pos) {
					node_ptr child = pos->first_node(LITERAL("a:solidFill"));
					if (child) {
						if (theme_) this->border(getrgb(child, theme_->palette(), clrmap_));
						else this->border(getrgb(child));
					}
					
					attr_ptr attr = pos->first_attribute(LITERAL("w"));
					if (attr && attr->value_size() > 0) {
						weight_ = emu(clx::lexical_cast<double>(attr->value()));
					}
					
					child = pos->first_node(LITERAL("a:prstDash"));
					if (child) {
						string_type val;
						attr = child->first_attribute(LITERAL("val"));
						if (attr && attr->value_size() > 0) val = string_type(attr->value());
						
						if (val == LITERAL("dash") || val == LITERAL("sysDash")) border_type_ = 0x002;
						else if (val == LITERAL("dot") || val == LITERAL("sysDot")) border_type_ = 0x003;
						else if (val == LITERAL("hair")) border_type_ = 0x004;
						else if (val == LITERAL("dashDot")) border_type_ = 0x005;
						else if (val == LITERAL("dashDotDot")) border_type_ = 0x006;
					}
					else border_type_ = 0x001;
					
					child = pos->first_node(LITERAL("a:headEnd"));
					if (child) opt_ |= 0x01;
					child = pos->first_node(LITERAL("a:tailEnd"));
					if (child) opt_ |= 0x02;
				}
				
				// 5. effect
				pos = root->first_node(LITERAL("a:effectLst"));
				if (pos) this->xread_effect(pos);
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_effect
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_shape& xread_effect(XMLNode* root) {
				if (!root) throw std::runtime_error("cannot find <a:effectLst>");
				
				node_ptr pos = root->first_node(LITERAL("a:outerShdw"));
				if (pos) {
					effect_type elem;
					
					double dis = -1.0;
					attr_ptr attr = pos->first_attribute(LITERAL("dist"));
					if (attr && attr->value_size() > 0) {
						dis = clx::lexical_cast<double>(attr->value()) / 21600.0;
					}
					if (dis < 0.0) dis = 2.0;
					elem.distance(dis);
					
					double dir = 0.0;
					attr = pos->first_attribute(LITERAL("dir"));
					if (attr && attr->value_size() > 0) {
						dir = deg60k(clx::lexical_cast<double>(attr->value()));
					}
					if (dir > 360.0) dir = 0.0;
					elem.direction(dir);
					
					elem.fill(color(0x808080));
					effects_[LITERAL("shadow")] = elem;
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_custom_shape
			/* ------------------------------------------------------------- */
			template <class XMLNode>
			basic_shape& xread_custom_shape(XMLNode* root) {
				if (!root) throw std::runtime_error("cannot find <a:custGeom>");
				
				this->name(string_type(LITERAL("custom")));
				this->xread_adjust(root->first_node(LITERAL("a:avLst")));
				node_ptr parent = root->first_node(LITERAL("a:pathLst"));
				if (!parent) return *this;
				
				for (node_ptr child = parent->first_node(); child; child = child->next_sibling()) {
					if (string_type(child->name()) != LITERAL("a:path")) continue;
					path_subcontainer elem;
					elem.read(child);
					paths_.push_back(elem);
				}
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_form
			/* ------------------------------------------------------------- */
			basic_shape& xread_form(node_ptr root) {
				if (!root) return *this;
				
				// 1. angle
				attr_ptr attr = root->first_attribute("rot");
				if (attr && attr->value_size() > 0) {
					double val = clx::lexical_cast<double>(attr->value());
					angle_ = deg60k(val);
					if (angle_ < 0.0) angle_ += 360.0;
				}
				
				attr = root->first_attribute(LITERAL("flipH"));
				if (attr) flip_ |= 0x01;
				attr = root->first_attribute(LITERAL("flipV"));
				if (attr) flip_ |= 0x02;
				
				// 2. origin
				double x = 0;
				double y = 0;
				node_ptr pos = root->first_node(LITERAL("a:off"));
				if (pos) {
					attr = pos->first_attribute(LITERAL("x"));
					if (attr && attr->value_size() > 0) x = clx::lexical_cast<double>(attr->value());
					attr = pos->first_attribute(LITERAL("y"));
					if (attr && attr->value_size() > 0) y = clx::lexical_cast<double>(attr->value());
					if (x > 0 || y > 0) this->origin(coordinate(x, y));
				}
				
				// 3. width and height
				double w = 0;
				double h = 0;
				pos = root->first_node(LITERAL("a:ext"));
				if (pos) {
					attr = pos->first_attribute(LITERAL("cx"));
					if (attr && attr->value_size() > 0) w = clx::lexical_cast<double>(attr->value());
					if (w > 0) this->width(w);
					attr = pos->first_attribute(LITERAL("cy"));
					if (attr && attr->value_size() > 0) h = clx::lexical_cast<double>(attr->value());
					if (h > 0) this->height(h);
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_style
			/* ------------------------------------------------------------- */
			basic_shape& xread_style(node_ptr root) {
				if (!root) throw std::runtime_error("cannot find <*:style> tag");
				
				node_ptr pos = root->first_node(LITERAL("a:fillRef"));
				if (pos) {
					if (theme_) this->background(getrgb(pos, theme_->palette(), clrmap_));
					else this->background(getrgb(pos));
				}
				
				pos = root->first_node(LITERAL("a:lnRef"));
				if (pos) {
					if (theme_) this->border(getrgb(pos, theme_->palette(), clrmap_));
					else this->border(getrgb(pos));
				}
				else this->border(this->background());
				
#if 0
				pos = root->first_node(LITERAL("a:fontRef"));
				if (pos) {
					if (theme_) this->font(getrgb(pos, theme_->palette(), clrmap_));
					else this->font(getrgb(pos));
					
					attr_ptr attr = pos->first_attribute(LITERAL("idx"));
					if (theme_ && attr && attr->value_size() > 0) {
						this->latin(theme_->latin(attr->value()));
						this->japan(theme_->japan(attr->value()));
					}
				}
#endif
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_adjust
			/* ------------------------------------------------------------- */
			basic_shape& xread_adjust(node_ptr root) {
				if (!root) return *this;
				
				for (node_ptr child = root->first_node(); child; child = child->next_sibling()) {
					if (string_type(child->name()) != LITERAL("a:gd")) continue;
					size_type idx = 0;
					try {
						attr_ptr attr = child->first_attribute(LITERAL("name"));
						if (attr && attr->value_size() > 0) {
							if (string_type(attr->value()).compare(0, 3, LITERAL("adj")) != 0) continue;
							else if (attr->value_size() > 3) {
								string_type tmp(&attr->value()[3]);
								idx = clx::lexical_cast<size_type>(tmp) - 1;
							}
						}
						
						attr = child->first_attribute(LITERAL("fmla"));
						if (attr && attr->value_size() > 4) {
							string_type tmp(&attr->value()[4]);
							int dest = clx::lexical_cast<int>(tmp);
							adjust_[idx] = dest;
						}
					}
					catch (clx::bad_lexical_cast&) {}
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_grad
			/* ------------------------------------------------------------- */
			basic_shape& xread_grad(node_ptr root) {
				if (!root) return *this;
				
				bg_type_ = 0x02;
				node_ptr pos = root->first_node(LITERAL("a:gsLst"));
				if (!pos) throw std::runtime_error("cannot find <a:gsLst>");
				for (node_ptr child = pos->first_node(); child; child = child->next_sibling()) {
					if (string_type(child->name()) != LITERAL("a:gs")) continue;
					size_type rgb;
					if (theme_) rgb = getrgb(child, theme_->palette(), clrmap_);
					else rgb = getrgb(child);
					if (rgb > 0x00ffffff) rgb = 0x00ffffff;
					this->background(rgb);
				}
				
				pos = root->first_node(LITERAL("a:lin"));
				if (pos) {
					attr_ptr attr = pos->first_attribute(LITERAL("ang"));
					if (attr && attr->value_size() > 0) {
						dir_ = deg60k(clx::lexical_cast<double>(attr->value()));
						if (dir_ > 360.0) dir_ = 0.0; // error?
					}
				}
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_pattern
			/* ------------------------------------------------------------- */
			basic_shape& xread_pattern(node_ptr root) {
				if (!root) return *this;
				
				bg_type_ = 0x03;
				node_ptr pos = root->first_node(LITERAL("a:fgClr"));
				size_type rgb(-1);
				if (theme_) rgb = getrgb(pos, theme_->palette(), clrmap_);
				else rgb = getrgb(pos);
				if (rgb > 0x00ffffff) rgb = 0x00ffffff;
				this->background(rgb);
				
				pos = root->first_node(LITERAL("a:bgClr"));
				rgb = size_type(-1);
				if (theme_) rgb = getrgb(pos, theme_->palette(), clrmap_);
				else rgb = getrgb(pos);
				if (rgb > 0x00ffffff) rgb = 0x00ffffff;
				this->background(rgb);
				
				return *this;
			}
			
			/* ------------------------------------------------------------- */
			//  xread_reference
			/* ------------------------------------------------------------- */
			basic_shape& xread_reference(node_ptr root) {
				if (!root) return *this;
				
				node_ptr pos = root->first_node(LITERAL("a:blip"));
				if (!pos) throw std::runtime_error("cannot find <a:blip> tag");
				attr_ptr attr = pos->first_attribute(LITERAL("r:embed"));
				if (!attr || attr->value_size() == 0) throw std::runtime_error("cannot find embed attribute");
				reference_ = string_type(attr->value());
				return *this;
			}
		};
	}
}

#endif
