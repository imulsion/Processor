#include "memory.h"

Memory::Memory()
{
	mdr = Byte(0);
	mar = {Byte(0),Byte(0)};
	bus={{},{}};
	nCE=true;
	nWE=true;
}

void Memory::setCE(bool a)
{
	nCE=a;
	if(nCE)
	{
		mar={Byte(0),Byte(0)};
		bus={{},{}};
	}
	else
	{
		bus[1] = Byte(0);
		bus[0] = ram[mar[0].toInt(0)+mar[1].toInt(1)];
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
	mar=bus;
}

void Memory::loadMDR()
{
	mdr=bus;
}

void Memory::placeData(std::array<Byte,2> m)
{
	bus=m;
}

void Memory::removeData()
{
	bus = {{},{}};
}

const Byte& Memory::readMDR() const
{
	return mdr;
}

