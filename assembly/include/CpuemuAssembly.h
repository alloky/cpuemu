#pragma once
#include <map>

#include "smart_descriptor.h"
#include "filelineprocesser.h"
#include "cpuemu-cmds.h"
#include "Signature.h"

class CpuemuAssembly {
public:
	
	/* Layout : 
	
	[signature][.code][.data][.stack]

	*/
	
	const std::string version = "0.0.2";

	void compile(const char* name, const char* path, const char* out_dir);

private:
	std::map<std::string, size_t> labels_addrs;
	void __get_labels_addresses(FileLineProcesser::LinesView* lv);
	void __compilation_loop(FileLineProcesser::LinesView* lv);
	void __process_line(std::string_view& line);
	void __write_to_out(const char * path, const char * out_dir, const char * name);

	void CreateSignature(FileLineProcesser::LinesView * lv);
	void calculateSegmentOffsets();
	void SET_STACK_SIZE(std::string_view & line, size_t first_word_end);
	void PUSH_TEXT_DATA(std::string_view & line, size_t first_word_end);
	void GetByteCode(FileLineProcesser::LinesView* lv);
	
	std::vector<std::pair<CPUVASM_ARG_TYPES, size_t>> __parse_args(std::string_view& line) const;
	size_t __sum_args_size(std::vector<std::pair<CPUVASM_ARG_TYPES, size_t>>& args);

	void __push_args(std::string_view& line, std::vector<std::pair<CPUVASM_ARG_TYPES, size_t>>& args);


	void __push_LABEL(std::string_view & line, size_t start);
	void __push_LONG(std::string_view & line, size_t start);
	void __push_LONG_STAR(std::string_view & line, size_t start);
	void __push_STATIC_DATA_STAR(std::string_view & line, size_t start);
	void __push_REG(std::string_view & line, size_t start);
	void __push_REG_STAR(std::string_view & line, size_t start);


	template<class T>
	void push_bytes(T v);

	string_view __parse_label_name(string_view& line, size_t start);
	char __parse_reg_name(string_view& line, size_t start);


	Signature signature;

	size_t addr{ 0 },
	   line_num{ 0 };
	
	size_t DEFAULT_STACK_SIZE = 32 * 1024;

	size_t code_segment_size;
	std::string byte_code;
	std::string data_segment;
};