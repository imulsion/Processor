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
	if(argc<2)//if no cmd argument supplied
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
	std::string outname = incpy.erase(incpy.find('.'))+".sbf";//trim .saf extension in favour of .sbf
	int linecount=1;//program line count
	int instructioncount=0;//program instruction count - not the same!
	std::vector<std::string> labels;//label names 
	std::vector<int> labelnumbers;//and their instruction numbers
	std::ifstream reader{std::string(argv[1])};//read file supplied in cmd argument
	if(!reader)
	{
		std::cout<<"Error: Input file not found"<<std::endl;
		return 0;
	}
	while(std::getline(reader,line))//scan file for labels
	{
		if(line=="")//empty line
		{
			continue;
		}
		if(line.at(0)==';')//comment
		{
			continue;
		}
		if(line.find(':')!=std::string::npos)
		{
			//label found!
			labels.push_back(line.substr(0,line.find(':')));//save label name
			labelnumbers.push_back(instructioncount);//...and its instruction number
			line=line.substr(line.find(':')+1);//remove label from line - in case the line continues as an instruction
			if(line=="")
			{
				continue;
			}
			if(line.find(';')!=std::string::npos)
			{
				line.erase(line.find(';'));//remove comments
			}
			if(line=="")
			{
				continue;
			}
		}
		//if no label found, or an instruction appeared after the label, increment the instruction count
		instructioncount++;
	}
	//reset file reader to prepare to parse file again
	reader.clear();
	reader.seekg(0,reader.beg);//set stream cursor to beginning of file
	
	while(std::getline(reader,line))//parse again - this time translating instructions to machine code
	{
		if(line.empty())
		{
			linecount++;//increase line count
			continue;
		}
		for(int i = 0;i<line.length();i++)
		{
			if(line.at(i)==';')
			{
				line.erase(i);//check for and erase any comments
				break;
			}
		}
		if(line.empty())
		{
			linecount++;//check if removing the comment made the line empty
			continue;
		}
		split=line.find(':');
		if(split!=std::string::npos)
		{
			line=line.substr(split+1);//remove any labels
		}
		if(line.empty())
		{
			linecount++;//check if removing label made line empty
			continue;
		}
		split = line.find(' ');
		if(split==std::string::npos)
		{
			instruction=line;//for instructions with no arguments
			args="";
		}
		else
		{
			//separate a line into an instruction and its arguments
			instruction = line.substr(0,split);
			args = line.substr(split);
		}
		//remove whitespace
		std::string::iterator end_pos = std::remove(args.begin(), args.end(), ' ');
		args.erase(end_pos, args.end());
		
		if(instruction=="MOV")
		{
			result+="000000";//append opcode to resulting binary string
			errchk = writeLine(args,2,true,linecount);//attempt to parse arguments
			if(errchk=="")
			{
				return 0;//if there was an error, terminate compilation
			}
			else
			{
				result+=errchk;//otherwise append parsed arguments
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
			result+="00010100000000000000000000000000";//RET instructions are always the same - no arguments
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
			for(int i = 0;i<labels.size();i++)//search labels vector for a matching label
			{
				if(labels[i]==args)
				{
					//matching label found!
					matchfound=true;
					int num = labelnumbers[i];//fetch the corresponding instruction number for the jump
					for(int i = 0;i<8;i++)//convert it to binary...
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
					result+="0000000000000";//append second (ignored) instruction
					break;
				}
			}
			if(!matchfound)
			{
				//no matching label found
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
			//RAM instructions are a bit special - we parse their arguments in a different way
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
			result+=argtemp;//append first (register) argument like normal
			arg2=args.substr(split+1);
			//however, addresses should be in the format %x where x is the RAM address (as an int)
			if(arg2.at(0)!='%')//search for a % sign
			{
				std::cout<<"Error on line "<<linecount<<": Invalid address supplied to LOAD instruction"<<std::endl;				
				return 0;
			}
			int address;
			try
			{
				address = std::stoi(arg2.substr(1));//attempt to convert address to an int
			}
			catch(const std::invalid_argument& e)
			{
				std::cerr<<"Error on line "<<linecount<<": Invalid address supplied to LOAD instruction"<<std::endl;
				return 0;
			}
			for(int i = 0;i<13;i++)//convert address to binary
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
			//similar format to the above
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
			//branch instruction - very similar to the jump instruction, with a different opcode
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
			//another branch...
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
			//more branching...
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
			//yet another branch...
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
			//PRNT is special as one or both of its arguments can be immediate - parse appropriately
			result+="011010";
			std::string arg2 = args.substr(args.find(',')+1);
			int num;
			try
			{
				num=std::stoi(arg2);//convert arg2 to int
			}
			catch(std::invalid_argument& e)
			{
				std::cerr<<"Error on line "<<linecount<<":: Invalid argument"<<std::endl;
				return 0;
			}
			if(num==3)//if arg1 is an immediate
			{
				std::string arg1 = args.substr(0,args.find(','));
				std::string temp = decodeArg(arg1,false);
				if(temp=="")//if argument parsing failed
				{
					std::cout<<"Error on line "<<linecount<<":: Invalid argument"<<std::endl;
					return 0;
				}
				result+="00000";
				result+=temp;
				result+="00000";
				result+="00000011";
			}
			else//otherwise parse like normal
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
			result+="10001000000";
			bool matchfound = false;
			for(int i = 0;i<labels.size();i++)
			{
				if(labels[i]==args)
				{
					matchfound=true;
					//DEBUG 
					std::cout<<labels[i]<<std::endl;
					std::cout<<labelnumbers[i]<<std::endl;
					//END
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
		else//missing or non existent instruction
		{	
			std::cout<<"Error on line "<<linecount<<": Instruction not found."<<std::endl;
			return 0;
		}
		linecount++;
	}
	//write resulting binary string to the output
	std::ofstream output{outname,std::ofstream::out};
	output<<result;
	output.close();
	return 0;//compilation done, exit program
}

std::string writeLine(std::string args, int argnums, bool type, int lcount)
{
	if(argnums==2)//two argument instruction
	{
		std::size_t split = args.find(',');
		std::string arg1,arg2,argtemp,result="";
		if(split==std::string::npos)
		{
			std::cout<<"Error on line "<<lcount<<": Incorrect number of arguments supplied to instruction, two required"<<std::endl;			
			return "";
		}	
		//separate arguments
		arg1=args.substr(0,split);
		arg2=args.substr(split+1);
		//decode arguments
		argtemp=decodeArg(arg1,true);
		if(argtemp=="")
		{
			std::cout<<"Error on line "<<lcount<<": Invalid argument"<<std::endl;
			return ""; 
		}
		result+="00000";//append 5 0s - it's a register argument so top 5 bits not needed
		result+=argtemp;
		argtemp = decodeArg(arg2,type);
		if(argtemp=="")
		{
			std::cout<<"Error on line "<<lcount<<": Invalid argument"<<std::endl;
			return ""; 
		}
		//and again for the other argument...
		result+="00000";
		result+=argtemp;
		return result;
	}
	else if(argnums==1)//single argument instruction - leave second arg as 0000000000000
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
	else//zero argument instruction - all arguments written to zero
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
			num = std::stoi(input.substr(1));//remove the 'R' at the front (e.g. R4->4) then convert to int
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
			num = std::stoi(input);//immediate value, so no need to edit the string - it's just a number
		}
		catch(const std::invalid_argument& e)
		{
			std::cerr<<e.what()<<std::endl;
		}
	}
	
	for(int i = 0;i<8;i++)//convert extracted int to binary, and append to resulting string
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





























