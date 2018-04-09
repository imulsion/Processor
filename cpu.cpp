#include "cpu.h"

CPU::CPU()
{
	for(int i=0;i<32;i++)
	{
		registers[i].setData({0,0,0,0,0,0,0,0});
	}
	pc[0]=Byte(0);
	pc[1]=Byte(0);
	status=Byte(0);
}

std::array<Byte,2> CPU::create16(int x) const
{
	std::array<std::array<bool,8>,2> temp = {std::array<bool,8>{0,0,0,0,0,0,0,0},std::array<bool,8>{0,0,0,0,0,0,0,0}};
	int y = x;
	x=abs(x);
	for(int i = 0;i<2;i++)
	{
		for(int j =0;j<8;j++)
		{
			if(x<pow(2,15-(i+j)))
			{
				temp[i][j]=false;
			}
			else
			{
				temp[i][j]=true;
				x-=pow(2,15-(i+j));
			}
		}
	}
	if(y<0)//flip bits and add 1 for two's complement
	{
		for(int k = 0;k<2;k++)
		{
			for(int l =0;l<8;l++)
			{
				temp[k][l]=!temp[k][l];
			}
		}
		Byte a(temp[1]);
		Byte b(temp[0]);
		a=a+1;
		if(a.getCarry())
		{
			a.setCarry(false);
			b=b+1;
		}
		return std::array<Byte,2>{b,a};
	}
	else
	{
		return std::array<Byte,2>{temp[0],temp[1]};
	}
}


const bool CPU::loadProgram(std::string* dataptr)
{
	if(dataptr->length()%32!=0)
	{
		std::cout<<"Error: One or more program instructions is the wrong length!"<<std::endl;
		return false;
	}
	
	std::array<bool,8> temp;
	std::array<Byte,2> addresscounter={Byte(0),Byte(0)};
	for(int i = 0;i<(dataptr->length()/8);i++)
	{
		for(int j = 0;j<8;j++)
		{
			temp[j]= dataptr->at((i*8)+j)=='1'?1:0;
			std::cout<<temp[j];
		}
		std::cout<<std::endl;
		mem.placeData({addresscounter[0],addresscounter[1]});
		mem.loadMAR();
		mem.placeData({Byte(0),temp});
		mem.loadMDR();
		mem.removeData();
		mem.setCE(false);
		mem.setWE(false);
		mem.setWE(true);
		mem.setCE(true);
		addresscounter[1]=addresscounter[1]+1;
		if(addresscounter[1].readCarry())
		{
			addresscounter[1].setCarry(false);
			addresscounter[0]=addresscounter[0]+1;
		}
	}

	std::cout<<std::endl;
	return true;
}

bool CPU::execute()
{
	std::array<Byte,4> cInstr;//current instruction
	std::array<Byte,2> wordAddress;
	Byte opcode;
	std::array<Byte,2> arg1;//R1
	std::array<Byte,2> arg2;//R2
	std::array<Byte,2> temp;//temporary storage for 16-bit operations
	bool isRunning = true;//detect program termination
	bool optype = false;//8-bit or 16-bit operation?
	
	while(true)//execution loop
	{
		/*
		      
			 ;;;;;;;;;;;;;;;;;;;;
			 ;Fetch cycle begins;
		     ;;;;;;;;;;;;;;;;;;;;
		
		*/
		
		//First, convert PC to a word address
		wordAddress=convertToWordAddress(pc);
		for(int i = 0;i<4;i++)
		{
			//Fetch the instruction
			mem.placeData(wordAddress);
			mem.loadMAR();
			mem.removeData();
			mem.setCE(false);
			mem.loadMDR();
			mem.setCE(true);
			cInstr[i]=mem.readMDR();
			//increment word address to address next byte of instruction
			wordAddress[1]=wordAddress[1]+1;
		}
		
		/*
		
			;;;;;;;;;;;;;;;;;;;;;
			;Decode cycle begins;
			;;;;;;;;;;;;;;;;;;;;;
			
		*/
		
		opcode.setData({0,0,0,cInstr[0][1],cInstr[0][2],cInstr[0][3],cInstr[0][4],cInstr[0][5]});
		arg1[0].setData({0,0,0,cInstr[0][6],cInstr[0][7],cInstr[1][0],cInstr[1][1],cInstr[1][2]});
		arg1[1].setData({cInstr[1][3],cInstr[1][4],cInstr[1][5],cInstr[1][6],cInstr[1][7],cInstr[2][0],cInstr[2][1],cInstr[2][2]});
		arg2[0].setData({0,0,0,cInstr[2][3],cInstr[2][4],cInstr[2][5],cInstr[2][6],cInstr[2][7]});
		arg2[1].setData({cInstr[3][0],cInstr[3][1],cInstr[3][2],cInstr[3][3],cInstr[3][4],cInstr[3][5],cInstr[3][6],cInstr[3][7]});
		
		/*
		
			;;;;;;;;;;;;;;;;;;;;;;
			;Execute cycle begins;
			;;;;;;;;;;;;;;;;;;;;;;
			
		*/
		
		switch(opcode.toInt(1))
		{
			case 0:
				registers[arg1[1].toInt(1)]=registers[arg2[1].toInt(1)];//only 32 registers so only need second half of address
				optype=false;
				break;
			case 1:
				registers[arg1[1].toInt(1)]=arg2[1].toInt(1);
				optype=false;
				break;
			case 2:
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]&registers[arg2[1].toInt(1)];
				optype=false;
				break;
			case 3:
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]|registers[arg2[1].toInt(1)];
				optype=false;
				break;
			case 4:
				registers[arg1[1].toInt(1)]=~registers[arg1[1].toInt(1)];
				optype=false;
				break;
			case 5:
				if(stack.empty())
				{
					isRunning=false;
				}
				else
				{
					pc = stack.front();
					stack.pop_back();
				}
				optype=false;
				break;
			case 6:
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]+registers[arg2[1].toInt(1)];
				optype=false;
				break;
			case 7:
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]+arg2[1].toInt(1);
				optype=false;
				break;
			case 8:
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]-registers[arg2[1].toInt(1)];
				optype=false;
				break;
			case 9:
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]-arg2[1].toInt(1);
				optype=false;
				break;
			case 10:
				temp=registers[arg1[1].toInt(1)]*registers[arg2[1].toInt(1)];
				registers[arg1[1].toInt(1)]=temp[1];
				registers[arg1[1].toInt(1)+1]=temp[0];
				optype=true;
				break;
			case 11:
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]/registers[arg2[1].toInt(1)];
				optype=false;
				break;
			case 12:
				stack.push_back(pc);
				pc[0]=arg1[0];
				pc[1]=arg1[1];
				optype=false;
				break;
			case 13:
				registers[arg1[1].toInt(1)]==registers[arg2[1].toInt(1)];
				optype=false;
				break;
			case 14:
				registers[arg1[1].toInt(1)]==arg2[1].toInt(1);
				optype=false;
				break;
			case 15:
				mem.placeData(arg2);
				mem.loadMAR();
				mem.removeData();
				mem.setCE(false);
				mem.loadMDR();
				mem.setCE(true);
				registers[arg1[1].toInt(1)]=mem.readMDR();
				optype=false;
				break;
			case 16:
				mem.placeData(arg1);
				mem.loadMAR();
				mem.placeData(arg2);
				mem.loadMDR();
				mem.removeData();
				mem.setCE(false);
				mem.setWE(false);
				mem.setWE(true);
				mem.setCE(true);
				optype=false;
				break;
			case 17:
				if(!status[0])
				{
					stack.push_back(pc);
					pc[0]=arg1[0];
					pc[1]=arg1[1];
				}
				optype=false;
				break;
			case 18:
				if(status[0])
				{
					stack.push_back(pc);
					pc[0]=arg1[0];
					pc[1]=arg1[1];
				}
				optype=false;
				break;
			case 19:
				if(status[1])
				{
					stack.push_back(pc);
					pc[0]=arg1[0];
					pc[1]=arg1[1];
				}
				optype=false;
				break;
			case 20:
				if(status[1])
				{
					stack.push_back(pc);
					pc[0]=arg1[0];
					pc[1]=arg1[1];
				}
				optype=false;
				break;
			case 21:
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]+1;
				optype=false;
				break;
			case 22:
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]-1;
				optype=false;
				break;
			case 23:
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]+registers[arg2[1].toInt(1)];
				if(registers[arg1[1].toInt(1)].readCarry())
				{
					registers[arg1[1].toInt(1)].setCarry(false);
					registers[arg1[1].toInt(1)+1].setCarryIn(true);
				}
				registers[arg1[1].toInt(1)+1]=registers[arg1[1].toInt(1)+1]+registers[arg2[1].toInt(1)+1];
				optype=true;
				break;
			case 24:
				registers[arg1[1].toInt(1)+1]=registers[arg1[1].toInt(1)+1]-registers[arg2[1].toInt(1)+1];
				if(registers[arg1[1].toInt(1)+1].readCarry())
				{
					registers[arg1[1].toInt(1)+1].setCarry(false);
					registers[arg1[1].toInt(1)].setCarryIn(true);
				}
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]-registers[arg2[1].toInt(1)];
				optype=true;
				break;
			case 25:
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]%registers[arg2[1].toInt(1)];
				optype=false;
				break;
			case 26:
				registers[arg1[1].toInt(1)].printData();
				std::cout<<std::endl;
				optype=false;
				break;
			case 27:
				if(status[2])
				{
					registers[arg1[1].toInt(1)].setCarryIn(true);
				}
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]+registers[arg2[1].toInt(1)];
				optype=false;
				break;
			case 28:
				if(status[2])
				{
					registers[arg1[1].toInt(1)].setCarryIn(true);
				}
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]+registers[arg2[1].toInt(1)];
				if(registers[arg1[1].toInt(1)].readCarry())
				{
					registers[arg1[1].toInt(1)].setCarry(false);
					registers[arg1[1].toInt(1)+1].setCarryIn(true);
				}
				registers[arg1[1].toInt(1)+1]=registers[arg1[1].toInt(1)+1]+registers[arg2[1].toInt(1)+1];
				optype=true;
			case 29:
				registers[arg1[1].toInt(1)]>>arg2[1].toInt(1);
				optype=false;
				break;
			case 30:
				registers[arg1[1].toInt(1)]<<arg2[1].toInt(1);
				optype=false;
				break;
			default:
				optype=false;
				break;
		}
		updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
		if(!isRunning)
		{
			break;
		}
		
	}

}

std::array<Byte,2>& CPU::convertToWordAddress(std::array<Byte,2> pc)
{
	std::array<std::array<bool,8>,2> temp={std::array<bool,8>{0,0,0,0,0,0,0,0},std::array<bool,8>{0,0,0,0,0,0,0,0}};
	for(int c=0;c<6;c++)
	{
		temp[1][5-c] = pc[1][7-c];
		temp[0][5-c] = pc[0][7-c];
	}
	temp[0][7]=pc[1][1];
	temp[0][6]=pc[1][0];
	std::array<Byte,2> tempbyte = {Byte(temp[0]),Byte(temp[1])};
}

void CPU::updateSREG(Byte* x,Byte* y,bool type)
{
	std::array<bool,8> temp = {0,0,0,0,0,0,0,0};
	if(!type)
	{
		if(x->toInt(1)==0)
		{
			temp[0]=true;
		}
		if(x->getData()[0])
		{
			temp[1]=true;
		}
		if(x->readCarry())
		{
			temp[2]=true;
		}
	}
	else
	{
		if((x->toInt(1)+y->toInt(1))==0)
		{
			temp[0]=true;
		}
		if(y->getData()[0])
		{
			temp[1]=true;
		}
		if(y->readCarry())
		{
			temp[2]=true;
		}
	}
	status = Byte(temp);
}

