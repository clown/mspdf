/* ------------------------------------------------------------------------- */
/*
 *  font_factory.h
 *
 *  Copyright (c) 2009, clown. All rights reserved.
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
 *  Last-modified: Thu 17 Dec 2009 14:19:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLIFF_PDF_FONT_FACTORY_H
#define CLIFF_PDF_FONT_FACTORY_H

#include <iostream>
#include <istream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <unistd.h>    // process
#include <sys/types.h> // process
#include <sys/wait.h>  // process
#include <clx/lexical_cast.h>
#include <clx/salgorithm.h>
#include <clx/scanner.h>
#include <clx/shared_ptr.h>
#include "font.h"
#include "font_property.h"
#include "stripttc.h"

//namespace cliff {
namespace faml {
	namespace pdf {
		class font_factory {
		public:
			typedef size_t size_type;
			typedef char char_type;
			typedef std::string string_type;
			typedef std::istream istream_type;
			typedef basic_font<char> object_type;
			
			/* ------------------------------------------------------------- */
			//  add
			/* ------------------------------------------------------------- */
			void add(const string_type& path) {
				mapped_type elem;
				elem.path = path;
				
				string_type ext = path.substr(path.find_last_of("."));
				clx::downcase(ext);
				if (ext == ".ttc") this->add_ttc(path);
				else if (ext == ".ttf") elem.value = read_ttf(path);
				else if (ext == ".afm") elem.value = read_afm(path);
				else throw std::runtime_error("cannot support the filetype");
				
				if (!elem.value) throw std::runtime_error("failed to parse");
				v_[elem.value->name()] = elem;
			}
			
			/* ------------------------------------------------------------- */
			//  add
			/* ------------------------------------------------------------- */
			void add(const string_type& name, const string_type& path, size_type index = 0) {
				mapped_type elem;
				elem.path = path;
				elem.index = index;
				v_[name] = elem;
			}
			
			/* ------------------------------------------------------------- */
			//  add_alias
			/* ------------------------------------------------------------- */
			void add_alias(const std::pair<string_type, string_type>& x) {
				aliases_.insert(x);
			}
			
			template <class InIter>
			void add_alias(InIter first, InIter last) {
				while (first != last) {
					this->add_alias(*first);
					++first;
				}
			}
			
			object_type create(const string_type& name, int cset, int dir = 0x01) {
				string_type s = (aliases_.find(name) != aliases_.end()) ? aliases_[name] : name;
				if (v_.find(s) == v_.end()) {
					return object_type(name, cset, dir);
				}
				
				if (!v_[s].value) {
					string_type ext = v_[s].path.substr(v_[s].path.find_last_of("."));
					clx::downcase(ext);
					if (ext == ".ttc") v_[s].value = read_ttc(v_[s].path, v_[s].index);
					else if (ext == ".ttf") v_[s].value = read_ttf(v_[s].path);
					else if (ext == ".afm") v_[s].value = read_afm(v_[s].path);
					else return object_type(name, cset, dir);
				}
				
				return object_type(*v_[s].value, cset, dir);
			}
			
		private:
			typedef basic_font_property<char> property_type;
			typedef clx::shared_ptr<property_type> property_ptr;
			
			struct mapped_type {
				string_type path;
				size_type index;
				property_ptr value;
				
				mapped_type() : path(), index(0), value() {}
			};
			
			typedef std::map<string_type, mapped_type> font_map;
			
			/* ------------------------------------------------------------- */
			//  add_ttc
			/* ------------------------------------------------------------- */
			bool add_ttc(const string_type& path) {
				FILE *ttc = fopen(path.c_str(),"r");
				int version, cnt, i;
				int *offsets;
				
				if ( ttc==NULL ) return false;
				
				version = getlong(ttc);
				if ( version!=CHR('t','t','c','f')) {
					fclose(ttc);
					return false;
				}

				version = getlong(ttc);
				if ( version!=0x10000 && version != 0x20000 ) {
					fprintf( stderr, "Unexpected ttc version number: %08x\n", version );
				}
				cnt = getlong(ttc);
				offsets = (int*)malloc(cnt*sizeof(int));
				for ( i=0; i<cnt; ++i )
					offsets[i] = getlong(ttc);
				for ( i=0; i<cnt; ++i ) {
					string_type tmp(tempnam(".", "ttc"));
					handlefont(tmp.c_str(),i,ttc,offsets[i]);
					mapped_type elem;
					elem.path = path;
					elem.value = read_ttf(tmp);
					remove(tmp.c_str());
					if (!elem.value) continue;
					v_[elem.value->name()] = elem;
				}
				fclose(ttc);
				free(offsets);
				
				return true;
			}
			
			/* ------------------------------------------------------------- */
			//  read_ttc
			/* ------------------------------------------------------------- */
			static property_ptr read_ttc(const string_type& path, size_type index) {
				FILE *ttc = fopen(path.c_str(),"r");
				int version, cnt, i;
				int *offsets;
				
				if ( ttc==NULL ) return property_ptr();
				
				version = getlong(ttc);
				if ( version!=CHR('t','t','c','f')) {
					fclose(ttc);
					return property_ptr();
				}

				version = getlong(ttc);
				if ( version!=0x10000 && version != 0x20000 ) {
					fprintf( stderr, "Unexpected ttc version number: %08x\n", version );
				}
				cnt = getlong(ttc);
				offsets = (int*)malloc(cnt*sizeof(int));
				for ( i=0; i<cnt; ++i ) offsets[i] = getlong(ttc);
				
				if (static_cast<int>(index) >= cnt) return property_ptr();
				string_type tmp(tempnam(".", "ttc"));
				handlefont(tmp.c_str(),index,ttc,offsets[index]);
				property_ptr dest = read_ttf(tmp);
				remove(tmp.c_str());
				fclose(ttc);
				free(offsets);
				
				return dest;
			}
			
			/* ------------------------------------------------------------- */
			//  read_ttf
			/* ------------------------------------------------------------- */
			static property_ptr read_ttf(const string_type& path) {
				string_type tmp(tempnam(".", "ttf"));
				
				char* args[12];
				std::vector<std::string> v;
				v.push_back(string_type("ttf2afm"));
				v.push_back(string_type("-i"));
				v.push_back(string_type("-o"));
				v.push_back(tmp);
				v.push_back(path);
				
				size_type i = 0;
				for (; i < v.size(); ++i) args[i] = const_cast<char*>(v.at(i).c_str());
				args[i] = NULL;
				
				int pid, status;
				if ((pid = fork()) < 0) throw std::runtime_error("failed to fork");
				if ( pid == 0 ) {
					execvp(*args, args);
					std::runtime_error("failed to execvp");
					std::exit(-1);
				}
				
				while(wait(&status) != pid) {
					// wait
				}
				
				property_ptr dest = read_afm(tmp);
				remove(tmp.c_str());
				return dest;
			}
			
			/* ------------------------------------------------------------- */
			//  read_afm
			/* ------------------------------------------------------------- */
			static property_ptr read_afm(const string_type& path) {
				std::ifstream in(path.c_str());
				property_ptr dest(new property_type());
				dest->type(0);
				dest->flags(32);
				dest->stemv(80);
				
				string_type line;
				while (std::getline(in, line)) {
					clx::chomp(line);
					if (line.find(" ") == string_type::npos) continue;
					string_type key = line.substr(0, line.find(" "));
					string_type val = line.substr(line.find(" ") + 1);
					if (key == "FullName") dest->name(val);
					else if (key == "ItalicAngle") dest->italic_angle(clx::lexical_cast<int>(val));
					else if (key == "Descender") dest->descent(clx::lexical_cast<int>(val));
					else if (key == "Ascender") {
						dest->ascent(clx::lexical_cast<int>(val));
						dest->cap_height(clx::lexical_cast<int>(val));
					}
					else if (key == "FontBBox") {
						std::vector<string_type> tok;
						clx::split(val, tok);
						dest->bbox(square(
							coordinate(clx::lexical_cast<double>(tok.at(0)), clx::lexical_cast<double>(tok.at(1))),
							coordinate(clx::lexical_cast<double>(tok.at(2)), clx::lexical_cast<double>(tok.at(3)))
						));
					}
					else if (key == "StartCharMetrics") {
						int n = clx::lexical_cast<int>(val);
						read_widths(in, dest, n);
					}
				}
				
//#ifdef FAML_DEBUG
#if 0
				std::cout << "name: " << dest->name() << std::endl;
				std::cout << "ascent: " << dest->ascent() << std::endl;
				std::cout << "descent: " << dest->descent() << std::endl;
				std::cout << "italic angle: " << dest->italic_angle() << std::endl;
				std::cout << "widths: " << dest->widths().size() << std::endl;
				//std::cout << "--" << std::endl;
				//std::copy(dest->widths().begin(), dest->widths().end(),
				//	std::ostream_iterator<int>(std::cout, " "));
				//std::cout << std::endl;
#endif
				return dest;
			}
			
			/* ------------------------------------------------------------- */
			//  read_widths
			/* ------------------------------------------------------------- */
			static bool read_widths(istream_type& in, property_ptr& dest, size_type n) {
				for (size_type i = 0; i < n; ++i) {
					static const string_type format("C %s ; WX %s ; N %s ; B %s ;");
					
					string_type line;
					std::getline(in, line);
					if (in.fail()) return false;
					int width = -1;
					clx::scanner(line, format)(1, width);
					if (width < 0) continue;
					dest->width(width);
				}
				
				return true;
			}
			
			font_map v_;
			std::map<string_type, string_type> aliases_;
		};
	} // namespace pdf
} // namespace cliff

#endif // CLIFF_PDF_FONT_FACTORY_H
