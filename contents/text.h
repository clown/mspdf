/* ------------------------------------------------------------------------- */
/*
 *  contents/text.h
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
 *  Last-modified: Sun 19 Apr 2009 17:33:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef FAML_PDF_CONTENTS_TEXT_H
#define FAML_PDF_CONTENTS_TEXT_H

#include <string>
#include "clx/format.h"
#include "clx/literal.h"
#include "clx/hexdump.h"
#include "../piece.h"
#include "../coordinate.h"

namespace faml {
	namespace pdf {
		namespace contents {
			/* ------------------------------------------------------------- */
			//  basic_text
			/* ------------------------------------------------------------- */
			template <
				class CharT,
				class Traits = std::char_traits<CharT>
			>
			class basic_text {
			public:
				typedef size_t size_type;
				typedef CharT char_type;
				typedef std::basic_string<CharT, Traits> string_type;
				typedef basic_extpiece<CharT, Traits> value_type;
				typedef std::vector<value_type> container;
				
				explicit basic_text(const coordinate& o) :
					v_(), origin_(o), space_(0.0), entag_(true) {}
				
				explicit basic_text(const coordinate& o, bool x) :
					v_(), origin_(o), space_(0.0), entag_(x) {}
				
				virtual ~basic_text() throw() {}
				
				void add(const value_type& elem) { v_.push_back(elem); }
				void space(double cp) { space_ = cp; }
				
				/* --------------------------------------------------------- */
				//  main operator
				/* --------------------------------------------------------- */
				template <class OutStream>
				bool operator()(OutStream& out) {
					static const double angle = 0.3;
					
					double x = origin_.x();
					
					if (entag_) out << LITERAL("BT") << std::endl;
					if (!v_.empty()) out << fmt(LITERAL("%f Tc")) % space_ << std::endl;
					
					double x0 = origin_.x();
					for (size_type i = 0; i < v_.size(); ++i) {
						this->xput_font(out, v_.at(i));
						
						double y = origin_.y() - v_.at(i).size();
						double r = (v_.at(i).decorate() & value_type::italic) ? angle : 0.0;
						out << fmt(LITERAL("1 0 %f 1 %f %f Tm")) % r % x % y << std::endl;
						
						this->xput_data(out, v_.at(i));
						
						text_helper h(v_.at(i).font().property(), v_.at(i).font().charset());
						double w = h.width(v_.at(i).data().begin(), v_.at(i).data().end()) / 1000.0;
						double n = h.count(v_.at(i).data().begin(), v_.at(i).data().end());
						double x = x0 + w * v_.at(i).size() + space_ * n;
						
						if ((v_.at(i).decorate() & value_type::underline)) {
							this->xput_line(out, coordinate(x0, origin_.y() - v_.at(i).size() * 1.1),
								x - x0, v_.at(i).size() / 20.0, v_.at(i).fill());
						}
						else if ((v_.at(i).decorate() & value_type::strike)) {
							this->xput_line(out, coordinate(x0, origin_.y() - v_.at(i).size() * 0.7),
								x - x0, v_.at(i).size() / 20.0, v_.at(i).fill());
						}
						
						x0 = x;
					}
					if (entag_) out << LITERAL("ET") << std::endl;
					
					return true;
				}
				
			private:
				typedef clx::basic_format<CharT, Traits> fmt;
				
				container v_;
				coordinate origin_;
				double space_;
				bool entag_;
				
				/* --------------------------------------------------------- */
				/*
				 *  xput_font
				 *
				 *  フォント名（に関連付けられているラベル），フォント
				 *  サイズ，色，フォントの横幅の倍率，テキスト装飾を
				 *  出力する．
				 *  ※ただし，テキスト装飾に関しては太字に関する情報のみ．
				 */
				/* --------------------------------------------------------- */
				template <class OutStream, class Source>
				bool xput_font(OutStream& out, const Source& src) {
					static const double weight = 0.6;
					out << fmt(LITERAL("/%s %d Tf")) % src.name() % src.size() << std::endl;
					out << fmt(LITERAL("%d Tz")) % src.scale() << std::endl;
					if ((src.decorate() & value_type::bold)) {
						out << LITERAL("2 Tr") << std::endl;
						out << fmt(LITERAL("%f w")) % weight << std::endl;
					}
					else {
						out << LITERAL("0 Tr") << std::endl;
						out << LITERAL("1 w") << std::endl;
					}
					
					out << fmt(LITERAL("%f %f %f rg"))
						% src.fill().red() % src.fill().green() % src.fill().blue()
					<< std::endl;
					
					out << fmt(LITERAL("%f %f %f RG"))
						% src.fill().red() % src.fill().green() % src.fill().blue()
					<< std::endl;
					
					return true;
				}
				
				/* --------------------------------------------------------- */
				//  xput_line
				/* --------------------------------------------------------- */
				template <class OutStream>
				bool xput_line(OutStream& out, const coordinate& o,
					double width, double height, const color& clr) {
					out << LITERAL("ET") << std::endl;
					out << fmt(LITERAL("%f %f %f rg")) % clr.red() % clr.green() % clr.blue() << std::endl;
					out << fmt(LITERAL("%f w")) % height << std::endl;
					out << fmt(LITERAL("%f %f m")) % o.x() % o.y() << std::endl;
					out << fmt(LITERAL("%f %f l")) % (o.x() + width) % o.y() << std::endl;
					out << LITERAL("s") << std::endl;
					if (!entag_) out << LITERAL("BT") << std::endl;
					return true;
				}
				
				/* --------------------------------------------------------- */
				/*
				 *  xput_data
				 *
				 *  データ部（文字列）を出力する．
				 *  TODO: 将来的には，ここで文字コードの差を吸収する．
				 *  （PDF への最終的な出力は UTF16 を用いて，各種文字コード
				 *  から UTF16 への変換をここで行う）．
				 */
				/* --------------------------------------------------------- */
				template <class OutStream, class Source>
				bool xput_data(OutStream& out, const Source& src) {
					string_type s = src.data();
					if (faml::pdf::is_ascii(s.at(0), charset::utf16)) faml::pdf::strip(s, charset::utf16);
					out << LITERAL("<");
					clx::hexdump(s, std::ostream_iterator<string_type>(out));
					out << LITERAL("> Tj") << std::endl;
					
					return true;
				}
			};
		}
	}
}

#endif // FAML_PDF_CONTENTS_TEXT_H
