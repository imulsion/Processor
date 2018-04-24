#ifndef MEMORY_H
#define MEMORY_H

#include <array>
#include <optional>//C++ 17 feature - will NOT compile without C++ 17 support
#include "byte.h"
class Memory
{
	private:
	/*
		Memory structure as follows:
		-Addresses 0-2047 are program memory
		-Addresses 2048-8191 are general memory
	*/
		std::array<Byte,8192> ram;//Main RAM
		std::array<Byte,2> mar;//Memory address register
		Byte mdr;//Memory data register
		std::array<std::optional<Byte>,2> bus;//memory interface bus
		bool nCE;//~chip enable
		bool nWE;//~write enable

	public:
		Memory();//constructor - init all variables
		//setters
		void setCE(bool);
		void setWE(bool);
		//load memory registers
		void loadMAR();
		void loadMDR();
		
		//interact with memory bus
		void placeData(std::array<Byte,2>);
		void removeData();
		//read memory data register
		const Byte& readMDR() const;

		/* 
		Debug only functions - allow direct reading of SRAM. Used only for debugging purposes, do not emulate hardware
		const std::array<Byte,2>& readMAR() const;//ONLY use for debugging!
		void readSomeData(int) const;//ONLY use for debugging!
		*/
		
};
#endif //MEMORY_H
		
	