#pragma once

#include <string>

class Signature {
	/*
	//-----Signature------//
		char : V
		char : B
		char : C
		size_t : code_segment_addr
		size_t : data_segment_addr
		size_t : stack_segment_addr
		size_t : stack_size
	*/
public:
	char V;
	char B;
	char C;
	size_t code_segment_addr;
	size_t data_segment_addr;
	size_t stack_segment_addr;
	size_t stack_size;

	std::string encode();
	bool verify();

	static Signature decode(std::string& input);
};