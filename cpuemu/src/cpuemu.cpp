#pragma once

#include "cpuemu.h"
#include "logging.h"
#include "CodeLoader.h"

CpuEmu::CpuEmu()
{
	LEV_LOG(LL_DEBUG, "//--------------CpuEmu VM started------------------//");
	LEV_LOG(LL_DEBUG, "                version: " + version);
}

CpuEmu::~CpuEmu()
{
}

void CpuEmu::IN(){
	std::cin >> rps[0];
	st.push<long long>(rps[0]);
	++instruction_pointer;
}

void CpuEmu::OUT() {
	rps[0] = st.pop<long long>();
	std::cout << rps[0] << std::endl;
	++instruction_pointer;
}

void CpuEmu::OUTC() {
	rps[0] = st.pop<long long>();
	std::cout << static_cast<char>(rps[0]);
	++instruction_pointer;
}

void CpuEmu::PUSH(){
	++instruction_pointer;
	long long val = extract_type<long long>();
	st.push<long long>(val);
}

void CpuEmu::POP(){
	st.pop<long long>();
	++instruction_pointer;
}

void CpuEmu::PUSHR() {
	++instruction_pointer;
	char r_ind = extract_type<char>();
	st.push<long long>(regs[r_ind]);
}

void CpuEmu::POPR() {
	++instruction_pointer;
	char r_ind = extract_type<char>();
	regs[r_ind] = st.pop<long long>();
}

void CpuEmu::MOV(){
	++instruction_pointer;
	char r_ind = extract_type<char>();
	long long val = extract_type<long long>();
	regs[r_ind] = val;
}

void CpuEmu::MOVL_REG_STATIC_STAR() {
	++instruction_pointer;
	char r_ind = extract_type<char>();
	size_t addr = extract_type<size_t>();
	regs[r_ind] = addr;
}


/*
void CpuEmu::SQRT() {
	++instruction_pointer;
	char reg = extract_type<char>();
	regs[reg] = sqrt(regs[reg]);
}
*/

void CpuEmu::CMP() {
	rps[0] = st.pop<long long>();
	rps[1] = st.pop<long long>();
	rps[2] = 0;
	if (rps[0] < rps[1]) {
		rps[2] = 2;
	}
	if (rps[0] > rps[1]) {
		rps[2] = 1;
	}
	st.push<long long>(rps[1]);
	st.push<long long>(rps[0]);
	++instruction_pointer;
}

void CpuEmu::JMP(){
	++instruction_pointer;
	size_t addr = extract_type<size_t>();
	instruction_pointer = code + addr;
}

void CpuEmu::JL(){
	++instruction_pointer;
	size_t addr = extract_type<size_t>();
	rps[0] = rps[2] & 3;
	if( rps[0] == 1 ){
		instruction_pointer = code + addr;
	}
}

void CpuEmu::JLE(){
	++instruction_pointer;
	size_t addr = extract_type<size_t>();
	rps[0] = rps[2] & 3;
	if(rps[0] == 1 || rps[0] == 0){
		instruction_pointer = code + addr;
	}
}

void CpuEmu::JEQ(){
	++instruction_pointer;
	size_t addr = extract_type<size_t>();
	rps[0] = rps[2] & 3;
	if(rps[0] == 0){
		instruction_pointer = code + addr;
	}
}

void CpuEmu::JNE() {
	++instruction_pointer;
	size_t addr = extract_type<size_t>();
	rps[0] = rps[2] & 3;
	if (rps[0] != 0) {
		instruction_pointer = code + addr;
	}
}


void CpuEmu::JGE(){
	++instruction_pointer;
	size_t addr = extract_type<size_t>();
	rps[0] = rps[2] & 3;
	if(rps[0] == 2 || rps[0] == 0){
		instruction_pointer = code + addr;
	}
}



void CpuEmu::JG(){
	++instruction_pointer;
	size_t addr = extract_type<size_t>();
	rps[0] = rps[2] & 3;
	if(rps[0] == 2){
		instruction_pointer = code + addr;
	}
}

/*
void CpuEmu::SHOT() {
	int type = rand() % 2;
	if (type) {
		*((char*)&st + rand() % sizeof(st)) = rand() % 1000;
	}
	else {
		regs[rand() % 132] = rand() % 1000;
	}
	++instruction_pointer;
}
*/

void CpuEmu::CALL() {
	st.push<long long>(reinterpret_cast<long long>(instruction_pointer + 1 + sizeof(size_t)));
	JMP();
}

void CpuEmu::RET() {
	instruction_pointer = reinterpret_cast<char*>(st.pop<long long>());
}

#define ST_ARITHM_INST(cmd, op) \
void CpuEmu:: ## cmd ## (){ \
	++instruction_pointer; \
	rps[0] = st.pop<long long>(); \
	rps[1] = st.pop<long long>(); \
	rps[0] = rps[0] op rps[1]; \
	st.push<long long>(rps[0]); \
}

ST_ARITHM_INST(ADD, +)
ST_ARITHM_INST(SUB, -)
ST_ARITHM_INST(MUL, *)
ST_ARITHM_INST(MOD, %)

#undef ST_ARITHM_INST

void CpuEmu::DIV(){
	++instruction_pointer;
	rps[0] = st.pop<long long>();
	rps[1] = st.pop<long long>();
	if (rps[1] == 0) {
		LEV_LOG(LL_ERR, "Zero divison occured on instruction " << instruction_pointer);
		throw std::exception("zero division occured");	
	}
	rps[0] = rps[0] / rps[1];
	st.push<long long>(rps[0]);
}

#define LEFTWISE_REG_INSTRUCTION(cmd, op) \
void CpuEmu:: ## cmd ## _RL() {\
	++instruction_pointer; \
	char reg_num = extract_type<char>(); \
	long long val = extract_type<long long>(); \
	regs[reg_num] op val; \
} \
void CpuEmu:: ## cmd ## _RSZ() {\
	++instruction_pointer; \
	char reg_num = extract_type<char>(); \
	size_t val = extract_type<size_t>(); \
	regs[reg_num] op val; \
} \
void CpuEmu:: ## cmd ## _RR() {\
	++instruction_pointer; \
	char reg_num_l = extract_type<char>(); \
	char reg_num_r = extract_type<char>(); \
	regs[reg_num_l] op regs[reg_num_r]; \
} \
void CpuEmu:: ## cmd ## _SR() {	\
	++instruction_pointer; \
	char reg_num_l = extract_type<char>(); \
	char reg_num_r = extract_type<char>(); \
	code[regs[reg_num_l]] op regs[reg_num_r]; \
} \
void CpuEmu:: ## cmd ## _RS() {	\
	++instruction_pointer; \
	char reg_num_l = extract_type<char>(); \
	char reg_num_r = extract_type<char>(); \
	regs[reg_num_l] op code[regs[reg_num_r]]; \
} \
void CpuEmu:: ## cmd ## _SS() {	\
	++instruction_pointer; \
	char reg_num_l = extract_type<char>(); \
	char reg_num_r = extract_type<char>(); \
	code[regs[reg_num_l]] op code[regs[reg_num_r]]; \
}


LEFTWISE_REG_INSTRUCTION(ADDL, += )
LEFTWISE_REG_INSTRUCTION(SUBL, -= )
LEFTWISE_REG_INSTRUCTION(MULL, *= )
LEFTWISE_REG_INSTRUCTION(MODL, %= )
LEFTWISE_REG_INSTRUCTION(MOVL, =  )
LEFTWISE_REG_INSTRUCTION(ANDL, &= )

#undef RR_LL_REG_INSTRUCTION

void CpuEmu::DIVL_RL() {
	++instruction_pointer;
	char reg_num_l = extract_type<char>();
	long long val = extract_type<long long>();
	if (val == 0) {
		LEV_LOG(LL_ERR, "Zero divison occured on instruction " << instruction_pointer);
		throw std::exception("zero division occured");
	}
	regs[reg_num_l] /= val;
}

void CpuEmu::DIVL_RSZ() {
	++instruction_pointer;
	char reg_num_l = extract_type<char>();
	size_t val = extract_type<size_t>();
	if (val == 0) {
		LEV_LOG(LL_ERR, "Zero divison occured on instruction " << instruction_pointer);
		throw std::exception("zero division occured");
	}
	regs[reg_num_l] /= val;
}

void CpuEmu::DIVL_RR() {
	++instruction_pointer;
	char reg_num_l = extract_type<char>();
	char reg_num_r = extract_type<char>();
	if (regs[reg_num_r] == 0) {
		LEV_LOG(LL_ERR, "Zero divison occured on instruction " << instruction_pointer);
		throw std::exception("zero division occured");
	}
	regs[reg_num_l] /= regs[reg_num_r];
}

void CpuEmu::DIVL_RS() {
	++instruction_pointer;
	char reg_num_l = extract_type<char>();
	char reg_num_r = extract_type<char>();
	if (regs[reg_num_r] == 0) {
		LEV_LOG(LL_ERR, "Zero divison occured on instruction " << instruction_pointer);
		throw std::exception("zero division occured");
	}
	regs[reg_num_l] /= data[regs[reg_num_r]];
}

void CpuEmu::DIVL_SR() {
	++instruction_pointer;
	char reg_num_l = extract_type<char>();
	char reg_num_r = extract_type<char>();
	if (regs[reg_num_r] == 0) {
		LEV_LOG(LL_ERR, "Zero divison occured on instruction " << instruction_pointer);
		throw std::exception("zero division occured");
	}
	data[regs[reg_num_l]] /= regs[reg_num_r];
}


void CpuEmu::DIVL_SS() {
	char reg_num_l = extract_type<char>();
	char reg_num_r = extract_type<char>();
	if (regs[reg_num_r] == 0) {
		LEV_LOG(LL_ERR, "Zero divison occured on instruction " << instruction_pointer);
		throw std::exception("zero division occured");
	}
	data[regs[reg_num_l]] /= data[regs[reg_num_r]];
}


void CpuEmu::execute(const std::string& path) {

#define DEF_CMD(CMD, WORD, COND, PARSE_CODE) \
		LEV_LOG(LL_DEBUG, WORD << " "  << CPUE_CMD_NUM::CMD);
#include "cpuemu-cmd-defs.h"
#undef DEF_CMD

	CodeLoader cl;
	cl.load_binary(path.c_str());
	
	code = cl.buff;
	code_segment = cl.buff + cl.signature.code_segment_addr;
	data = cl.buff + cl.signature.data_segment_addr;
	st.SetStart(cl.buff + cl.signature.stack_segment_addr);
	st.SetSize(cl.signature.stack_size);

	instruction_pointer = code_segment;
	code_size = cl.size;
	exec_loop();
}

void CpuEmu::exec_loop(){
	auto debug_string = std::string(code, code_size);
	st.push<char*>(code);
	if (st.pop<char*>() != code) {
		exit(0);
	}
	while(true){
		process_instruction();
		if (code + code_size <= instruction_pointer) break;
	}
	LEV_LOG(LL_INFO, "Execution ended.");
}

void CpuEmu::process_instruction(){
	//LEV_LOG(LL_DEBUG, WORD << CPUE_CMD_NUM::CMD); 
	switch (*instruction_pointer) {
		// Codegeneration for cmd processing
#define DEF_CMD(CMD, WORD, COND, PARSE_CODE) \
	case CPUE_CMD_NUM:: ## CMD : \
		if(true) { CMD(); } \
		break;
#include "cpuemu-cmd-defs.h"
#undef DEF_CMD
		default:
			LEV_LOG(LL_ERR, "Unknown command found on addr");
			break;
	}
}

template <class T>
T CpuEmu::extract_type() {
	T ret = *((T*)instruction_pointer);
	instruction_pointer += sizeof(T);
	return ret;
}


#include <windows.h>

void CpuEmu::MEW() {
	++instruction_pointer;
	std::cout << std::endl;
	std::cout << __zZz_CAT_zZz__;
	std::cout << std::endl;
	PlaySound(TEXT("meow.wav"), NULL, SND_FILENAME | SND_ASYNC);
}

void CpuEmu::MAX_CMD_NUM() {

}