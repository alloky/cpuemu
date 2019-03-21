#include <string>

#include "stack.h"
#include "cpuemu-cmds.h"

/**
 * @brief      Class for cpu emulation
 *
 *
 *             CpuEmu - class for emulating preusdo x86 cpu unite. Has it's own
 *             stack, data segment (just raw pointer to allocated memory) and
 *             registers.
 *
 *             Author: Ivanin Vitaly aka alloky e-mail: alloky2011@gmail.com
 *
 *             version: 0.0.2 (untested)
 */
class CpuEmu {
	/*
	 * @brief user registers
	 * 
	 * Array with long long registers
	 * Supposed to be used by user in programms
	 * 
	*/
	long long regs[16];

	/*
	 *	@brief system registers
	 *	
	 *	Private registers used by processor
	 *	
	 *	rps[0] : tmp regiter
	 *	rps[1] : tmp2 regiter
	 *	rps[2] : flag register
	 *
	*/
	long long rps[16];

	/*
	 * @brief pointer to heap segment
	 * 
	 * Unused in this version
	*/
	char* heap{nullptr};
	
	/*
	 * @brief pointer to data segment
	 * 
	 * Allocated with constructer. May be used by user for data store
	*/
	char* data{nullptr};

	/*
	 * @brief stack for push pop operations
	*/
	Stack st;

	/*
	 * @brief size of data segment
	*/
	size_t DATA_SIZE;	

public:

	CpuEmu(); //!< constructor
	~CpuEmu(); //!< destructor

	/*
	 * @brief      execute binary code
	 *
	 *
	 *             Code should be in file lied in path
	 *
	 * @param[in]  path  Path to binary file
	 */
	void execute(const std::string& path);

	std::string version = "0.0.2";

private:

	char* code{ nullptr };
	size_t code_size{ 0 };
	char* instruction_pointer{ 0 };
	char* byte_code, *code_segment;

	void exec_loop();
	void process_instruction();

	template <class T>
	T extract_type();


	/*
	 *	Arithmetic instructions
	 *	
	*/
#define DEF_CMD(CMD, WORD, COND, PARSE_CODE) \
void CMD ## (); \

#include "cpuemu-cmd-defs.h"
#undef DEF_CMD

};

