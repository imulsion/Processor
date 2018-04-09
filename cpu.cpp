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


bool CPU::loadProgram(std::string* dataptr)
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
		arg2[1].setData(cInstr[3]);
		
		/*
		
			;;;;;;;;;;;;;;;;;;;;;;
			;Execute cycle begins;
			;;;;;;;;;;;;;;;;;;;;;;
			
		*/
		
		switch(opcode.toInt(1))
		{
			case 0:
				registers[arg1[1].toInt(1)]=registers[arg2[1].toInt(1)];//only 32 registers so only need second half of address
				break;
			case 1:
				registers[arg1[1].toInt(1)]=arg2[1].toInt();
				break;
			case 2:
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]&registers[arg2[1].toInt(1)];
				break;
			case 3:
				registers[arg1[1].toInt(1)]=~registers[arg1[1].toInt(1)];
				break;
			case 4:
				isRunning=false;
				break;
			case 5:
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]+registers[arg2[1].toInt(1)];
				break;
			case 6:
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]+arg2[1].toInt(1);
				break;
			case 7:
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]-registers[arg2[1].toInt(1)];
				break;
			case 8:
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]-arg2[1].toInt(1);
				break;
			case 9:
				temp=registers[arg1[1].toInt(1)]*registers[arg2[1].toInt(1)];
				registers[arg1[1].toInt(1)]=temp[1];
				registers[arg1[1].toInt(1)+1]=temp[0];
				break;
			case 10:
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]/registers[arg2[1].toInt(1)];
				break;
			case 11:
				pc[0]=arg1[0];
				pc[1]=arg1[1];
				break;
			case 12:
				registers[arg1[1].toInt(1)]==registers[arg2[1].toInt(1)];
				break;
			case 13:
				registers[arg1[1].toInt(1)]==arg2[1].toInt(1);
				break;
			case 14:
				
				
				
		}
		updateSREG(&registers[arg1[1].toInt(1)]);
		if(!isRunning)
		{
			break;
		}
		
	}

}

std::array<Byte,2>& CPU::convertToWordAddress(std::array<Byte,2> pc)
{
	std::array<Byte,2> temp={Byte(0),Byte(0)};
	for(int c=0;c<6;c++)
	{
		temp[1][5-c] = pc[1][7-c];
		temp[0][5-c] = pc[0][7-c];
	}
	temp[0][7]=pc[1][1];
	temp[0][6]=pc[1][0];
	return temp;
}

void CPU::updateSREG(Byte* x)
{
	status[0]=false;
	status[1]=false;
	status[2]=false;
	if(x->readZero())
	{
		status[0]=true;
	}
	if(x->readSign())
	{
		status[1]=true;
	}
	if(x->readCarry())
	{
		status[2]=true;
	}
}

