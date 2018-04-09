#ifndef CPU_H
#define CPU_H
#include "memory.h"
#include "byte.h"

class CPU
{
	private:
		std::array<Byte,32> registers;//CPU general purpose registers
		Memory mem;//RAM module
		Byte status;//status register: Format ZXCV: Zero[0],sign[1],carry[2]. Bits 3 through 7 are unused
		std::array<Byte,2> pc;//Program counter
		
	public:
		CPU();
		const bool loadProgram(std::string*);
		bool execute();
		void updateSREG(Byte*);
};

#endif //CPU_H