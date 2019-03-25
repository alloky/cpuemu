#include "CpuemuAssembly.h"
#include "cpuemu.h"
#include "disassembler.h"

#include <iostream>

int main() {
	CpuemuAssembly assembler;
	assembler.compile("test_prog", ".", "out_bins");
	Disassembler disassembler;
	disassembler.Disassembly("out_bins/test_prog_" + std::string(assembler.version));
	CpuEmu emulator;
	emulator.execute("out_bins/test_prog_" + std::string(emulator.version) + ".vbc");
	return 0;
}