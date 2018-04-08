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
		-Addresses 2048-8191 are general registers
	*/
		std::array<Byte,8192> ram;//Main RAM
		std::array<Byte,2> mar;//Memory address register
		Byte mdr;//Memory data register
		std::array<std::optional<Byte>,2> bus;//memory interface bus
		bool nCE;//~chip enable
		bool nWE;//~write enable

	public:
		Memory();
		void setCE(bool);
		void setWE(bool);
		void loadMAR();
		void loadMDR();
		void placeData(std::array<Byte,2>);
		void removeData();
		const Byte& readMDR() const;
		//const std::array<Byte,2>& readMAR() const;//ONLY use for debugging!
		
};
#endif //MEMORY_H
		
	