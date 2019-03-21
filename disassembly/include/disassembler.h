#pragma once

#include <string>
#include <map>

#include "smart_descriptor.h"
#include "Signature.h"

class Disassembler {
public:
	void Disassembly(const std::string & pathToSrc);
	void OpenFiles(const std::string & fileName);
private:

	void ParseHeaders();
	void ParseData();
	void ParseByteCode();

	void ParseLablesAddrs();
	
	void decode_register();
	void decode_register_star();
	void decode_number();
	void decode_static_data_label();
	void decode_label();

	void define_label();
	void define_fun();

	// Generated methods for parsing commands

#define DEF_CMD(NAME, TEXT, COND, PARSE_CODE) void decode_cmd_ ## NAME ## ();
#include "cpuemu-cmd-defs.h"
#undef DEF_CMD

#define DEF_CMD(NAME, TEXT, COND, PARSE_CODE) void first_run_ ## NAME ## ();
#include "cpuemu-cmd-defs.h"
#undef DEF_CMD

	std::map<char*, std::string> labels_names;

	SmartDescriptor byteCodeDescriptor;
	SmartDescriptor resultDescriptor;
	
	char* byteCodeBuffer;
	size_t byteCodeSize;

	char* current_segment;
	size_t total_size;
	
	Signature headers;

	template<class T>
	T extract_type();
};