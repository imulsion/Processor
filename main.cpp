#include "cpu.h"
#include <fstream>

int main()
{
	CPU cpu;
	std::cout<<"Welcome to Stafford King's processor emulation! Please type the file name (ending in .sbf) of the program you'd like to run! Or type \"exit\" to exit."<<std::endl;
	std::string filename;
	std::ifstream infile;
	std::string cleaneddata="";

	while(true)//loop input data
	{
		std::getline(std::cin,filename);
		if(filename=="exit")
		{
			return 0;
		}
		infile.open(filename);
		if(!infile)//file can't be opened...
		{
			std::cout<<std::endl;
			std::cout<<"Error: File is either unreadable or does not exist. Check that the filename is correct."<<std::endl;
			std::cout<<"Type the name again to retry, or type \"exit\" to exit."<<std::endl;
		}
		else
		{
			break;//file found, break
		}
	}
	std::string progdata((std::istreambuf_iterator<char>(infile)),(std::istreambuf_iterator<char>()));//iterate through filestream and place contents of file in string
	for(int i = 0;i<progdata.length();i++)
	{
		if((progdata.at(i)=='1')||(progdata.at(i)=='0'))//clean data of anything but 1s and 0s
		{
			cleaneddata+=progdata.at(i);
		}
	}
	if(!cpu.loadProgram(&cleaneddata))//load the program into memory, using pointer to prevent copying large strings
	{
		return -1;
	}
	else
	{
		cpu.execute();//run the program
	}
	
	return 0;
}