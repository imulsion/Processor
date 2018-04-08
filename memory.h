#ifndef MEMORY_H
#define MEMORY_H

#include <array>
#include <optional>
#include "byte.h"
class Memory
{
	private:
	/*
		Memory structure as follows:
		-Addresses 0-2047 are program memory
		-Addresses 2048-2049 are the program counter
		-Addresses 2050-8190 are general registers
		-Address 8191 is the status register
		-status register: Format ZXCV: Zero[0],sign[1],carry[2],overflow[3]. Bits 4 through 7 are unused
	*/
		std::array<Byte,8192> ram;//Main RAM
		std::array<Byte,2> mar;//Memory address register
		Byte mdr;//Memory data register
		std::array<std::optional<Byte>,2> bus;//memory interface bus
		bool nCE;//~chip enable
		bool nWE;//~write enable
		bool 
	public:
		Memory();
		void setCE(bool);
		void setWE(bool);
		void loadMAR();
		void loadMDR();
		void placeData(std::array<Byte,2>);
		void removeData();
		const Byte& readDatabus() const;
		const Byte& readMDR() const;
		
};
#endif //MEMORY_H
		
	