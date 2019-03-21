#include "..\include\disassembler.h"

#include "cpuemu.h"

void Disassembler::Disassembly(const std::string & fileName)
{
	OpenFiles(fileName);
	ParseHeaders();
	ParseData();
	ParseByteCode();
	resultDescriptor.close();
}


void Disassembler::OpenFiles(const std::string& fileName) {
	int status = byteCodeDescriptor.open_read(fileName + ".vbc");
	if (status != 0) {
		throw std::exception("error while opening file");
	}
	status = resultDescriptor.open_write(fileName + "_disassemled.txt");
	if (status != 0) {
		throw std::exception("error while opening file");
	}
	byteCodeDescriptor.read_all(&byteCodeBuffer, &byteCodeSize);
	byteCodeDescriptor.close();
}


void Disassembler::ParseHeaders()
{
	headers = *reinterpret_cast<Signature*>(byteCodeBuffer);
	if (!headers.verify()) {
		throw std::exception("error while parsing headers : wrong signature");
	}
	resultDescriptor.write(".stack_size " + std::to_string(headers.stack_size), '\n');
}

void Disassembler::ParseData()
{
	char* data_start = byteCodeBuffer + headers.data_segment_addr;
	size_t data_size = headers.stack_segment_addr - headers.data_segment_addr;
	size_t total_size = 0;
	size_t count = 0;
	while (total_size < data_size) {
		char* curPosition = data_start + total_size;
		int curLineLength = strlen(curPosition);
		std::string_view currentLine = std::string_view(curPosition, curLineLength);
		resultDescriptor.write(".text ");
		labels_names.insert({ total_size + data_start, "msg_" + std::to_string(count) });
		resultDescriptor.write("msg_" + std::to_string(count), ' ');
		resultDescriptor.write(currentLine, '\n');
		total_size += curLineLength + 1;
		count++;
	}
	resultDescriptor.write("",'\n');
}

void Disassembler::ParseByteCode()
{
	char* code_start = byteCodeBuffer + headers.code_segment_addr;
	current_segment = code_start;
	size_t code_size = headers.data_segment_addr - headers.code_segment_addr;
	ParseLablesAddrs();
	total_size = 0;
	while (total_size < code_size) {
		char cmd = current_segment[total_size];
		if (labels_names.find(total_size + current_segment) != labels_names.end()) {
			resultDescriptor.write("", '\n');
			resultDescriptor.write(":" + labels_names[total_size + current_segment], '\n');
		}
		total_size++;
		switch (cmd)
		{
#define DEF_CMD(NAME, TEXT, COND, PARSE_CODE) \
		case CPUE_CMD_NUM:: ## NAME : \
			resultDescriptor.write(TEXT, ' '); \
			decode_cmd_ ## NAME ## (); \
			resultDescriptor.write("",'\n'); \
			break;
#include "cpuemu-cmd-defs.h"
#undef DEF_CMD
		default:
			break;
		}
	}
}

void Disassembler::ParseLablesAddrs() {
	size_t code_size = headers.data_segment_addr - headers.code_segment_addr;
	total_size = 0;
	while (total_size < code_size) {
		char cmd = current_segment[total_size];
		total_size++;
		switch (cmd)
		{
#define DEF_CMD(NAME, TEXT, COND, PARSE_CODE) \
		case CPUE_CMD_NUM:: ## NAME : \
			first_run_ ## NAME ## (); \
			break;
#include "cpuemu-cmd-defs.h"
#undef DEF_CMD

		default:
			break;
		}
	}
}


template <class T>
T Disassembler::extract_type() {
	T ret = *(reinterpret_cast<T*>(current_segment + total_size));
	total_size += sizeof(T);
	return ret;
}

void Disassembler::decode_register() {
	char reg_num = extract_type<char>();
	if (reg_num < 4) {
		std::string s = "r" + std::string(1, 'a' + reg_num) + std::string(1, 'x');
		resultDescriptor.write(s);
	}
	else {
		resultDescriptor.write("r" + std::to_string(reg_num - 4));
	}
}

void Disassembler::decode_register_star() {
	char reg_num = extract_type<char>();
	if (reg_num < 4) {
		std::string s = "[r" + std::string(1, 'a' + reg_num) + "x]";
		resultDescriptor.write(s);
	}
	else {
		std::string s = "[r" + std::to_string(reg_num - 4) + std::string(1, ']');
		resultDescriptor.write(s);
	}
}


void Disassembler::decode_number() {
	long long value = extract_type<long long>();
	resultDescriptor.write(std::to_string(value));
}

void Disassembler::decode_static_data_label() {
	size_t addr = extract_type<size_t>();
	if (labels_names.find(byteCodeBuffer + addr) == labels_names.end()) {
		labels_names.insert({byteCodeBuffer + addr, std::to_string(labels_names.size())});
	}
	resultDescriptor.write("$" + labels_names[byteCodeBuffer + addr]);
}

void Disassembler::decode_label() {
	size_t addr = extract_type<size_t>();
	if (labels_names.find(byteCodeBuffer + addr) == labels_names.end()) {
		labels_names.insert({ byteCodeBuffer + addr, std::to_string(labels_names.size()) });
	}
	resultDescriptor.write(":" + labels_names[byteCodeBuffer + addr]);
}

void Disassembler::decode_cmd_IN() {
}

void Disassembler::decode_cmd_OUT() {
}

void Disassembler::decode_cmd_OUTC() {
}

void Disassembler::decode_cmd_PUSH() {
	decode_number();
}

void Disassembler::decode_cmd_POP() {
}

void Disassembler::decode_cmd_PUSHR() {
	decode_register();
}

void Disassembler::decode_cmd_POPR() {
	decode_register();
}

void Disassembler::decode_cmd_MOV() {
	decode_register();
	resultDescriptor.write("", ' ');
	decode_number();
}

void Disassembler::decode_cmd_MOVL_REG_STATIC_STAR() {
	decode_register();
	resultDescriptor.write("", ' ');
	decode_static_data_label();
}


/*
void Disassembler::decode_cmd_SQRT() {
	++instruction_pointer;
	char reg = extract_type<char>();
	regs[reg] = sqrt(regs[reg]);
}
*/

void Disassembler::decode_cmd_CMP() {
}

void Disassembler::decode_cmd_JMP() {
	decode_label();
}

void Disassembler::decode_cmd_JL() {
	decode_label();
}

void Disassembler::decode_cmd_JLE() {
	decode_label();
}

void Disassembler::decode_cmd_JEQ() {
	decode_label();
}

void Disassembler::decode_cmd_JNE() {
	decode_label();
}


void Disassembler::decode_cmd_JGE() {
	decode_label();
}

void Disassembler::decode_cmd_JG() {
	decode_label();
}

void Disassembler::decode_cmd_CALL() {
	decode_label();
}

void Disassembler::decode_cmd_RET() {
}

void Disassembler::decode_cmd_MEW() {
}

void Disassembler::decode_cmd_MAX_CMD_NUM() {
}

#define ST_ARITHM_INST(cmd, op) \
void Disassembler::decode_cmd_ ## cmd ## (){ \
}

ST_ARITHM_INST(ADD, +)
ST_ARITHM_INST(SUB, -)
ST_ARITHM_INST(MUL, *)
ST_ARITHM_INST(MOD, %)

#undef ST_ARITHM_INST

void Disassembler::decode_cmd_DIV() {
}

#define LEFTWISE_REG_INSTRUCTION(cmd, op) \
void Disassembler::decode_cmd_ ## cmd ## _RL() {\
	decode_register(); \
	resultDescriptor.write("", ' '); \
	decode_number(); \
} \
void Disassembler::decode_cmd_ ## cmd ## _RSZ() {\
	decode_register(); \
	resultDescriptor.write("", ' '); \
	decode_static_data_label(); \
} \
void Disassembler::decode_cmd_ ## cmd ## _RR() {\
	decode_register(); \
	resultDescriptor.write("", ' '); \
	decode_register(); \
} \
void Disassembler::decode_cmd_ ## cmd ## _SR() {	\
	decode_register_star(); \
	resultDescriptor.write("", ' '); \
	decode_register(); \
} \
void Disassembler::decode_cmd_ ## cmd ## _RS() {	\
	decode_register(); \
	resultDescriptor.write("", ' '); \
	decode_register_star(); \
} \
void Disassembler::decode_cmd_ ## cmd ## _SS() {	\
	decode_register_star(); \
	resultDescriptor.write("", ' '); \
	decode_register_star(); \
}


LEFTWISE_REG_INSTRUCTION(ADDL, +=)
LEFTWISE_REG_INSTRUCTION(SUBL, -=)
LEFTWISE_REG_INSTRUCTION(MULL, *=)
LEFTWISE_REG_INSTRUCTION(MODL, %=)
LEFTWISE_REG_INSTRUCTION(MOVL, =)
LEFTWISE_REG_INSTRUCTION(ANDL, &=)
LEFTWISE_REG_INSTRUCTION(DIVL, /=)

#undef RR_LL_REG_INSTRUCTION


// ---  Section for first pass size calcs         --- //
// Needed because of poor assembler type design :...( //

void Disassembler::define_label() {
	size_t addr = extract_type<size_t>();
	if (labels_names.find(byteCodeBuffer + addr) == labels_names.end()) {
		labels_names.insert({ byteCodeBuffer + addr, "label_" + std::to_string(labels_names.size()) });
	}
}

void Disassembler::define_fun() {
	size_t addr = extract_type<size_t>();
	if (labels_names.find(byteCodeBuffer + addr) == labels_names.end()) {
		labels_names.insert({ byteCodeBuffer + addr, "fun_" + std::to_string(labels_names.size()) });
	}
}

void Disassembler::first_run_IN() {
}

void Disassembler::first_run_OUT() {
}

void Disassembler::first_run_OUTC() {
}

void Disassembler::first_run_PUSH() {
	total_size += sizeof(long long);
}

void Disassembler::first_run_POP() {
}

void Disassembler::first_run_PUSHR() {
	total_size += sizeof(char);
}

void Disassembler::first_run_POPR() {
	total_size += sizeof(char);
}

void Disassembler::first_run_MOV() {
	total_size += sizeof(char);
	total_size += sizeof(long long);
}

void Disassembler::first_run_MOVL_REG_STATIC_STAR() {
	total_size += sizeof(char);
	total_size += sizeof(size_t);
}

void Disassembler::first_run_CMP() {
}

void Disassembler::first_run_JMP() {
	define_label();
}

void Disassembler::first_run_JL() {
	define_label();
}

void Disassembler::first_run_JLE() {
	define_label();
}

void Disassembler::first_run_JEQ() {
	define_label();
}

void Disassembler::first_run_JNE() {
	define_label();
}


void Disassembler::first_run_JGE() {
	define_label();
}

void Disassembler::first_run_JG() {
	define_label();
}

void Disassembler::first_run_CALL() {
	define_fun();
}

void Disassembler::first_run_RET() {
}

void Disassembler::first_run_MEW() {
}

void Disassembler::first_run_MAX_CMD_NUM() {
}

#define ST_ARITHM_INST(cmd, op) \
void Disassembler::first_run_ ## cmd ## (){ \
}

ST_ARITHM_INST(ADD, +)
ST_ARITHM_INST(SUB, -)
ST_ARITHM_INST(MUL, *)
ST_ARITHM_INST(MOD, %)

#undef ST_ARITHM_INST

void Disassembler::first_run_DIV() {
}

#define LEFTWISE_REG_INSTRUCTION(cmd, op) \
void Disassembler::first_run_ ## cmd ## _RL() {\
	total_size += sizeof(char); \
	total_size += sizeof(long long); \
} \
void Disassembler::first_run_ ## cmd ## _RSZ() {\
	total_size += sizeof(char); \
	total_size += sizeof(size_t); \
} \
void Disassembler::first_run_ ## cmd ## _RR() {\
	total_size += sizeof(char); \
	total_size += sizeof(char); \
} \
void Disassembler::first_run_ ## cmd ## _SR() {	\
	total_size += sizeof(char); \
	total_size += sizeof(char); \
} \
void Disassembler::first_run_ ## cmd ## _RS() {	\
	total_size += sizeof(char); \
	total_size += sizeof(char); \
} \
void Disassembler::first_run_ ## cmd ## _SS() {	\
	total_size += sizeof(char); \
	total_size += sizeof(char); \
}


LEFTWISE_REG_INSTRUCTION(ADDL, +=)
LEFTWISE_REG_INSTRUCTION(SUBL, -=)
LEFTWISE_REG_INSTRUCTION(MULL, *=)
LEFTWISE_REG_INSTRUCTION(MODL, %=)
LEFTWISE_REG_INSTRUCTION(MOVL, =)
LEFTWISE_REG_INSTRUCTION(ANDL, &=)
LEFTWISE_REG_INSTRUCTION(DIVL, /=)

#undef RR_LL_REG_INSTRUCTION

