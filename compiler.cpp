#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <cmath>

std::string decodeArg(std::string,bool);
std::string writeLine(std::string,int,bool,int);

int main(int argc, char* argv[])
{
	if(argc<2)
	{
		std::cout<<"Error: No input files"<<std::endl;
		return 0;
	}
	std::string line;
	std::string result = "";
	std::string instruction;
	std::string args;
	std::size_t split;
	std::string errchk="";
	std::string incpy = argv[1];
	std::string outname = incpy.erase(incpy.find('.'))+".sbf";
	int linecount=1;
	int instructioncount=0;
	std::vector<std::string> labels;
	std::vector<int> labelnumbers;
	std::ifstream reader{std::string(argv[1])};
	if(!reader)
	{
		std::cout<<"Error: Input file not found"<<std::endl;
		return 0;
	}
	while(std::getline(reader,line))
	{
		if(line=="")
		{
			continue;
		}
		if(line.at(0)==';')
		{
			continue;
		}
		if(line.find(':')!=std::string::npos)
		{
			labels.push_back(line.substr(0,line.find(':')));
			labelnumbers.push_back(instructioncount);
			line=line.substr(line.find(':')+1);
			if(line=="")
			{
				continue;
			}
			if(line.find(';')!=std::string::npos)
			{
				line.erase(line.find(';'));
			}
			if(line=="")
			{
				continue;
			}
		}
		instructioncount++;
	}
	
	reader.clear();
	reader.seekg(0,reader.beg);
	while(std::getline(reader,line))
	{
		if(line.empty())
		{
			linecount++;
			continue;
		}
		for(int i = 0;i<line.length();i++)
		{
			if(line.at(i)==';')
			{
				line.erase(i);
				break;
			}
		}
		if(line.empty())
		{
			linecount++;
			continue;
		}
		split=line.find(':');
		if(split!=std::string::npos)
		{
			line=line.substr(split+1);
		}
		if(line.empty())
		{
			linecount++;
			continue;
		}
		split = line.find(' ');
		if(split==std::string::npos)
		{
			instruction=line;
			args="";
		}
		else
		{
			instruction = line.substr(0,split);
			args = line.substr(split);
		}
		//remove whitespace
		std::string::iterator end_pos = std::remove(args.begin(), args.end(), ' ');
		args.erase(end_pos, args.end());
		
		if(instruction=="MOV")
		{
			result+="000000";
			errchk = writeLine(args,2,true,linecount);
			if(errchk=="")
			{
				return 0;
			}
			else
			{
				result+=errchk;
			}
		}
		else if(instruction=="MOVI")
		{
			result+="000001";
			errchk = writeLine(args,2,false,linecount);
			if(errchk=="")
			{
				return 0;
			}
			else
			{
				result+=errchk;
			}
		}
		else if(instruction=="AND")
		{
			result+="000010";
			errchk = writeLine(args,2,true,linecount);
			if(errchk=="")
			{
				return 0;
			}
			else
			{
				result+=errchk;
			}
		}
		else if(instruction=="OR")
		{
			result+="000011";
			errchk = writeLine(args,2,true,linecount);
			if(errchk=="")
			{
				return 0;
			}
			else
			{
				result+=errchk;
			}
		}
		else if(instruction=="NOT")
		{
			result+="000100";
			errchk = writeLine(args,1,true,linecount);
			if(errchk=="")
			{
				return 0;
			}
			else
			{
				result+=errchk;
			}
		}
		else if(instruction=="RET")
		{
			result+="00010100000000000000000000000000";
		}
		else if(instruction=="ADD")
		{
			result+="000110";
			errchk = writeLine(args,2,true,linecount);
			if(errchk=="")
			{
				return 0;
			}
			else
			{
				result+=errchk;
			}
		}
		else if(instruction=="ADDI")
		{
			result+="000111";
			errchk = writeLine(args,2,false,linecount);
			if(errchk=="")
			{
				return 0;
			}
			else
			{
				result+=errchk;
			}
		}
		else if(instruction=="SUB")
		{
			result+="001000";
			errchk = writeLine(args,2,true,linecount);
			if(errchk=="")
			{
				return 0;
			}
			else
			{
				result+=errchk;
			}
		}
		else if(instruction=="SUBI")
		{
			result+="001001";
			errchk = writeLine(args,2,false,linecount);
			if(errchk=="")
			{
				return 0;
			}
			else
			{
				result+=errchk;
			}
		}
		else if(instruction=="MUL")
		{
			result+="001010";
			errchk = writeLine(args,2,true,linecount);
			if(errchk=="")
			{
				return 0;
			}
			else
			{
				result+=errchk;
			}
		}
		else if(instruction=="DIV")
		{
			result+="001011";
			errchk = writeLine(args,2,true,linecount);
			if(errchk=="")
			{
				return 0;
			}
			else
			{
				result+=errchk;
			}
		}
		else if(instruction=="JMP")
		{
			result+="00110000000";
			bool matchfound = false;
			for(int i = 0;i<labels.size();i++)
			{
				if(labels[i]==args)
				{
					matchfound=true;
					int num = labelnumbers[i];
					for(int i = 0;i<8;i++)
					{
						if(num<pow(2,7-i))
						{
							result+="0";
						}
						else
						{
							result+="1";
							num-=pow(2,7-i);
						}
					}
					result+="0000000000000";
					break;
				}
			}
			if(!matchfound)
			{
				std::cout<<"Error on line "<<linecount<<": No match found for label"<<std::endl;
			}
			
		}
		else if(instruction=="CP")
		{
			result+="001101";
			errchk = writeLine(args,2,true,linecount);
			if(errchk=="")
			{
				return 0;
			}
			else
			{
				result+=errchk;
			}
		}
		else if(instruction=="CPI")
		{
			result+="001110";
			errchk = writeLine(args,2,false,linecount);
			if(errchk=="")
			{
				return 0;
			}
			else
			{
				result+=errchk;
			}
		}
		else if(instruction=="LOAD")
		{
			result+="001111";
			std::size_t split = args.find(',');
			std::string arg1,arg2,argtemp;
			if(split==std::string::npos)
			{
				std::cout<<"Error on line "<<linecount<<": Incorrect number of arguments supplied to instruction, two required"<<std::endl;			
				return 0;
			}
			arg1=args.substr(0,split);
			argtemp = decodeArg(arg1,true);
			if(argtemp=="")
			{
				std::cout<<"Error on line "<<linecount<<": Incorrect number of arguments supplied to instruction."<<std::endl;
				return 0;
			}
			result+="00000";
			result+=argtemp;
			arg2=args.substr(split+1);
			if(arg2.at(0)!='%')
			{
				std::cout<<"Error on line "<<linecount<<": Invalid address supplied to LOAD instruction"<<std::endl;				
				return 0;
			}
			int address;
			try
			{
				address = std::stoi(arg2.substr(1));
			}
			catch(const std::invalid_argument& e)
			{
				std::cerr<<"Error on line "<<linecount<<": Invalid address supplied to LOAD instruction"<<std::endl;
				return 0;
			}
			for(int i = 0;i<13;i++)
			{
				if(address<pow(2,12-i))
				{
					result+="0";
				}
				else
				{
					result+="1";
					address-=pow(2,7-i);
				}
			}
		}
		else if(instruction=="STORE")
		{
			result+="010000";
			std::size_t split = args.find(',');
			std::string arg1,arg2,argtemp;
						
			arg1=args.substr(0,split);
			arg2=args.substr(split+1);
			if(arg1.at(0)!='%')
			{
				std::cout<<"Error on line "<<linecount<<": Invalid address supplied to STORE instruction"<<std::endl;				
				return 0;
			}
			
			int address;
			try
			{
				address = std::stoi(arg1.substr(1));
			}
			catch(const std::invalid_argument& e)
			{
				std::cerr<<"Error on line "<<linecount<<": Invalid address supplied to STORE instruction"<<std::endl;
				return 0;
			}
			for(int i = 0;i<13;i++)
			{
				if(address<pow(2,12-i))
				{
					result+="0";
				}
				else
				{
					result+="1";
					address-=pow(2,7-i);
				}
			}
			
			if(split==std::string::npos)
			{
				std::cout<<"Error on line "<<linecount<<": Incorrect number of arguments supplied to instruction, two required"<<std::endl;			
				return 0;
			}
			//DEBUG
			std::cout<<"Arg2: "<<arg2<<std::endl;
			//END
			argtemp = decodeArg(arg2,true);
			if(argtemp=="")
			{
				std::cout<<"Error on line "<<linecount<<": Incorrect number of arguments supplied to instruction."<<std::endl;
				return 0;
			}
			result+="00000";
			result+=argtemp;
			
		}
		else if(instruction=="BNE")
		{
			result+="01000100000";
			bool matchfound = false;
			for(int i = 0;i<labels.size();i++)
			{
				if(labels[i]==args)
				{
					matchfound=true;
					int num = labelnumbers[i];
					for(int i = 0;i<8;i++)
					{
						if(num<pow(2,7-i))
						{
							result+="0";
						}
						else
						{
							result+="1";
							num-=pow(2,7-i);
						}
					}
					result+="0000000000000";
					break;
				}
			}
			if(!matchfound)
			{
				std::cout<<"Error on line "<<linecount<<": No match found for label"<<std::endl;
			}
			
		}
		else if(instruction=="BREQ")
		{
			result+="01001000000";
			bool matchfound = false;
			for(int i = 0;i<labels.size();i++)
			{
				if(labels[i]==args)
				{
					matchfound=true;
					int num = labelnumbers[i];
					for(int i = 0;i<8;i++)
					{
						if(num<pow(2,7-i))
						{
							result+="0";
						}
						else
						{
							result+="1";
							num-=pow(2,7-i);
						}
					}
					result+="0000000000000";
					break;
				}
			}
			if(!matchfound)
			{
				std::cout<<"Error on line "<<linecount<<": No match found for label"<<std::endl;
			}
			
		}
		else if(instruction=="BRSN")
		{
			result+="01001100000";
			bool matchfound = false;
			for(int i = 0;i<labels.size();i++)
			{
				if(labels[i]==args)
				{
					matchfound=true;
					int num = labelnumbers[i];
					for(int i = 0;i<8;i++)
					{
						if(num<pow(2,7-i))
						{
							result+="0";
						}
						else
						{
							result+="1";
							num-=pow(2,7-i);
						}
					}
					result+="0000000000000";
					break;
				}
			}
			if(!matchfound)
			{
				std::cout<<"Error on line "<<linecount<<": No match found for label"<<std::endl;
			}
			
		}
		else if(instruction=="BRSS")
		{
			result+="01010000000";
			bool matchfound = false;
			for(int i = 0;i<labels.size();i++)
			{
				if(labels[i]==args)
				{
					matchfound=true;
					int num = labelnumbers[i];
					for(int i = 0;i<8;i++)
					{
						if(num<pow(2,7-i))
						{
							result+="0";
						}
						else
						{
							result+="1";
							num-=pow(2,7-i);
						}
					}
					result+="0000000000000";
					break;
				}
			}
			if(!matchfound)
			{
				std::cout<<"Error on line "<<linecount<<": No match found for label"<<std::endl;
			}
			
		}
		else if(instruction=="INC")
		{
			result+="010101";
			errchk = writeLine(args,1,true,linecount);
			if(errchk=="")
			{
				return 0;
			}
			else
			{
				result+=errchk;
			}
		}
		else if(instruction=="DEC")
		{
			result+="010110";
			errchk = writeLine(args,1,true,linecount);
			if(errchk=="")
			{
				return 0;
			}
			else
			{
				result+=errchk;
			}
		}
		else if(instruction=="ADDL")
		{
			result+="010111";
			errchk = writeLine(args,2,true,linecount);
			if(errchk=="")
			{
				return 0;
			}
			else
			{
				result+=errchk;
			}
		}
		else if(instruction=="SUBL")
		{
			result+="011000";
			errchk = writeLine(args,2,true,linecount);
			if(errchk=="")
			{
				return 0;
			}
			else
			{
				result+=errchk;
			}
		}
		else if(instruction=="MOD")
		{
			result+="011001";
			errchk = writeLine(args,2,true,linecount);
			if(errchk=="")
			{
				return 0;
			}
			else
			{
				result+=errchk;
			}
		}
		else if(instruction=="PRNT")
		{
			result+="011010";
			std::string arg2 = args.substr(args.find(',')+1);
			int num;
			try
			{
				num=std::stoi(arg2);
			}
			catch(std::invalid_argument& e)
			{
				std::cerr<<"Error on line "<<linecount<<":: Invalid argument"<<std::endl;
				return 0;
			}
			if(num==3)
			{
				std::string arg1 = args.substr(0,args.find(','));
				std::string temp = decodeArg(arg1,false);
				if(temp=="")
				{
					std::cout<<"Error on line "<<linecount<<":: Invalid argument"<<std::endl;
					return 0;
				}
				result+="00000";
				result+=temp;
				result+="00000";
				result+="00000011";
			}
			else
			{
				errchk = writeLine(args,2,false,linecount);
				if(errchk=="")
				{
					return 0;
				}
				else
				{
					result+=errchk;
				}
			}
		}
		else if(instruction=="STRG")
		{
			result+="011011";
			errchk = writeLine(args,2,true,linecount);
			if(errchk=="")
			{
				return 0;
			}
			else
			{
				result+=errchk;
			}
		}
		else if(instruction=="ADCL")
		{
			result+="011100";
			errchk = writeLine(args,2,true,linecount);
			if(errchk=="")
			{
				return 0;
			}
			else
			{
				result+=errchk;
			}
		}
		else if(instruction=="ASR")
		{
			result+="011101";
			errchk = writeLine(args,2,false,linecount);
			if(errchk=="")
			{
				return 0;
			}
			else
			{
				result+=errchk;
			}
		}
		else if(instruction=="ASL")
		{
			result+="011110";
			errchk = writeLine(args,2,false,linecount);
			if(errchk=="")
			{
				return 0;
			}
			else
			{
				result+=errchk;
			}
		}
		else if(instruction=="LDRG")
		{
			result+="011111";
			errchk = writeLine(args,2,true,linecount);
			if(errchk=="")
			{
				return 0;
			}
			else
			{
				result+=errchk;
			}
		}
		else if(instruction=="SBRS")
		{
			result+="100000";
			errchk = writeLine(args,2,false,linecount);
			if(errchk=="")
			{
				return 0;
			}
			else
			{
				result+=errchk;
			}
		}
		else if(instruction=="SBRN")
		{
			result+="100001";
			errchk = writeLine(args,2,false,linecount);
			if(errchk=="")
			{
				return 0;
			}
			else
			{
				result+=errchk;
			}
		}
		else if(instruction=="CALL")
		{
			result+="100010";
			errchk = writeLine(args,1,true,linecount);
			if(errchk=="")
			{
				return 0;
			}
			else
			{
				result+=errchk;
			}
		}
		else
		{	
			std::cout<<"Error on line "<<linecount<<": Instruction not found."<<std::endl;
			return 0;
		}
		linecount++;
	}
	std::ofstream output{outname,std::ofstream::out};
	output<<result;
	output.close();
	return 0;
}

std::string writeLine(std::string args, int argnums, bool type, int lcount)
{
	if(argnums==2)
	{
		std::size_t split = args.find(',');
		std::string arg1,arg2,argtemp,result="";
		if(split==std::string::npos)
		{
			std::cout<<"Error on line "<<lcount<<": Incorrect number of arguments supplied to instruction, two required"<<std::endl;			
			return "";
		}	
		arg1=args.substr(0,split);
		arg2=args.substr(split+1);
		argtemp=decodeArg(arg1,true);
		if(argtemp=="")
		{
			std::cout<<"Error on line "<<lcount<<": Invalid argument"<<std::endl;
			return ""; 
		}
		result+="00000";
		result+=argtemp;
		argtemp = decodeArg(arg2,type);
		if(argtemp=="")
		{
			std::cout<<"Error on line "<<lcount<<": Invalid argument"<<std::endl;
			return ""; 
		}
		result+="00000";
		result+=argtemp;
		return result;
	}
	else if(argnums==1)
	{
		std::string result="00000";
		std::string argtemp=decodeArg(args,type);
		if(argtemp=="")
		{
			std::cout<<"Error on line "<<lcount<<": Invalid argument"<<std::endl;
			return "";
		}
		result+=argtemp;
		result+="0000000000000";
		return result;
	}
	else
	{
		return "00000000000000000000000000";
	}
}

std::string decodeArg(std::string input,bool type)//type: if true arg is a register, false if an immediate
{
	std::string result="";
	int num;
	
	if(type)
	{
		try
		{
			num = std::stoi(input.substr(1));
		}
		catch(const std::invalid_argument& e)
		{
			std::cerr<<e.what()<<std::endl;
		}
	}
	else
	{
		try
		{
			num = std::stoi(input);
		}
		catch(const std::invalid_argument& e)
		{
			std::cerr<<e.what()<<std::endl;
		}
	}
	
	for(int i = 0;i<8;i++)
	{
		if(num<pow(2,7-i))
		{
			result+="0";
		}
		else
		{
			result+="1";
			num-=pow(2,7-i);
		}
	}
		
	return result;
	
}





























