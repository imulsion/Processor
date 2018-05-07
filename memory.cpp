#include "memory.hpp"

Memory::Memory(): mdr(Byte(0)),mar({Byte(0)}),bus({}),nCE(true),nWE(true){}

//set chip enable
void Memory::setCE(bool a)
{
	nCE=a;
	if(!nCE&&nWE)
	{
		//if chip enable low and write enable high, read memory from address in MAR and place on memory bus
		bus[1] = ram[mar[0].toInt(0)+mar[1].toInt(1)];
	}
}

//set write enable
void Memory::setWE(bool a)
{
	nWE=a;
	if(!nWE&&!nCE)
	{
		//if write enable low, write data in MDR to address stored in MAR
		ram[mar[0].toInt(0)+mar[1].toInt(1)] = mdr;
	}
}

//load memory data register
void Memory::loadMAR()
{
	if(bus[0].has_value()&&bus[1].has_value())//if bus not in high Z mode
	{
		mar[0]=bus[0].value();
		mar[1]=bus[1].value();//load MAR from bus
		if(!nCE&&nWE)
		{
			//update bus with read info if necessary
			bus[1] = ram[mar[0].toInt(0)+mar[1].toInt(1)];
		}
	}
	else//shouldnt try to access bus in high Z
	{
		std::cout<<"Warning: Memory access violation: MAR attempted load from bus partially or fully in Z-mode! Incorrect signal path?"<<std::endl;
	}
}

//load MDR
void Memory::loadMDR()
{
	if(bus[1].has_value())
	{
		mdr=bus[1].value();//load lower byte of bus into MDR
	}
	else//bus accessed in high Z mode
	{
		std::cout<<"Warning: Memory access violation: MDR attempted load from bus in Z-mode! Incorrect signal path?"<<std::endl;
		mdr = Byte(0);
	}
}

//place data on bus
void Memory::placeData(const std::array<Byte,2> m)
{
	bus[0]=m[0];
	bus[1]=m[1];
}

//clear bus: set to high Z mode
void Memory::removeData()
{
	bus[0]={};
	bus[1]={};
}

//read data in MDR
const Byte& Memory::readMDR() const
{
	return mdr;
}
/* DEBUG ONLY FUNCTIONS
const std::array<Byte,2>& Memory::readMAR() const
{
	return mar;
}

void Memory::readSomeData(int i) const
{
	ram[i].printData();
}
*/