#include "cpu.h"

CPU::CPU()
{
	for(int i=0;i<32;i++)
	{
		registers[i]=Byte(0);
	}
	pc[0]=Byte(0);
	pc[1]=Byte(0);
	registers[31]=Byte(0);
}

std::array<Byte,2> CPU::create16(int x) const//quick initialiser for 16 bit numbers
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
		if(a.readCarry())
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
		std::cout<<"Error: One or more program instructions is the wrong length! Current program length is "<<dataptr->length()<<std::endl;
		return false;
	}
	
	std::array<bool,8> temp;
	std::array<Byte,2> addresscounter={Byte(0),Byte(0)};
	for(int i = 0;i<(dataptr->length()/8);i++)
	{
		for(int j = 0;j<8;j++)
		{
			temp[j]= dataptr->at((i*8)+j)=='1'?1:0;
		}
		mem.placeData(addresscounter);
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
		if(!stack.empty())
		{
			if(pc==stack.top())
			{
				stack.pop();//if program is looping don't endlessly fill up the stack
			}
		}
		/*
		
			;;;;;;;;;;;;;;;;;;;;;
			;Decode cycle begins;
			;;;;;;;;;;;;;;;;;;;;;
			
		*/
		
		opcode.setData({0,0,cInstr[0][0],cInstr[0][1],cInstr[0][2],cInstr[0][3],cInstr[0][4],cInstr[0][5]});
		arg1[0].setData({0,0,0,cInstr[0][6],cInstr[0][7],cInstr[1][0],cInstr[1][1],cInstr[1][2]});
		arg1[1].setData({cInstr[1][3],cInstr[1][4],cInstr[1][5],cInstr[1][6],cInstr[1][7],cInstr[2][0],cInstr[2][1],cInstr[2][2]});
		arg2[0].setData({0,0,0,cInstr[2][3],cInstr[2][4],cInstr[2][5],cInstr[2][6],cInstr[2][7]});
		arg2[1].setData({cInstr[3][0],cInstr[3][1],cInstr[3][2],cInstr[3][3],cInstr[3][4],cInstr[3][5],cInstr[3][6],cInstr[3][7]});
		
		/*
		//DEBUG
		if(registers[1].toInt(0)+registers[0].toInt(1)==1000)
		{
			std::cout<<"On line "<<pc[1].toInt(1)+1<<":"<<std::endl;
			std::cout<<"Data: "<<std::endl;
			for(int i = 0;i<12;i++)
			{
				std::cout<<"R"<<i<<": ";
				registers[i].printData();
				std::cout<<std::endl;
			}
			std::cout<<"Status register: ";
			registers[31].printData();
			std::cout<<std::endl<<std::endl;
		}
		//END
		*/
		
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
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
				break;
			case 1:
				registers[arg1[1].toInt(1)]=arg2[1].toInt(1);
				optype=false;
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
				break;
			case 2:
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]&registers[arg2[1].toInt(1)];
				optype=false;
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
				break;
			case 3:
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]|registers[arg2[1].toInt(1)];
				optype=false;
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
				break;
			case 4:
				registers[arg1[1].toInt(1)]=~registers[arg1[1].toInt(1)];
				optype=false;
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
				break;
			case 5:
				if(stack.empty())
				{
					isRunning=false;
					break;
				}
				pc = stack.top();
				stack.pop();
				optype=false;
				clearSREG();
				break;
			case 6:
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]+registers[arg2[1].toInt(1)];
				optype=false;
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
				break;
			case 7:
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]+arg2[1].toInt(1);
				optype=false;
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
				break;
			case 8:
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]-registers[arg2[1].toInt(1)];
				optype=false;
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
				break;
			case 9:
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]-arg2[1].toInt(1);
				optype=false;
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
				break;
			case 10:
				temp=registers[arg1[1].toInt(1)]*registers[arg2[1].toInt(1)];
				registers[arg1[1].toInt(1)]=temp[1];
				registers[arg1[1].toInt(1)+1]=temp[0];
				optype=true;
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
				break;
			case 11:
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]/registers[arg2[1].toInt(1)];
				optype=false;
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
				break;
			case 12:
				pc[0]=arg1[0];
				pc[1]=arg1[1];
				optype=false;
				clearSREG();
				continue;
				break;
			case 13:
				if(registers[arg1[1].toInt(1)]==registers[arg2[1].toInt(1)])
				{
					registers[31]=Byte(0);
				}
				else
				{
					if(registers[arg2[1].toInt(1)]>registers[arg1[1].toInt(1)])
					{
						registers[31] = Byte({1,1,0,0,0,0,0,0});
					}
					else
					{
						registers[31] = Byte({1,0,0,0,0,0,0,0});
					}
				}
				optype=false;
				break;
			case 14:
				if(registers[arg1[1].toInt(1)]==arg2[1].toInt(1))
				{
					registers[31]=Byte(0);
				}
				else
				{
					if(registers[arg1[1].toInt(1)]>arg2[1].toInt(1))
					{
						registers[31] = Byte({1,1,0,0,0,0,0,0});
					}
					else
					{
						registers[31] = Byte({1,0,0,0,0,0,0,0});
					}
				}
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
				clearSREG();
				break;
			case 16:
				mem.placeData(arg1);
				mem.loadMAR();
				mem.placeData({Byte(0),registers[arg2[1].toInt(1)]});
				mem.loadMDR();
				mem.removeData();
				mem.setCE(false);
				mem.setWE(false);
				mem.setWE(true);
				mem.setCE(true);
				optype=false;
				clearSREG();
				break;
			case 17:
				if(registers[31][0])
				{
					pc[0]=arg1[0];
					pc[1]=arg1[1];
					continue;
				}
				optype=false;
				clearSREG();
				break;
			case 18:
				if(!registers[31][0])
				{
					pc[0]=arg1[0];
					pc[1]=arg1[1];
					continue;
				}
				optype=false;
				clearSREG();
				break;
			case 19:
				if(!registers[31][1])
				{
					pc[0]=arg1[0];
					pc[1]=arg1[1];
					continue;
				}
				optype=false;
				clearSREG();
				break;
			case 20:
				if(registers[31][1])
				{
					pc[0]=arg1[0];
					pc[1]=arg1[1];
					continue;
				}
				optype=false;
				clearSREG();
				break;
			case 21:
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]+1;
				optype=false;
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
				break;
			case 22:
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]-1;
				optype=false;
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
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
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
				break;
			case 24:
				registers[arg1[1].toInt(1)].setCarryIn(true);
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]-registers[arg2[1].toInt(1)];
				if(registers[arg1[1].toInt(1)].readCarry())
				{
					registers[arg1[1].toInt(1)+1].setCarryIn(true);
				}
				registers[arg1[1].toInt(1)+1]=registers[arg1[1].toInt(1)+1]-registers[arg2[1].toInt(1)+1];
				optype=true;
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
				break;
			case 25:
				registers[arg1[1].toInt(1)]=registers[arg2[1].toInt(1)]%registers[arg1[1].toInt(1)];
				optype=false;
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
				break;
			case 26:
				if(arg2[1].toInt(1)==0)
				{
					registers[arg1[1].toInt(1)].printData();
				}
				else if(arg2[1].toInt(1)==1)
				{
					std::cout<<registers[arg1[1].toInt(1)].toInt(1);
				}
				else if(arg2[1].toInt(1)==2)
				{
					std::cout<<registers[arg1[1].toInt(1)].toInt(1)+registers[arg1[1].toInt(1)+1].toInt(0);
				}
				else
				{
					std::cout<<arg1[1].toInt(1);
				}
				std::cout<<std::endl;
				optype=false;
				clearSREG();
				break;
			case 27:
				mem.placeData({registers[arg1[1].toInt(1)+1],registers[arg1[1].toInt(1)]});
				mem.loadMAR();
				mem.placeData({Byte(0),registers[arg2[1].toInt(1)]});
				mem.loadMDR();
				mem.removeData();
				mem.setCE(false);
				mem.setWE(false);
				mem.setWE(true);
				mem.setCE(true);
				optype=false;
				clearSREG();
				break;
			case 28:
				if(registers[31][2])
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
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
				break;
			case 29:
				registers[arg1[1].toInt(1)]>>arg2[1].toInt(1);
				optype=false;
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
				break;
			case 30:
				registers[arg1[1].toInt(1)]<<arg2[1].toInt(1);
				optype=false;
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
				break;
			case 31:
				mem.placeData({registers[arg2[1].toInt(1)+1],registers[arg2[1].toInt(1)]});
				mem.loadMAR();
				mem.removeData();
				mem.setCE(false);
				mem.loadMDR();
				mem.setCE(true);
				registers[arg1[1].toInt(1)]=mem.readMDR();
				optype=false;
				clearSREG();
				break;
			case 32:
				if(registers[arg1[1].toInt(1)][arg2[1].toInt(1)])
				{
					pc[1]=pc[1]+1;
					if(pc[1].readCarry())
					{
						pc[1].setCarry(false);
						pc[0]=pc[0]+1;
					}
				}
				optype=false;
				clearSREG();
				break;
			case 33:
				if(!registers[arg1[1].toInt(1)][arg2[1].toInt(1)])
				{
					pc[1]=pc[1]+1;
					if(pc[1].readCarry())
					{
						pc[1].setCarry(false);
						pc[0]=pc[0]+1;
					}
				}
				optype=false;
				clearSREG();
				break;
			case 34:
				stack.push(pc);
				pc[0]=arg1[0];
				pc[1]=arg1[1];
				optype = false;
				clearSREG();
				break;
			default:
				optype=false;
				clearSREG();
				break;
		}
		
		
		if(!isRunning)
		{
			return true;
		}
		
		//increment PC
		pc[1]=pc[1]+1;
		if(pc[1].readCarry())
		{
			pc[1].setCarry(false);
			pc[0]=pc[0]+1;
		}
	}

}

std::array<Byte,2> CPU::convertToWordAddress(std::array<Byte,2> pc)
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
	return tempbyte;
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
		y->setCarry(false);
	}
	x->setCarry(false);
	
	registers[31] = Byte(temp);
}

void CPU::clearSREG()
{
	registers[31]=Byte(0);
}
