#ifndef CPU_H
#define CPU_H
#include "memory.h"
#include "byte.h"

class CPU
{
	private:
		std::array<Byte,32> registers;//CPU general purpose registers
		Memory mem;//RAM module
		Byte status;//status register: Format ZXCV: Zero[0],sign[1],carry[2],overflow[3]. Bits 4 through 7 are unused
		
	public:
		bool loadProgram(std::string*);
};

#endif //CPU_H