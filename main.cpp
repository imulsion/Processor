#include "cpu.h"
#include <fstream>

int main()
{
	CPU cpu;
	std::cout<<"Welcome to Stafford King's processor emulation! Please type the file name (ending in .sbf) of the program you'd like to run!"<<std::endl;
	std::string filename;
	std::ifstream infile;

	while(true)
	{
		std::getline(std::cin,filename);
		if(filename=="exit")
		{
			return 0;
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
	std::string progdata((std::istreambuf_iterator<char>(infile)),(std::istreambuf_iterator<char>()));
	if(!cpu.loadProgram(&progdata))//use pointer for large files
	{
		return -1;
	}
	else
	{
		cpu.execute();
	}
	
	return 0;
}