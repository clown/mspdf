#ifndef FAML_XLSX_CELL_H
#define FAML_XLSX_CELL_H

namespace faml {
	namespace xlsx {
		/* ----------------------------------------------------------------- */
		//  cell
		/* ----------------------------------------------------------------- */
		class cell {
		public:
			typedef size_t size_type;
			
			/* ------------------------------------------------------------- */
			//  cell type
			/* ------------------------------------------------------------- */
			enum {
				empty		= 0x0001,
				sst			= 0x0002,
				value		= 0x0004,
				row_beg		= 0x0010,
				row_merge	= 0x0020,
				row_end		= 0x0040,
				col_beg		= 0x0100,
				col_merge	= 0x0200,
				col_end		= 0x0400,
				hidden		= 0xf000
			};
			
			/* ------------------------------------------------------------- */
			//  constructor and destructor
			/* ------------------------------------------------------------- */
			cell() :
				type_(0), style_(0), data_(0.0),
				width_(0.0), height_(0.0) {}
			
			virtual ~cell() throw() {}
			
			/* ------------------------------------------------------------- */
			//  Access methods (get).
			/* ------------------------------------------------------------- */
			size_type type() const { return type_; }
			size_type style() const { return style_; }
			double data() const { return data_; }
			double width() const { return width_; }
			double height() const { return height_; }
			
			/* ------------------------------------------------------------- */
			//  Access methods (set).
			/* ------------------------------------------------------------- */
			void type(size_type cp) { type_ = cp; }
			void style(size_type cp) { style_ = cp; }
			void data(double cp) { data_ = cp; }
			void width(double cp) { width_ = cp; }
			void height(double cp) { height_ = cp; }
			
		private:
			size_type type_;
			size_type style_;
			double data_;
			double width_;
			double height_;
		};
	}
}

#endif // FAML_XLSX_CELL_H
