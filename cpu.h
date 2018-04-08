#ifndef CPU_H
#define CPU_H
#include "memory.h"
#include "byte.h"

class CPU : public Memory
{
	private:
		std::array<Byte,32> registers;//CPU general purpose registers
	public:
		void execute();
		
};

#endif //CPU_H