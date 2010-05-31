#ifndef FAML_PDF_UTF8_H
#define FAML_PDF_UTF8_H

#include "clx/utf8.h"

namespace faml {
	namespace pdf {
		namespace utf8 {
			/* ------------------------------------------------------------- */
			//  is_alnum
			/* ------------------------------------------------------------- */
			inline bool is_alnum(int c) {
				if (static_cast<unsigned int>(c) >= 0x7f) return false;
				return std::isalnum(static_cast<char>(c)) != 0;
			}
			
			/* ------------------------------------------------------------- */
			//  is_space
			/* ------------------------------------------------------------- */
			inline bool is_space(int c) {
				unsigned int uc = static_cast<unsigned int>(c);
				if (uc == 0x20) return true;
				return false;
			}
			
			/* ------------------------------------------------------------- */
			//  is_break
			/* ------------------------------------------------------------- */
			inline bool is_break(int c) {
				unsigned int uc = static_cast<unsigned int>(c);
				if (uc == 0x0c || uc == 0x0a) return true;
				return false;
			}
			
			/* ------------------------------------------------------------- */
			//  is_lpunct
			/* ------------------------------------------------------------- */
			inline bool is_lpunct(int c) {
				static const unsigned int codes[] = {
					
				};
				
				unsigned int uc = static_cast<unsigned int>(c);
				for (size_t i = 0; i < sizeof(codes) / sizeof(unsigned int); ++i) {
					if (uc == codes[i]) return true;
				}
				return false;
			}
			
			/* ------------------------------------------------------------- */
			//  is_rpunct
			/* ------------------------------------------------------------- */
			inline bool is_rpunct(int c) {
				static const unsigned int codes[] = {
					
				};
				
				if (is_space(c) || is_break(c)) return true;
				unsigned int uc = static_cast<unsigned int>(c);
				for (size_t i = 0; i < sizeof(codes) / sizeof(unsigned int); ++i) {
					if (uc == codes[i]) return true;
				}
				return false;
			}
			
			/* ------------------------------------------------------------- */
			//  is_ascii
			/* ------------------------------------------------------------- */
			inline bool is_ascii(const std::basic_string<char>& s) {
				std::basic_string<char>::const_iterator pos = s.begin();
				while (pos != s.end()) {
					unsigned char c = static_cast<unsigned char>(*pos);
					if (c > 0x7f) return false;
					clx::utf8::advance(pos, 1);
				}
				return true;
			}
			
			/* ------------------------------------------------------------- */
			/*
			 *  chomp
			 *
			 *  末尾の改行コード (0x0a, 0x0d), および改ページコード (0x0c)
			 *  を取り除く．
			 */
			/* ------------------------------------------------------------- */
			inline std::basic_string<char>& chomp(std::basic_string<char>& src) {
				size_t pos = src.size();
				while (pos > 1) {
					unsigned char c = src.at(pos - 1);
					if (c != 0x0a && c != 0x0d && c != 0x0c) break;
					--pos;
				}
				
				if (pos - 1 == 0) {
					unsigned char c = src.at(pos - 1);
					if (c == 0x0a || c == 0x0d || c == 0x0c) --pos;
				}
				
				src.erase(pos);
				return src;
			}
			
			/* ------------------------------------------------------------- */
			//  count
			/* ------------------------------------------------------------- */
			template <class InIter>
			inline size_t count(InIter first, InIter last) {
				return clx::utf8::distance(first, last);
			}
			
			/* ------------------------------------------------------------- */
			//  width
			/* ------------------------------------------------------------- */
			template <class InIter, class Container>
			inline size_t width(InIter first, InIter last, const Container& ws) {
				size_t n = 0;
				while (first != last) {
					byte_type c = static_cast<byte_type>(*first);
					if (c <= 0x7f) {
						if (c < ws.size()) n += ws.at(c);
						else n += 500;
					}
					else n += 1000;
					clx::utf8::advance(first);
				}
				return n;
			}
			
			/* ------------------------------------------------------------- */
			//  gets
			/* ------------------------------------------------------------- */
			template <class InIter, class OutIter, class Container, class Predicate>
			inline size_t gets(InIter& first, InIter last, OutIter out,
				const Container& ws, Predicate pred) {
				size_t n = 0;
				while (first != last) {
					if (!pred(!clx::utf8::peek(first, last))) break;
					byte_type c = static_cast<byte_type>(*first);
					if (c <= 0x7f) {
						if (c <= ws.size()) n += ws.at(c);
						else n += 500;
					}
					else n += 1000;
					out = clx::utf8::get(first, last, out);
				}
				return n;
			}
			
			/* ------------------------------------------------------------- */
			//  getline
			/* ------------------------------------------------------------- */
			template <class InIter, class OutIter, class Container>
			inline size_t getline(InIter& first, InIter last, OutIter out,
				const Container& ws, size_t limit, bool wrap = true) {
#if 0
				if (first == last) return 0;
				
				std::deque<byte_type> v;
				InIter cur = first;
				size_t m = 0;
				size_t n = 0; // 読み込んだ総文字幅
				
				while (first != last && n <= limit) {
					byte_type c = static_cast<byte_type>(*first);
					size_t l = 0;
					if (c <= 0x7f) {
						if (c <= ws.size()) l = ws.at(c);
						else l = 500;
					}
					else l = 1000;
					
					if (n + m + l > limit) {
						size_t wc = clx::utf8::getword(first, last);
						if (is_rpunct(wc)) {
							std::insert_iterator<std::deque<byte_type> > oit(v, v.end());
							m += faml::pdf::utf8::gets(first, last, oit, ws, is_rpunct);
							out = std::copy(v.begin(), v.end(), out);
							v.clear();
							n += m;
						}
						else {
							
						}
						break;
					}
					
					m += l;
					std::insert_iterator<std::deque<byte_type> > oit(v, v.end());
					clx::utf8::getword(first, last, oit);
					clx::utf8::advance(first);
				}
#endif
				return 0;
			}
		}
	}
}

#endif // FAML_PDF_TEXT_HELPER_UTF8_H
