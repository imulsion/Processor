#include "memory.h"

Memory::Memory()
{
	mdr = Byte(0);
	mar = {Byte(0),Byte(0)};
	bus[0]={};
	bus[1]={};
	nCE=true;
	nWE=true;
}

void Memory::setCE(bool a)
{
	nCE=a;
	if(!nCE&&nWE)
	{
		bus[1] = ram[mar[0].toInt(0)+mar[1].toInt(1)];
	}
}

void Memory::setWE(bool a)
{
	nWE=a;
	if(!nWE&&!nCE)
	{
		ram[mar[0].toInt(0)+mar[1].toInt(1)] = mdr;
	}
}

void Memory::loadMAR()
{
	if(bus[0].has_value()&&bus[1].has_value())
	{
		mar[0]=bus[0].value();
		mar[1]=bus[1].value();
		if(!nCE&&nWE)
		{
			bus[1] = ram[mar[0].toInt(0)+mar[1].toInt(1)];
		}
	}
	else
	{
		std::cout<<"Warning: Memory access violation: MAR attempted load from bus partially or fully in Z-mode! Incorrect signal path?"<<std::endl;
	}
}

void Memory::loadMDR()
{
	if(bus[1].has_value())
	{
		mdr=bus[1].value();
	}
	else
	{
		std::cout<<"Warning: Memory access violation: MDR attempted load from bus in Z-mode! Incorrect signal path?"<<std::endl;
		mdr = Byte(0);
	}
}

void Memory::placeData(std::array<Byte,2> m)
{
	bus[0]=m[0];
	bus[1]=m[1];
}

void Memory::removeData()
{
	bus[0]={};
	bus[1]={};
}

const Byte& Memory::readMDR() const
{
	return mdr;
}
/* DEBUG ONLY FUNCTION
const std::array<Byte,2>& Memory::readMAR() const
{
	return mar;
}
*/
void Memory::readSomeData(int i) const
{
	ram[i].printData();
}
