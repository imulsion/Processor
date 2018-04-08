#include "alu.h"
#include <fstream>

int main()
{
	ALU alu;
	std::cout<<"Welcome to Stafford King's processor emulation! Please type the file name (ending in .sbf) of the program you'd like to run!"<<std::endl;
	std::string filename;
	std::ifstream infile;
	std::array<bool,8> temp;
	char inchar;
	Byte lowmem(0),highmem(0); 
	while(true)
	{
		std::getline(std::cin,filename);
		if(filename=="exit")
		{
			return -1;
		}
		infile.open(filename);
		if(!infile)
		{
			std::cout<<std::endl;
			std::cout<<"Error: File is either unreadable or does not exist. Check that the filename is correct."<<std::endl;
			std::cout<<"Type the name again to retry, or type \"exit\" to exit."<<std::endl;
		}
		else
		{
			break;
		}
	}
	while(!infile.eof())
	{
		for(int i = 0;i<8;i++)
		{
			infile.get(inchar);
			temp[i] = (inchar=='1')?1:0;
		}
		alu.writeRAM({highmem,lowmem},temp);
		lowmem=lowmem+1;
		if(lowmem.readCarry())
		{
			lowmem.setCarry(false);
			highmem=highmem+1;
		}
	}
	//TODO file reading etc
	//after this point program loaded into memory
	
	alu.execute();
	
	return 0;
}