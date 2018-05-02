#ifndef CPU_H//header guard
#define CPU_H
#include "memory.h"
#include "byte.h"
#include <stack>
#include <vector>
#include <algorithm>
#include <fstream>

class CPU
{
	private:
		std::array<Byte,32> registers;//CPU general purpose registers. registers[31] is the status register - DO NOT OVERWRITE
		//status register: Format ZXCV: Zero[0],sign[1],carry[2]. Bits 3 through 7 are unused
		
		Memory mem;//RAM module
		std::array<Byte,2> pc;//Program counter
		std::stack<std::array<Byte,2>> stack;//call stack for branching
		int cyclecount;
		
	public:
		CPU();//constructor
		const bool loadProgram(std::string*);//load machine code file to memory
		const bool execute(std::string fname="",std::optional<std::vector<int>> regnums={},int num_cycles=-1);//execute stored program
		//update/clear status register - called once per cycle
		void updateSREG(Byte*,Byte*,bool);
		void clearSREG();
		std::array<Byte,2> convertToWordAddress(std::array<Byte,2>);//function to convert PC to a word address
};

#endif //CPU_H