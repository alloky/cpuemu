#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <string_view>

#include "file_tools.h"
//#include "my_string_view.h"
#include "smart_descriptor.h"

//using string_view = my_string_view;

/**
 * @brief      FileLineProcesser
 */
class FileLineProcesser {

	SmartDescriptor sfd; //!< fd for io ops
	char* buff;
	size_t size;
public:
	
	/**
	 * @brief      Array of lines, mutable
	 */
	class LinesView {
	public:
		std::vector<string_view> lines; //!< array of pointers to lines
		

		/**
		 * @brief      Print top <b> size </b> non-empty lines
		 *
		 * @param[in]  size  The size
		 */
		void print_top_non_empty(size_t size);
		

		/**
		 * @brief      Writes to file, creates it if not_exists 
		 *
		 * @param[in]  filename  path to file
		 *
		 * @return     status code, 0 if OK
		 */
		int write(const std::string& path, char sep='\n');


		LinesView() = default; //!< default constructor
		LinesView(const LinesView&) = default;
		~LinesView() = default; //!< default destructor

	};

	
	/**
	 * @brief Return new LinesView
	 */
	LinesView* make_LinesView();
	

	/**
	 * @brief      Opens file for io 
	 *
	 * @param[in]  filename  The filename
	 *
	 * @return     status code, 0 if OK
	 */
	int open(const std::string& filename);

	/**
	 * @brief      Read file
	 *
	 * @param[in]  filename  Path to file
	 * 
	 * @return     status code, 0 if OK
	 * 
	 * Read file to internal buffer
	 */
	int read_lines();

	FileLineProcesser();
	~FileLineProcesser();
private:

	LinesView* precalced_lv; //!< lines ptrs
};
