#include "cpu.h"


bool CPU::loadProgram(std::string* dataptr)
{
	if(dataptr->length()%32!=0)
	{
		std::cout<<"Error: One or more program instructions is the wrong length!"<<std::endl;
		return false;
	}
	mem.setCE(false);
	std::array<bool,8> temp;
	std::array<Byte,2> addresscounter={Byte(0),Byte(0)};
	for(int i = 0;i<(dataptr->length()/8);i++)
	{
		for(int j = 0;j<8;j++)
		{
			temp[j]= dataptr->at((i*8)+j)=='1'?1:0;
		}
		mem.placeData({addresscounter[0],addresscounter[1]});
		mem.loadMAR();
		mem.placeData({Byte(0),temp});
		mem.loadMDR();
		mem.removeData();
		mem.setWE(false);
		mem.setWE(true);
		
		addresscounter[1]=addresscounter[1]+1;
		if(addresscounter[1].readCarry())
		{
			addresscounter[1].setCarry(false);
			addresscounter[0]=addresscounter[0]+1;
		}
	}
	mem.setCE(true);
	return true;
}