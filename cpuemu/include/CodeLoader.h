#pragma once

#include "smart_descriptor.h"
#include "Signature.h"

class CodeLoader {
	/* Layout :

	[signature][.code][.data][.stack]
	
	*/

public:
	size_t size;
	char* buff;
	Signature signature;

	void load_binary(const char* path);
	~CodeLoader();
};