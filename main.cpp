#include "cpu.h"



int main(int argc, char* argv[])
{
	
	CPU cpu;
	//std::cout<<"Welcome to Stafford King's processor emulation! Please type the file name (ending in .sbf) of the program you'd like to run! Or type \"exit\" to exit."<<std::endl;
	if(argc<2)
	{
		std::cout<<"Error: no input files"<<std::endl;
		return 0;
	}
	std::string filename(argv[1]);
	std::ifstream infile;
	int num_cycles=-1;
	infile.open(filename);
	if(!infile)
	{
		std::cout<<"Error: Program couldn't be read. Check the filename and try again."<<std::endl;
		return 0;
	}
	std::string cleaneddata="";
	std::size_t split;
	std::vector<int> regs;
	std::string temp;
	bool loop=false;
	/*
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
	*/
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
		if(argc>2)
		{
			std::string dcheck(argv[2]);//enable debug mode?
			if(dcheck == "-debug")
			{
				if(argc==3)
				{
					std::cout<<"Error: Debug mode selected but register range not specified"<<std::endl;
					return 0;
				}
				if(argc==5)
				{
					try
					{
						num_cycles=std::stoi(std::string(argv[4]));
					}
					catch(const std::invalid_argument& e)
					{
						std::cout<<"Error: Cycle numbers incorrectly specified."<<std::endl;
						return 0;
					}
				}
				
				std::string regnums(argv[3]);
				if(regnums.find(',')==std::string::npos)
				{
					if(regnums.find('-')==std::string::npos)
					{
						try
						{
							regs.push_back(std::stoi(regnums));
						}
						catch(const std::invalid_argument& e)
						{
							std::cout<<"Error 1: Register range specified in invalid format. Use , to separate values and - to indicate a range."<<std::endl;
							return 0;
						}
					}
					else
					{
						split = regnums.find('-');
						try
						{
							for(int i = std::stoi(regnums.substr(0,split));i<(std::stoi(regnums.substr(split+1)))+1;i++)
							{
								regs.push_back(i);
							}
						}
						catch(const std::invalid_argument& e)
						{
							std::cout<<"Error 2: Register range specified in invalid format. Use , to separate values and - to indicate a range."<<std::endl;
							return 0;
						}
					}
					cpu.execute(filename,regs,num_cycles);
				}
				else
				{
					while(true)
					{
						split=regnums.find(',');
						temp = regnums.substr(0,split);
						if(temp.find('-')!=std::string::npos)
						{
							try
							{
								for(int i = std::stoi(regnums.substr(0,temp.find('-')));i<(std::stoi(regnums.substr(temp.find('-')+1)))+1;i++)
								{
									regs.push_back(i);
								}
							}
							catch(const std::invalid_argument& e)
							{
								std::cout<<"Error 3: Register range specified in invalid format. Use , to separate values and - to indicate a range."<<std::endl;
								return 0;
							}
						}
						else
						{
							try
							{
								regs.push_back(std::stoi(temp));
							}
							catch(const std::invalid_argument& e)
							{
								std::cout<<"Error 4: Register range specified in invalid format. Use , to separate values and - to indicate a range."<<std::endl;
								return 0;
							}
						}
						regnums.erase(0,split+1);
						if(regnums.find(',')==std::string::npos)
						{
							if(loop)
							{
								break;
							}
							else
							{
								loop=true;
							}
						}
						
					}
					cpu.execute(filename,regs,num_cycles);
				}

			}
			else
			{
				cpu.execute();
			}
		}
		else
		{
			cpu.execute();
		}
	}
	
	return 0;
}