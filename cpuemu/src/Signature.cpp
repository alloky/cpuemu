#include "Signature.h"

template <class T>
void push_bytes(std::string& byte_code, T v) {
	byte_code.append(std::string(reinterpret_cast<char*>(&v), sizeof(T)));
};

std::string Signature::encode()
{
	std::string representation;
	/*
	push_bytes<char>(representation, V);
	push_bytes<char>(representation, B);
	push_bytes<char>(representation, C);
	push_bytes<size_t>(representation, code_segment_addr);
	push_bytes<size_t>(representation, data_segment_addr);
	push_bytes<size_t>(representation, stack_segment_addr);
	push_bytes<size_t>(representation, stack_size);
	*/
	for (int i = 0; i < sizeof(Signature); ++i) {
		representation.push_back(*(reinterpret_cast<char*>(this) + i));
	}
	return representation;
}

Signature Signature::decode(std::string & input)
{
	Signature s = *(reinterpret_cast<const Signature*>(input.c_str()));
	return s;
}

bool Signature::verify()
{
	return  
		V == 'V' &&
		B == 'B' &&
		C == 'C' &&
		stack_size > 0 &&
		code_segment_addr == sizeof(Signature) &&
		data_segment_addr >= code_segment_addr &&
		stack_segment_addr >= data_segment_addr;
}
