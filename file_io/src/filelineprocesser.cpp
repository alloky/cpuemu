
#include "filelineprocesser.h"
#include "logging.h"



FileLineProcesser::LinesView* FileLineProcesser::make_LinesView(){
	LinesView* lv = new LinesView();
	
	if(precalced_lv == NULL){
		precalced_lv = new LinesView();
		char** ptr_arr = NULL;
		size_t arr_size = make_ptr_arr(buff, &ptr_arr, size);
		precalced_lv->lines = std::vector<string_view>(arr_size);	
		for(size_t i = 0; i < arr_size; ++i){
			precalced_lv->lines[i] = string_view(ptr_arr[i], strlen(ptr_arr[i]));
		}
	}

	*lv = *precalced_lv;
	LEV_LOG(LL_INFO, "HERE");
	return lv;
}

int FileLineProcesser::LinesView::write(const std::string& path, char sep){
	LEV_LOG(LL_INFO, "Writitng lines to file : " << path);

	SmartDescriptor temp_fd;	
	int status = temp_fd.open_write(path);
	if(status != 0){
		return status;
	}
	
	for(auto& line : lines){
		if((status = temp_fd.write(line, sep)) != 0){
			return status;
		}
	}
	temp_fd.close();

	LEV_LOG(LL_INFO, "Done");
	return 0;
}

int FileLineProcesser::open(const std::string& path){
	return sfd.open_read(path);
}

int FileLineProcesser::read_lines(){
	if(buff != NULL){
		delete buff;
	}
	int status = sfd.read_all(&buff, &size);
	if(status != 0 ){
		return status;
	}
	LEV_LOG(LL_INFO, "All lines were read" );
	return status;
}


void FileLineProcesser::LinesView::print_top_non_empty(size_t size){
	auto min_l = std::min((size_t)size, (size_t)lines.size());

	LEV_LOG(LL_INFO, "Top " << min_l << " non-empty lines\n");

	for (size_t i = 1, j = 0; i <= min_l; ++j) {
		if(lines[j].size() >= 10) {
			std::cout << lines[j] << "\n";
			//if(i % 4 == 0) std::cout << "\n";
			++i;
		}
	}
	std::cout << '\n';
	LEV_LOG(LL_INFO, "End");
}

/*
bool FileLineProcesser::LinesView::_is_prep(char c){
	switch(c){
		case '.': case ';': case ',': case '!': case '?':
		case '[' : case ']' : case '(' : case ')' :
		case '"' : case ' ' : case '\'' : case ':':
		case '-' : case '*' :
		case '0' : case '1' : case '2' :
		case '3' : case '4' : case '5' :
		case '6' : case '7' : case '8' : case '9' :
			return true;
		default:
			return false;
	}
}
*/

FileLineProcesser::FileLineProcesser() :
 buff(NULL),
 size(0),
 precalced_lv(NULL) {

 }

FileLineProcesser::~FileLineProcesser(){
	if(buff != NULL) delete buff;
	if(precalced_lv != NULL) delete precalced_lv;
}
