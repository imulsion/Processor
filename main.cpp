#include "cpu.hpp"



int main(int argc, char* argv[])
{
	
	CPU cpu;
	
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
				if(argc==3)//no registers specified
				{
					std::cout<<"Error: Debug mode selected but register range not specified"<<std::endl;
					return 0;
				}
				if(argc==5)//enable cycle limit
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
				//parse register selection string
				std::string regnums(argv[3]);
				if(regnums.find(',')==std::string::npos)//only one range
				{
					if(regnums.find('-')==std::string::npos)//no range, only a value
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
								regs.push_back(i);//loop through a range x-y starting at x and ending at y
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
						temp = regnums.substr(0,split);//substr all text up to the first comma
						//parse as usual...
						if(temp.find('-')!=std::string::npos)
						{
							try
							{
								for(int i = std::stoi(regnums.substr(0,temp.find('-')));i<(std::stoi(regnums.substr(temp.find('-')+1)))+1;i++)
								{
									regs.push_back(i);//loop through range
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
								regs.push_back(std::stoi(temp));//not a range, so just push back number
							}
							catch(const std::invalid_argument& e)
							{
								std::cout<<"Error 4: Register range specified in invalid format. Use , to separate values and - to indicate a range."<<std::endl;
								return 0;
							}
						}
						regnums.erase(0,split+1);//erase up to and including first comma
						if(regnums.find(',')==std::string::npos)//no commas left?
						{
							if(loop)//have we been here before?
							{
								break;//if yes, break as all values parsed
							}
							else
							{
								loop=true;//if no, loop once more to parse final value with no commas
							}
						}
						
					}
					cpu.execute(filename,regs,num_cycles);//all parsing done hooray, let's execute
				}

			}
			else//weird flag, probably spelled wrong
			{
				std::cout<<"Error: Unrecognised flag "+dcheck<<std::endl;
				return 0;
			}
		}
		else
		{
			cpu.execute();//execute without debug
		}
	}
	
	return 0;
}