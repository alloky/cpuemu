// defines additional assembly derectives
#define ASSEMBLING

#pragma once

#include "cpuemuassembly.h"
#include "logging.h"


#include "cpuemu-cmds.h"

void CpuemuAssembly::compile(const char* name, const char* path, const char* out_dir) {
	FileLineProcesser flp;
	flp.open(path + std::string("/") + name + ".vbcasm");
	flp.read_lines();
	auto lv = flp.make_LinesView();

	signature.stack_size = DEFAULT_STACK_SIZE;

	CreateSignature(lv);
	GetByteCode(lv);
	__write_to_out(path, out_dir, name);
}


void CpuemuAssembly::__write_to_out(const char* path, const char* out_dir, const char* name) {
	SmartDescriptor sd_writer;
	sd_writer.open_write(out_dir + std::string("/") + name + "_" + version + ".vbc");
	sd_writer.write(signature.encode());
	sd_writer.write(byte_code);
	sd_writer.write(data_segment);
	sd_writer.write_offset(signature.stack_size);
	sd_writer.write('\0');
	sd_writer.close();
}

void CpuemuAssembly::GetByteCode(FileLineProcesser::LinesView* lv) {
	__get_labels_addresses(lv);
	calculateSegmentOffsets();
	__compilation_loop(lv);
	signature.stack_segment_addr = signature.data_segment_addr + data_segment.length();
}

 void CpuemuAssembly::__get_labels_addresses(FileLineProcesser::LinesView* lv) {
	size_t addr = sizeof(Signature);
	for (auto line : lv->lines) {
		if (line.size() == 0) continue;
		if (line[0] == '#' || line[0] == '.' || line[0] == EOF)	 continue;
		if (line[0] == ':') {
			labels_addrs[std::string(__parse_label_name(line, 0))] = addr;
			continue;
		}
		size_t first_word_end = line.find(' ');
		std::vector<std::pair<CPUVASM_ARG_TYPES, size_t>> args = __parse_args(line);
		LEV_LOG(LL_DEBUG, addr << " " << (int)args.size() << " " << __sum_args_size(args)) << line << '\n';
		addr += __sum_args_size(args) + 1;
	}
	code_segment_size = addr - sizeof(Signature);
}

 size_t CpuemuAssembly::__sum_args_size(std::vector<std::pair<CPUVASM_ARG_TYPES, size_t>>& args) {
	 size_t sum = 0;
	 for (auto& arg : args) {
		 switch (arg.first) {
			case CPUVASM_ARG_TYPES::REG_STAR:
				sum += sizeof(char);
				break;
			case CPUVASM_ARG_TYPES::REG:
				sum += sizeof(char);
				break;
			case CPUVASM_ARG_TYPES::LONG_STAR:
			case CPUVASM_ARG_TYPES::LONG:
				sum += sizeof(long long);
				break;
			case CPUVASM_ARG_TYPES::LABEL:
				sum += sizeof(size_t);
				break;
			case CPUVASM_ARG_TYPES::STATIC_DATA_STAR:
				sum += sizeof(size_t);
				break;
			default:
				throw std::exception("size of arg not implemented");
		 }
	 }
	 return sum;
 }

void CpuemuAssembly::__compilation_loop(FileLineProcesser::LinesView* lv) {
	addr = sizeof(Signature), 
	line_num = 0;
	for (auto line : lv->lines) {
		__process_line(line);
		line_num++;
	}
}


void CpuemuAssembly::__process_line(std::string_view& line) {
	if (line.size() == 0) return;
	if (line[0] == '#') return;
	if (line[0] == ':') return;
	size_t first_word_end = line.find(' ');
	
	std::vector<std::pair<CPUVASM_ARG_TYPES, size_t>> args;
	size_t _tmp_reg_val;
	long long _tmp_long_val;
	

	if (line[0] != '.') {
		args = __parse_args(line);
	}

	// Codegeneration for cmd parsing
	if (0) {}
#define DEF_CMD(CMD, WORD, COND, PARSE_CODE) else if((strncmp(line.data(), WORD , first_word_end) == 0) && COND){\
		if(true) PARSE_CODE \
		__push_args(line, args); \
	}

#define PUSH_NUM(CMD) byte_code.push_back(CPUE_CMD_NUM:: ## CMD);\
		addr++;
#define ARGS_SIZE args.size() 
#define IS_LONG(N) (args[N].first == CPUVASM_ARG_TYPES::LONG) 
#define IS_REG(N)  (args[N].first == CPUVASM_ARG_TYPES::REG)
#define IS_REG_STAR(N)  (args[N].first == CPUVASM_ARG_TYPES::REG_STAR)
#define IS_STATIC_STAR(N) (args[N].first == CPUVASM_ARG_TYPES::STATIC_DATA_STAR)

#define PARSE_SIZE_T \
	_tmp_reg_val = atoll(line.data() + first_word_end); \
	byte_code.append(std::string(reinterpret_cast<char*>(&_tmp_reg_val), sizeof(size_t))); \
	addr += sizeof(size_t);


#include "cpuemu-cmd-defs.h"

#undef DEF_CMD
#undef PUSH_NUM
#undef ARGS_SIZE
#undef IN_LONG
#undef IS_REG
#undef PARSE_REG
#undef PARSE_LONG
#undef PARSE_SIZE_T

}


std::vector<std::pair<CPUVASM_ARG_TYPES, size_t>> CpuemuAssembly::__parse_args(std::string_view& line) const {
	std::vector<std::pair<CPUVASM_ARG_TYPES, size_t>> ret;
	bool flag = false;
	for (size_t i = 0; i < line.size(); ++i) {
		if (flag) {
			if (line[i] != ' ' && line[i] != '\n') {
				if (line[i] >= '0' && line[i] <= '9') {
					ret.push_back({ CPUVASM_ARG_TYPES::LONG, i });
				}
				else if (line[i] == 'r') {
					ret.push_back({ CPUVASM_ARG_TYPES::REG, i });
				}
				else if (line[i] == ':') {
					ret.push_back({ CPUVASM_ARG_TYPES::LABEL, i });
				}
				else if (line[i] == '$') {
					ret.push_back({ CPUVASM_ARG_TYPES::STATIC_DATA_STAR, i });
				}
				else if (line[i] == '[') {
					++i;
					if (line[i] >= '0' && line[i] <= '9') {
						ret.push_back({ CPUVASM_ARG_TYPES::LONG_STAR, i });
					}
					else if (line[i] == 'r') {
						ret.push_back({ CPUVASM_ARG_TYPES::REG_STAR, i });
					}
					else {
						LEV_LOG(LL_ERR, "Compilation error in line ");
					}
				}
				else {
					LEV_LOG(LL_ERR, "Compilation error in line ");
				}
				flag = false;
			}
		}
		else if (line[i] == ' ') {
			flag = true;
		}
	}
	return ret;
}

void CpuemuAssembly::__push_args(string_view& line, std::vector<std::pair<CPUVASM_ARG_TYPES, size_t>>& args) {
	if (args.size() == 0) {
		return;
	}

	for (size_t i = 0; i < args.size(); ++i) {
		switch (args[i].first)
		{
		case CPUVASM_ARG_TYPES::LABEL:
			__push_LABEL(line, args[i].second);
			break;

		case CPUVASM_ARG_TYPES::LONG:
			__push_LONG(line, args[i].second);
			break;

		case CPUVASM_ARG_TYPES::LONG_STAR:
			__push_LONG_STAR(line, args[i].second);
			break;

		case CPUVASM_ARG_TYPES::REG:
			__push_REG(line, args[i].second);
			break;

		case CPUVASM_ARG_TYPES::REG_STAR:
			__push_REG_STAR(line, args[i].second);
			break;
		case CPUVASM_ARG_TYPES::STATIC_DATA_STAR:
			__push_STATIC_DATA_STAR(line, args[i].second);
			break;
		default:
			break;
		}
	}
}

string_view CpuemuAssembly::__parse_label_name(string_view& line, size_t start) {
	size_t label_name_start = start + 1;
	size_t label_name_end = line.find(' ', label_name_start);
	if (label_name_end != line.size()) {
		label_name_end -= 1;
	}
	return line.substr(label_name_start, label_name_end - label_name_start + 1);
}

char CpuemuAssembly::__parse_reg_name(string_view & line, size_t start)
{
	size_t value_part_start = start + 1;
	if (('a' <= line[value_part_start]) && ('d' >= line[value_part_start])) {
		return line[value_part_start] - 'a';
	}
	else {
		return atoi(line.data() + value_part_start) + 4;
	}
}

void CpuemuAssembly::__push_LABEL(std::string_view& line, size_t start) {
	std::string label_name = std::string(__parse_label_name(line, start));
	if (labels_addrs.find(label_name) == labels_addrs.end()) {
		std::cout << "Assembly error on line " << line_num << " label name not found: " << label_name << std::endl;
		throw std::exception("Invalid label name");
	}
	size_t lbl_addr = labels_addrs[label_name];
	push_bytes<size_t>(lbl_addr);
}

void CpuemuAssembly::__push_LONG(std::string_view & line, size_t start)
{
	long long _tmp_long_val = atoll(line.data() + start); 
	push_bytes<long long>(_tmp_long_val);
}

void CpuemuAssembly::__push_LONG_STAR(std::string_view & line, size_t start)
{
	throw std::exception("not implemented");
}

void CpuemuAssembly::__push_STATIC_DATA_STAR(std::string_view & line, size_t start)
{
	std::string name = std::string(line.substr(start + 1, line.find(' ', start + 1) - start));
	push_bytes<size_t>(labels_addrs[name] + signature.data_segment_addr);
}

void CpuemuAssembly::__push_REG(std::string_view & line, size_t start)
{
	char _tmp_reg_val = __parse_reg_name(line, start); 
	push_bytes<char>(_tmp_reg_val);
}

void CpuemuAssembly::__push_REG_STAR(std::string_view & line, size_t start)
{
	char _tmp_reg_val = __parse_reg_name(line, start);
	push_bytes<char>(_tmp_reg_val);
}

template <class T>
void CpuemuAssembly::push_bytes(T v) {
	byte_code.append(std::string(reinterpret_cast<char*>(&v), sizeof(T)));
	addr += sizeof(T);
};


void CpuemuAssembly::CreateSignature(FileLineProcesser::LinesView* lv) {
	signature.V = 'V';
	signature.B = 'B';
	signature.C = 'C';
}




void CpuemuAssembly::calculateSegmentOffsets() {
	signature.code_segment_addr = sizeof(signature);
	signature.data_segment_addr = signature.code_segment_addr + code_segment_size;
}

void CpuemuAssembly::SET_STACK_SIZE(std::string_view& line, size_t first_word_end) {
	long long _tmp_long_val = atoll(line.data() + first_word_end);
	signature.stack_size = _tmp_long_val;
}

void CpuemuAssembly::PUSH_TEXT_DATA(std::string_view& line, size_t first_word_end) {
	size_t left = line.find('"');
	size_t right = line.find('"', left + 1);
	std::string text = std::string(line.substr(left + 1, right - left + 1));
	text[text.size() - 1] = '\0';

	size_t label_start = line.find(' ');
	size_t label_end = line.find(' ', label_start + 1);
	std::string label_name = std::string(line.substr(label_start + 1, label_end  - label_start - 1));
	
	labels_addrs.insert({ label_name, data_segment.length() });
	data_segment.append(text);

}

#undef ASSEMBLING