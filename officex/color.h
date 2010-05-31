/* ------------------------------------------------------------------------- */
/*
 *  officex/color.h
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
#ifndef FAML_OFFICEX_COLOR_H
#define FAML_OFFICEX_COLOR_H

#include <stdexcept>
#include <string>
#include "rapidxml/rapidxml.hpp"
#include "clx/literal.h"
#include "clx/lexical_cast.h"
#include "clx/case_conv.h"

namespace faml {
	namespace officex {
		/* ----------------------------------------------------------------- */
		//  RGBtoHSV
		/* ----------------------------------------------------------------- */
		inline void RGBtoHSV(size_t rgb,double& dH,double& dS,double& dV)
		{
			double	dR;
			double	dG;
			double	dB;
			double	dMax;
			double	dMin;
			double	dDiff;
	
			size_t red = rgb >> 16;
			size_t green = (rgb >> 8) & 0xff;
			size_t blue = rgb & 0xff;
			
			dR = red	/ 255.0;
			dG = green	/ 255.0;
			dB = blue	/ 255.0;
	
			dMax = std::max(std::max(dR, dG), dB);
			dMin = std::min(std::min(dR, dG), dB);
	
			if(dMax == 0)
			{
				dS = 0.0;
				dV = 0.0;
				return;
			}
	
			dDiff = dMax - dMin;
			dV = dMax;
			dS = dDiff / dMax;
			//dS = dMax - dMin;
	
			if(dR == dMax)
				dH = 60 * (dG - dB) / dDiff;
			else if(dG == dMax)
				dH = 60 * (dB - dR) / dDiff + 120;
			else
				dH = 60 * (dR - dG) / dDiff + 240;
	
			if(dH < 0)
				dH = dH + 360;
	
			return;
		}

		/* ----------------------------------------------------------------- */
		//  HSVtoRGB
		/* ----------------------------------------------------------------- */
		inline size_t HSVtoRGB(double dH,double dS,double dV)
		{
			size_t red;
			size_t green;
			size_t blue;
			
			size_t dest = 0;
			
			if(dS == 0.0)
			{
				red		= (char)(dV * 255);
				green	= red;
				blue	= green;
				
				dest |= (static_cast<size_t>(red) << 16);
				dest |= (static_cast<size_t>(green) << 8);
				dest |= (static_cast<size_t>(blue));
			
				return dest;
			}
	
			double	f;
			double	p;
			double	q;
			double	t;
			int		nHi;
	
			nHi = (int)(dH / 60) % 6;
			if(nHi < 0)
				nHi *= -1;
	
			f = dH / 60 - nHi;
			p = dV * (1 - dS);
			q = dV * (1 - f * dS);
			t = dV * (1 - (1 - f) * dS);
	
			dV = dV * 255;
			p = p * 255;
			q = q * 255;
			t = t * 255;
	
			if(dV > 255)
				dV = 255;
			if(t > 255)
				t = 255;
			if(p > 255)
				p = 255;
			if(q > 255)
				q = 255;
	
			if(nHi == 0)
			{
				red		= (char)dV;
				green	= (char)t;
				blue	= (char)p;
				
				dest |= (static_cast<size_t>(red) << 16);
				dest |= (static_cast<size_t>(green) << 8);
				dest |= (static_cast<size_t>(blue));
				
				return dest;
			}
			if(nHi == 1)
			{
				red		= (char)q;
				green	= (char)dV;
				blue	= (char)p;
				
				dest |= (static_cast<size_t>(red) << 16);
				dest |= (static_cast<size_t>(green) << 8);
				dest |= (static_cast<size_t>(blue));
				
				return dest;
			}
			if(nHi == 2)
			{
				red		= (char)p;
				green	= (char)dV;
				blue	= (char)t;
				
				dest |= (static_cast<size_t>(red) << 16);
				dest |= (static_cast<size_t>(green) << 8);
				dest |= (static_cast<size_t>(blue));
				
				return dest;
			}
			if(nHi == 3)
			{
				red		= (char)p;
				green	= (char)q;
				blue	= (char)dV;
				
				dest |= (static_cast<size_t>(red) << 16);
				dest |= (static_cast<size_t>(green) << 8);
				dest |= (static_cast<size_t>(blue));
				
				return dest;
			}
			if(nHi == 4)
			{
				red		= (char)t;
				green	= (char)p;
				blue	= (char)dV;
				
				dest |= (static_cast<size_t>(red) << 16);
				dest |= (static_cast<size_t>(green) << 8);
				dest |= (static_cast<size_t>(blue));
				
				return dest;
			}
	
			//if(nHi == 5)
			//{
				red		= (char)dV;
				green	= (char)p;
				blue	= (char)q;
				
				dest |= (static_cast<size_t>(red) << 16);
				dest |= (static_cast<size_t>(green) << 8);
				dest |= (static_cast<size_t>(blue));
				
				return dest;
			//}
		}
		
		/* ----------------------------------------------------------------- */
		//  enred
		/* ----------------------------------------------------------------- */
		inline size_t enred(size_t rgb, double percent) {
			//size_t red = rgb >> 16;
			size_t green = (rgb >> 8) & 0xff;
			size_t blue = rgb & 0xff;
			
			size_t dest = 0;
			dest |= (static_cast<size_t>(255 * percent) << 16);
			dest |= (static_cast<size_t>(green) << 8);
			dest |= (static_cast<size_t>(blue));
			
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  enredMod
		/* ----------------------------------------------------------------- */
		inline size_t enredMod(size_t rgb, double percent) {
			size_t red = rgb >> 16;
			size_t green = (rgb >> 8) & 0xff;
			size_t blue = rgb & 0xff;
			
			size_t dest = 0;
			dest |= (static_cast<size_t>(red * percent) << 16);
			dest |= (static_cast<size_t>(green) << 8);
			dest |= (static_cast<size_t>(blue));
			
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  enredOff
		/* ----------------------------------------------------------------- */
		inline size_t enredOff(size_t rgb, double percent) {
			size_t red = rgb >> 16;
			size_t green = (rgb >> 8) & 0xff;
			size_t blue = rgb & 0xff;
			
			size_t dest = 0;
			dest |= (static_cast<size_t>(red + 255 * percent) << 16);
			dest |= (static_cast<size_t>(green) << 8);
			dest |= (static_cast<size_t>(blue));
			
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  engreen
		/* ----------------------------------------------------------------- */
		inline size_t engreen(size_t rgb, double percent) {
			size_t red = rgb >> 16;
			//size_t green = (rgb >> 8) & 0xff;
			size_t blue = rgb & 0xff;
			
			size_t dest = 0;
			dest |= (static_cast<size_t>(red) << 16);
			dest |= (static_cast<size_t>(255 * percent) << 8);
			dest |= (static_cast<size_t>(blue));
			
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  engreenMod
		/* ----------------------------------------------------------------- */
		inline size_t engreenMod(size_t rgb, double percent) {
			size_t red = rgb >> 16;
			size_t green = (rgb >> 8) & 0xff;
			size_t blue = rgb & 0xff;
			
			size_t dest = 0;
			dest |= (static_cast<size_t>(red) << 16);
			dest |= (static_cast<size_t>(green * percent) << 8);
			dest |= (static_cast<size_t>(blue));
			
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  engreenOff
		/* ----------------------------------------------------------------- */
		inline size_t engreenOff(size_t rgb, double percent) {
			size_t red = rgb >> 16;
			size_t green = (rgb >> 8) & 0xff;
			size_t blue = rgb & 0xff;
			
			size_t dest = 0;
			dest |= (static_cast<size_t>(red) << 16);
			dest |= (static_cast<size_t>(green + 255 * percent) << 8);
			dest |= (static_cast<size_t>(blue));
			
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  enblue
		/* ----------------------------------------------------------------- */
		inline size_t enblue(size_t rgb, double percent) {
			size_t red = rgb >> 16;
			size_t green = (rgb >> 8) & 0xff;
			//size_t blue = rgb & 0xff;
			
			size_t dest = 0;
			dest |= (static_cast<size_t>(red) << 16);
			dest |= (static_cast<size_t>(green) << 8);
			dest |= (static_cast<size_t>(255 * percent));
			
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  enblueMod
		/* ----------------------------------------------------------------- */
		inline size_t enblueMod(size_t rgb, double percent) {
			size_t red = rgb >> 16;
			size_t green = (rgb >> 8) & 0xff;
			size_t blue = rgb & 0xff;
			
			size_t dest = 0;
			dest |= (static_cast<size_t>(red) << 16);
			dest |= (static_cast<size_t>(green) << 8);
			dest |= (static_cast<size_t>(blue * percent));
			
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  enblueOff
		/* ----------------------------------------------------------------- */
		inline size_t enblueOff(size_t rgb, double percent) {
			size_t red = rgb >> 16;
			size_t green = (rgb >> 8) & 0xff;
			size_t blue = rgb & 0xff;
			
			size_t dest = 0;
			dest |= (static_cast<size_t>(red) << 16);
			dest |= (static_cast<size_t>(green) << 8);
			dest |= (static_cast<size_t>(blue + 255 * percent));
			
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  encomp
		/* ----------------------------------------------------------------- */
		inline size_t encomp(size_t rgb) {
			size_t red = rgb >> 16;
			size_t green = (rgb >> 8) & 0xff;
			size_t blue = rgb & 0xff;
			
			size_t max_rgb = std::max(std::max(red, green), blue);
			size_t min_rgb = std::min(std::min(red, green), blue);
			
			size_t dest = 0;
			dest |= (static_cast<size_t>(max_rgb + min_rgb - red) << 16);
			dest |= (static_cast<size_t>(max_rgb + min_rgb - green) << 8);
			dest |= (static_cast<size_t>(max_rgb + min_rgb - blue));
			
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  engray
		/* ----------------------------------------------------------------- */
		inline size_t engray(size_t rgb) {
			size_t red = rgb >> 16;
			size_t green = (rgb >> 8) & 0xff;
			size_t blue = rgb & 0xff;
			size_t gray = (red + green + blue) / 3.0;
			
			size_t dest = 0;
			dest |= (static_cast<size_t>(gray) << 16);
			dest |= (static_cast<size_t>(gray) << 8);
			dest |= (static_cast<size_t>(gray));
			
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  eninv
		/* ----------------------------------------------------------------- */
		inline size_t eninv(size_t rgb) {
			size_t red = rgb >> 16;
			size_t green = (rgb >> 8) & 0xff;
			size_t blue = rgb & 0xff;
			
			size_t dest = 0;
			dest |= (static_cast<size_t>(255 - red) << 16);
			dest |= (static_cast<size_t>(255 - green) << 8);
			dest |= (static_cast<size_t>(255 - blue));
			
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  endark
		/* ----------------------------------------------------------------- */
		inline size_t endark(size_t rgb, double percent) {
			size_t red = rgb >> 16;
			size_t green = (rgb >> 8) & 0xff;
			size_t blue = rgb & 0xff;
			
			size_t dest = 0;
			dest |= (static_cast<size_t>(red * percent) << 16);
			dest |= (static_cast<size_t>(green * percent) << 8);
			dest |= (static_cast<size_t>(blue * percent));
			
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  enhue
		/* ----------------------------------------------------------------- */
		inline size_t enhue(size_t rgb, size_t val) {
			double hue = 0.0;
			double lum = 0.0;
			double sat = 0.0;
			
			RGBtoHSV(rgb, hue, lum, sat);
			hue = val/ 60000.0;
			rgb = HSVtoRGB(hue, lum, sat);
			
			return rgb;
		}
		
		/* ----------------------------------------------------------------- */
		//  enhueMod
		/* ----------------------------------------------------------------- */
		inline size_t enhueMod(size_t rgb, double percent) {
			double hue = 0.0;
			double lum = 0.0;
			double sat = 0.0;
			
			RGBtoHSV(rgb, hue, lum, sat);
			hue = hue * percent;
			rgb = HSVtoRGB(hue, lum, sat);
			
			return rgb;
		}
		
		/* ----------------------------------------------------------------- */
		//  enhueOff
		/* ----------------------------------------------------------------- */
		inline size_t enhueOff(size_t rgb, size_t val) {
			double hue = 0.0;
			double lum = 0.0;
			double sat = 0.0;
			
			RGBtoHSV(rgb, hue, lum, sat);
			hue = hue + val/ 60000.0;
			rgb = HSVtoRGB(hue, lum, sat);
			
			return rgb;
		}
		
		/* ----------------------------------------------------------------- */
		//  enlum
		/* ----------------------------------------------------------------- */
		inline size_t enlum(size_t rgb, double percent) {
			double hue = 0.0;
			double lum = 0.0;
			double sat = 0.0;
			
			RGBtoHSV(rgb, hue, lum, sat);
			lum = percent;
			rgb = HSVtoRGB(hue, lum, sat);
			
			return rgb;
		}
		
		/* ----------------------------------------------------------------- */
		//  enlumMod
		/* ----------------------------------------------------------------- */
		inline size_t enlumMod(size_t rgb, double percent) {
			double hue = 0.0;
			double lum = 0.0;
			double sat = 0.0;
			
			RGBtoHSV(rgb, hue, lum, sat);
			lum = lum * percent;
			rgb = HSVtoRGB(hue, lum, sat);
			
			return rgb;
		}
		
		/* ----------------------------------------------------------------- */
		//  enlumOff
		/* ----------------------------------------------------------------- */
		inline size_t enlumOff(size_t rgb, double percent) {
			double hue = 0.0;
			double lum = 0.0;
			double sat = 0.0;
			
			RGBtoHSV(rgb, hue, lum, sat);
			lum = lum + percent;
			rgb = HSVtoRGB(hue, lum, sat);
			
			return rgb;
		}
		
		/* ----------------------------------------------------------------- */
		//  ensat
		/* ----------------------------------------------------------------- */
		inline size_t ensat(size_t rgb, double percent) {
			double hue = 0.0;
			double lum = 0.0;
			double sat = 0.0;
			
			RGBtoHSV(rgb, hue, lum, sat);
			sat = percent;
			rgb = HSVtoRGB(hue, lum, sat);
			
			return rgb;
		}
		
		/* ----------------------------------------------------------------- */
		//  ensatMod
		/* ----------------------------------------------------------------- */
		inline size_t ensatMod(size_t rgb, double percent) {
			double hue = 0.0;
			double lum = 0.0;
			double sat = 0.0;
			
			RGBtoHSV(rgb, hue, lum, sat);
			sat = sat * percent;
			rgb = HSVtoRGB(hue, lum, sat);
			
			return rgb;
		}
		
		/* ----------------------------------------------------------------- */
		//  ensatOff
		/* ----------------------------------------------------------------- */
		inline size_t ensatOff(size_t rgb, double percent) {
			double hue = 0.0;
			double lum = 0.0;
			double sat = 0.0;
			
			RGBtoHSV(rgb, hue, lum, sat);
			sat = sat + percent;
			rgb = HSVtoRGB(hue, lum, sat);
			
			return rgb;
		}
		
		/* ----------------------------------------------------------------- */
		//  entint
		/* ----------------------------------------------------------------- */
		inline size_t entint(size_t rgb, double percent) {
			size_t red = rgb >> 16;
			size_t green = (rgb >> 8) & 0xff;
			size_t blue = rgb & 0xff;
			
			size_t dest = 0;
			dest |= (static_cast<size_t>(red * percent + 255 * (1.0 - percent)) << 16);
			dest |= (static_cast<size_t>(green * percent + 255 * (1.0 - percent)) << 8);
			dest |= (static_cast<size_t>(blue * percent + 255 * (1.0 - percent)));
			
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  convrgb
		/* ----------------------------------------------------------------- */
		inline size_t convrgb(double red, double green, double blue) {
			size_t r = static_cast<size_t>(0xff * red);
			size_t g = static_cast<size_t>(0xff * green);
			size_t b = static_cast<size_t>(0xff * blue);
			
			size_t dest = 0;
			dest |= (r << 16);
			dest |= (g << 8);
			dest |= b;
			
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  indexedrgb
		/* ----------------------------------------------------------------- */
		inline size_t indexedrgb(size_t pos, size_t offset = 0) {
			static const size_t indexed_[56] = {
				0x000000, 0xffffff, 0xff0000, 0x00ff00, 0x0000ff,
				0xffff00, 0xff00ff, 0x00ffff, 0x800000, 0x008000,
				0x000080, 0x808000, 0x800080, 0x008080, 0xc0c0c0,
				0x808080, 0x9999ff, 0x993366, 0xffffcc, 0xccffff,
				0x660066, 0xff8080, 0x0066cc, 0xccccff, 0x000080,
				0xff00ff, 0xffff00, 0x00ffff, 0x800080, 0x800000,
				0x008080, 0x0000ff, 0x00ccff, 0xccffff, 0xccffcc,
				0xffff99, 0x99ccff, 0xff99cc, 0xcc99ff, 0xffcc99,
				0x3366ff, 0x33cccc, 0x99cc00, 0xffcc00, 0xff9900,
				0xff6600, 0x666699, 0x969696, 0x003366, 0x339966,
				0x003300, 0x333300, 0x993300, 0x993366, 0x333399,
				0x333333
			};
			
			if (pos - offset >= 0 && pos - offset < 56) return indexed_[pos - offset];
			return 0;
		}
		
		/* ----------------------------------------------------------------- */
		//  getrgb
		/* ----------------------------------------------------------------- */
		template <class Ch, class Tr>
		inline size_t getrgb(const std::basic_string<Ch, Tr>& name) {
			std::basic_string<Ch, Tr> s = clx::downcase_copy(name);
			if (s == LITERAL("yellow")) return 0xffff00;
			if (s == LITERAL("green")) return 0x00ff00;
			if (s == LITERAL("cyan")) return 0x00ffff;
			if (s == LITERAL("magenta")) return 0xff00ff;
			if (s == LITERAL("blue")) return 0x0000ff;
			if (s == LITERAL("red")) return 0xff0000;
			if (s == LITERAL("darkBlue")) return 0x000080;
			if (s == LITERAL("darkCyan")) return 0x008080;
			if (s == LITERAL("darkGreen")) return 0x008000;
			if (s == LITERAL("darkMagenta")) return 0x800080;
			if (s == LITERAL("darkRed")) return 0x800000;
			if (s == LITERAL("darkYellow")) return 0x808000;
			if (s == LITERAL("darkGray")) return 0x808080;
			if (s == LITERAL("lightGray")) return 0xc0c0c0;
			if (s == LITERAL("black")) return 0x000000;
			if (s == LITERAL("white")) return 0xffffff;
			return size_t(-1);
		}
		
		/* ----------------------------------------------------------------- */
		//  getrgb
		/* ----------------------------------------------------------------- */
		template <class CharT>
		inline size_t getrgb(rapidxml::xml_node<CharT>* root) {
			typedef rapidxml::xml_node<CharT>* node_ptr;
			typedef rapidxml::xml_attribute<CharT>* attr_ptr;
			
			if (!root) return size_t(-1);
			
			size_t rgb = size_t(-1);
			node_ptr pos = root->first_node(LITERAL("a:srgbClr"));
			if (pos) {
				attr_ptr attr = pos->first_attribute(LITERAL("val"));
				if (attr && attr->value_size() > 0) {
					rgb = clx::lexical_cast<size_t>(attr->value(), std::ios::hex);
				}
			}
			else if ((pos = root->first_node(LITERAL("a:srgbClr"))) != NULL) {
				double r = 0.0, g = 0.0, b = 0.0;
				attr_ptr attr = pos->first_attribute(LITERAL("r"));
				if (attr && attr->value_size() > 0) {
					r = clx::lexical_cast<size_t>(attr->value()) / 1000.0 / 100.0;
				}
				
				attr = pos->first_attribute(LITERAL("g"));
				if (attr && attr->value_size() > 0) {
					g = clx::lexical_cast<size_t>(attr->value()) / 1000.0 / 100.0;
				}
				
				attr = pos->first_attribute(LITERAL("b"));
				if (attr && attr->value_size() > 0) {
					b = clx::lexical_cast<size_t>(attr->value()) / 1000.0 / 100.0;
				}
				
				rgb = convrgb(r, g, b);
			}
			else if ((pos = root->first_node(LITERAL("a:sysClr"))) != NULL) {
				attr_ptr attr = pos->first_attribute(LITERAL("lastClr"));
				if (attr && attr->value_size() > 0) {
					rgb = clx::lexical_cast<size_t>(attr->value(), std::ios::hex);
				}
			}
			
			// optional change.
			node_ptr opt = pos->first_node(LITERAL("a:shade"));
			if (opt) {
				attr_ptr attr = opt->first_attribute(LITERAL("val"));
				if (attr && attr->value_size() > 0) {
					double percent = clx::lexical_cast<size_t>(attr->value()) / 1000.0 / 100.0;
					rgb = endark(rgb, percent);
				}
			}
			
			return rgb;
		}
		
		/* ----------------------------------------------------------------- */
		//  getrgb
		/* ----------------------------------------------------------------- */
		template <class Ch, class Tr, class Palette>
		inline size_t getrgb(const std::basic_string<Ch, Tr>& name, const Palette& v) {
			if (v.size() < 12) return size_t(-1);
			
			if (name == LITERAL("dk1") || name == LITERAL("tx1")) return v.at(0);
			if (name == LITERAL("lt1") || name == LITERAL("bg1")) return v.at(1);
			if (name == LITERAL("dk2") || name == LITERAL("tx2")) return v.at(2);
			if (name == LITERAL("lt2") || name == LITERAL("bg2")) return v.at(3);
			if (name == LITERAL("accent1")) return v.at(4);
			if (name == LITERAL("accent2")) return v.at(5);
			if (name == LITERAL("accent3")) return v.at(6);
			if (name == LITERAL("accent4")) return v.at(7);
			if (name == LITERAL("accent5")) return v.at(8);
			if (name == LITERAL("accent6")) return v.at(9);
			if (name == LITERAL("hlink")) return v.at(10);
			if (name == LITERAL("folHlink")) return v.at(11);
			
			return size_t(-1);
		}
		
		template <class CharT, class Palette>
		inline size_t getrgb(const CharT* name, const Palette& v) {
			std::basic_string<CharT> tmp(name);
			return getrgb(tmp, v);
		}
		
		/* ----------------------------------------------------------------- */
		//  getrgb
		/* ----------------------------------------------------------------- */
		template <class CharT, class Palette, class ClrMap>
		inline size_t getrgb(rapidxml::xml_node<CharT>* root, const Palette& v, const ClrMap& clr) {
			typedef rapidxml::xml_node<CharT>* node_ptr;
			typedef rapidxml::xml_attribute<CharT>* attr_ptr;
			
			if (!root) return size_t(-1);
			
			size_t rgb = size_t(-1);
			node_ptr pos = root->first_node(LITERAL("a:schemeClr"));
			if (pos) {
				attr_ptr attr = pos->first_attribute(LITERAL("val"));
				if (!attr || attr->value_size() > 0) {
					std::basic_string<CharT> name = (clr.find(attr->value()) != clr.end())
						? clr.find(attr->value())->second : attr->value();
					rgb = getrgb(name, v);
					clx::logger::debug(DEBUGF("color: %s -> %s (%06x)", attr->value(), name.c_str(), rgb));
				}
				
				// optional change.
				node_ptr opt;
				
				// red
				opt = pos->first_node(LITERAL("a:red"));
				if (opt) {
					attr_ptr attr = opt->first_attribute(LITERAL("val"));
					if (attr && attr->value_size() > 0) {
						double percent;
						std::string tmp(attr->value());
						if (tmp.find("%") != std::string::npos){
							tmp = tmp.erase(tmp.length() - 1);
							percent = clx::lexical_cast<size_t>(tmp) / 100.0;
						}
						else{
							percent = clx::lexical_cast<size_t>(attr->value()) / 1000.0 / 100.0;
						}
						rgb = enred(rgb, percent);
					}
				}
				
				// redMod
				opt = pos->first_node(LITERAL("a:redMod"));
				if (opt) {
					attr_ptr attr = opt->first_attribute(LITERAL("val"));
					if (attr && attr->value_size() > 0) {
						std::string tmp(attr->value());
						if (tmp.find("%") != std::string::npos){
							tmp = tmp.erase(tmp.length() - 1);
							double percent = clx::lexical_cast<size_t>(tmp) / 100.0;
							rgb = enredMod(rgb, percent);
						}
					}
				}
				
				// redOff
				opt = pos->first_node(LITERAL("a:redOff"));
				if (opt) {
					attr_ptr attr = opt->first_attribute(LITERAL("val"));
					if (attr && attr->value_size() > 0) {
						double percent;
						std::string tmp(attr->value());
						if (tmp.find("%") != std::string::npos){
							tmp = tmp.erase(tmp.length() - 1);
							percent = clx::lexical_cast<size_t>(tmp) / 100.0;
						}
						else{
							percent = clx::lexical_cast<size_t>(attr->value()) / 1000.0 / 100.0;
						}
						rgb = enredOff(rgb, percent);
					}
				}
				
				// green
				opt = pos->first_node(LITERAL("a:green"));
				if (opt) {
					attr_ptr attr = opt->first_attribute(LITERAL("val"));
					if (attr && attr->value_size() > 0) {
						double percent;
						std::string tmp(attr->value());
						if (tmp.find("%") != std::string::npos){
							tmp = tmp.erase(tmp.length() - 1);
							percent = clx::lexical_cast<size_t>(tmp) / 100.0;
						}
						else{
							percent = clx::lexical_cast<size_t>(attr->value()) / 1000.0 / 100.0;
						}
						rgb = engreen(rgb, percent);
					}
				}
				
				// greenMod
				opt = pos->first_node(LITERAL("a:greenMod"));
				if (opt) {
					attr_ptr attr = opt->first_attribute(LITERAL("val"));
					if (attr && attr->value_size() > 0) {
						std::string tmp(attr->value());
						if (tmp.find("%") != std::string::npos){
							tmp = tmp.erase(tmp.length() - 1);
							double percent = clx::lexical_cast<size_t>(tmp) / 100.0;
							rgb = engreenMod(rgb, percent);
						}
					}
				}
				
				// greenOff
				opt = pos->first_node(LITERAL("a:greenOff"));
				if (opt) {
					attr_ptr attr = opt->first_attribute(LITERAL("val"));
					if (attr && attr->value_size() > 0) {
						double percent;
						std::string tmp(attr->value());
						if (tmp.find("%") != std::string::npos){
							tmp = tmp.erase(tmp.length() - 1);
							percent = clx::lexical_cast<size_t>(tmp) / 100.0;
						}
						else{
							percent = clx::lexical_cast<size_t>(attr->value()) / 1000.0 / 100.0;
						}
						rgb = engreenOff(rgb, percent);
					}
				}
				
				// blue
				opt = pos->first_node(LITERAL("a:blue"));
				if (opt) {
					attr_ptr attr = opt->first_attribute(LITERAL("val"));
					if (attr && attr->value_size() > 0) {
						double percent;
						std::string tmp(attr->value());
						if (tmp.find("%") != std::string::npos){
							tmp = tmp.erase(tmp.length() - 1);
							percent = clx::lexical_cast<size_t>(tmp) / 100.0;
						}
						else{
							percent = clx::lexical_cast<size_t>(attr->value()) / 1000.0 / 100.0;
						}
						rgb = enblue(rgb, percent);
					}
				}
				
				// blueMod
				opt = pos->first_node(LITERAL("a:blueMod"));
				if (opt) {
					attr_ptr attr = opt->first_attribute(LITERAL("val"));
					if (attr && attr->value_size() > 0) {
						std::string tmp(attr->value());
						if (tmp.find("%") != std::string::npos){
							tmp = tmp.erase(tmp.length() - 1);
							double percent = clx::lexical_cast<size_t>(tmp) / 100.0;
							rgb = enblueMod(rgb, percent);
						}
					}
				}
				
				// blueOff
				opt = pos->first_node(LITERAL("a:blueOff"));
				if (opt) {
					attr_ptr attr = opt->first_attribute(LITERAL("val"));
					if (attr && attr->value_size() > 0) {
						double percent;
						std::string tmp(attr->value());
						if (tmp.find("%") != std::string::npos){
							tmp = tmp.erase(tmp.length() - 1);
							percent = clx::lexical_cast<size_t>(tmp) / 100.0;
						}
						else{
							percent = clx::lexical_cast<size_t>(attr->value()) / 1000.0 / 100.0;
						}
						rgb = enblueOff(rgb, percent);
					}
				}
				
				// comp
				opt = pos->first_node(LITERAL("a:comp"));
				if (opt) {
					rgb = encomp(rgb);
				}
				
				// gray
				opt = pos->first_node(LITERAL("a:gray"));
				if (opt) {
					rgb = engray(rgb);
				}
				
				// inv
				opt = pos->first_node(LITERAL("a:inv"));
				if (opt) {
					rgb = eninv(rgb);
				}
				
				// shade
				opt = pos->first_node(LITERAL("a:shade"));
				if (opt) {
					attr = opt->first_attribute(LITERAL("val"));
					if (attr && attr->value_size() > 0) {
						double percent;
						std::string tmp(attr->value());
						if (tmp.find("%") != std::string::npos){
							tmp = tmp.erase(tmp.length() - 1);
							percent = clx::lexical_cast<size_t>(tmp) / 100.0;
						}
						else{
							percent = clx::lexical_cast<size_t>(attr->value()) / 1000.0 / 100.0;
						}
						rgb = endark(rgb, percent);
					}
				}
				
				// hue
				opt = pos->first_node(LITERAL("a:hue"));
				if (opt) {
					attr = opt->first_attribute(LITERAL("val"));
					if (attr && attr->value_size() > 0) {
						rgb = enhue(rgb, clx::lexical_cast<size_t>(attr->value()));
					}
				}
				
				// hueMod
				opt = pos->first_node(LITERAL("a:hueMod"));
				if (opt) {
					attr_ptr attr = opt->first_attribute(LITERAL("val"));
					if (attr && attr->value_size() > 0) {
						std::string tmp(attr->value());
						if (tmp.find("%") != std::string::npos){
							tmp = tmp.erase(tmp.length() - 1);
							double percent = clx::lexical_cast<size_t>(tmp) / 100.0;
							rgb = enhueMod(rgb, percent);
						}
					}
				}
				
				// hueOff
				opt = pos->first_node(LITERAL("a:hueOff"));
				if (opt) {
					attr = opt->first_attribute(LITERAL("val"));
					if (attr && attr->value_size() > 0) {
						rgb = enhueOff(rgb, clx::lexical_cast<size_t>(attr->value()));
					}
				}
				
				// lum
				opt = pos->first_node(LITERAL("a:lum"));
				if (opt) {
					attr_ptr attr = opt->first_attribute(LITERAL("val"));
					if (attr && attr->value_size() > 0) {
						std::string tmp(attr->value());
						if (tmp.find("%") != std::string::npos){
							tmp = tmp.erase(tmp.length() - 1);
							double percent = clx::lexical_cast<size_t>(tmp) / 100.0;
							rgb = enlum(rgb, percent);
						}
					}
				}
				
				// lumMod
				opt = pos->first_node(LITERAL("a:lumMod"));
				if (opt) {
					attr_ptr attr = opt->first_attribute(LITERAL("val"));
					if (attr && attr->value_size() > 0) {
						std::string tmp(attr->value());
						if (tmp.find("%") != std::string::npos){
							tmp = tmp.erase(tmp.length() - 1);
							double percent = clx::lexical_cast<size_t>(tmp) / 100.0;
							rgb = enlumMod(rgb, percent);
						}
					}
				}
				
				// lumOff
				opt = pos->first_node(LITERAL("a:lumOff"));
				if (opt) {
					attr_ptr attr = opt->first_attribute(LITERAL("val"));
					if (attr && attr->value_size() > 0) {
						std::string tmp(attr->value());
						if (tmp.find("%") != std::string::npos){
							tmp = tmp.erase(tmp.length() - 1);
							double percent = clx::lexical_cast<size_t>(tmp) / 100.0;
							rgb = enlumOff(rgb, percent);
						}
					}
				}
				
				// sat
				opt = pos->first_node(LITERAL("a:sat"));
				if (opt) {
					attr_ptr attr = opt->first_attribute(LITERAL("val"));
					if (attr && attr->value_size() > 0) {
						std::string tmp(attr->value());
						if (tmp.find("%") != std::string::npos){
							tmp = tmp.erase(tmp.length() - 1);
							double percent = clx::lexical_cast<size_t>(tmp) / 100.0;
							rgb = ensat(rgb, percent);
						}
					}
				}
				
				// satMod
				opt = pos->first_node(LITERAL("a:satMod"));
				if (opt) {
					attr_ptr attr = opt->first_attribute(LITERAL("val"));
					if (attr && attr->value_size() > 0) {
						std::string tmp(attr->value());
						if (tmp.find("%") != std::string::npos){
							tmp = tmp.erase(tmp.length() - 1);
							double percent = clx::lexical_cast<size_t>(tmp) / 100.0;
							rgb = ensatMod(rgb, percent);
						}
					}
				}
				
				// satOff
				opt = pos->first_node(LITERAL("a:satOff"));
				if (opt) {
					attr_ptr attr = opt->first_attribute(LITERAL("val"));
					if (attr && attr->value_size() > 0) {
						std::string tmp(attr->value());
						if (tmp.find("%") != std::string::npos){
							tmp = tmp.erase(tmp.length() - 1);
							double percent = clx::lexical_cast<size_t>(tmp) / 100.0;
							rgb = ensatOff(rgb, percent);
						}
					}
				}
				
				// tint
				opt = pos->first_node(LITERAL("a:tint"));
				if (opt) {
					attr_ptr attr = opt->first_attribute(LITERAL("val"));
					if (attr && attr->value_size() > 0) {
						std::string tmp(attr->value());
						if (tmp.find("%") != std::string::npos){
							tmp = tmp.erase(tmp.length() - 1);
							double percent = clx::lexical_cast<size_t>(tmp) / 100.0;
							rgb = entint(rgb, percent);
						}
					}
				}
				
				return rgb;
			}
			else return getrgb(root);
		}
		
		/* ----------------------------------------------------------------- */
		//  getrgb
		/* ----------------------------------------------------------------- */
		template <class CharT, class Palette>
		inline size_t getrgb(rapidxml::xml_node<CharT>* root, const Palette& v) {
			std::map<std::basic_string<CharT>, std::basic_string<CharT> > dummy;
			return getrgb(root, v, dummy);
		}
	}
}

#endif // FAML_OFFICEX_UTILITY_H
