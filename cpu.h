#ifndef CPU_H
#define CPU_H
#include "memory.h"
#include "byte.h"
#include <stack>

class CPU
{
	private:
		std::array<Byte,32> registers;//CPU general purpose registers. registers[31] is the status register - DO NOT OVERWRITE
		//status register: Format ZXCV: Zero[0],sign[1],carry[2]. Bits 3 through 7 are unused
		
		Memory mem;//RAM module
		std::array<Byte,2> pc;//Program counter
		std::stack<std::array<Byte,2>> stack;//call stack for branching
		
	public:
		CPU();
		const bool loadProgram(std::string*);
		bool execute();
		void updateSREG(Byte*,Byte*,bool);
		void clearSREG();
		std::array<Byte,2> convertToWordAddress(std::array<Byte,2>);
		std::array<Byte,2> create16(int) const;
};

#endif //CPU_H