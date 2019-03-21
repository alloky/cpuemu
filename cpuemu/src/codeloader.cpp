#include "CodeLoader.h"

#include <exception>

void CodeLoader::load_binary(const char* path) {
	SmartDescriptor sd;
	sd.open_read(path);
	if (buff != nullptr) delete buff;
	sd.read_all(&buff, &size);
	sd.close();
	auto byte_code = std::string(buff, size);
	if (!signature.verify()) {
		std::exception("wrong signature");
	}
	signature = Signature::decode(byte_code);
}

CodeLoader::~CodeLoader() {
	if(buff != nullptr) delete buff;
}