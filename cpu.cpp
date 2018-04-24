#include "cpu.h"

CPU::CPU()
{
	//clear all memory
	for(int i=0;i<32;i++)
	{
		registers[i]=Byte(0);
	}
	pc[0]=Byte(0);
	pc[1]=Byte(0);
	registers[31]=Byte(0);
}

const bool CPU::loadProgram(std::string* dataptr)
{
	if(dataptr->length()%32!=0)//program length not divisible by 32 => incorrect instruction length
	{
		std::cout<<"Error: One or more program instructions is the wrong length! Current program length is "<<dataptr->length()<<std::endl;
		return false;//load failed
	}
	
	std::array<bool,8> temp;
	std::array<Byte,2> addresscounter={Byte(0),Byte(0)};//address counter - increment as bytes are written to write to new addresses
	for(int i = 0;i<(dataptr->length()/8);i++)
	{
		//iterate through data string, load into memory one byte at a time
		for(int j = 0;j<8;j++)
		{
			temp[j]= dataptr->at((i*8)+j)=='1'?1:0;
		}
		mem.placeData(addresscounter);//place address on bus
		mem.loadMAR();//load memory address register
		mem.placeData({Byte(0),temp});//place program data on bus
		mem.loadMDR();//load memory data register
		mem.removeData();//clear bus
		mem.setCE(false);//chip enable low
		mem.setWE(false);//write enable low
		mem.setWE(true);//write enable high
		mem.setCE(true);//chip enable high
		
		addresscounter[1]=addresscounter[1]+1;//select next address
		if(addresscounter[1].readCarry())
		{
			//add with carry
			addresscounter[1].setCarry(false);
			addresscounter[0]=addresscounter[0]+1;
		}
	}
	return true;//load successful
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
			//Fetch the instruction from RAM
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
		//construct opcode and argument Bytes from program memory data
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
			case 0://MOV
				registers[arg1[1].toInt(1)]=registers[arg2[1].toInt(1)];//only 32 registers so only need second half of address
				optype=false;//8 bit operation
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);//this function uses the operation results to determine the correct state of the status register
				break;//break from case
			case 1://MOVI
				registers[arg1[1].toInt(1)]=arg2[1].toInt(1);//second argument is an immediate - doesn't access registers
				optype=false;
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
				break;
			case 2://AND
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]&registers[arg2[1].toInt(1)];//Most logical and arithmetic operators have been overridden - see Byte class for details
				optype=false;
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
				break;
			case 3://OR
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]|registers[arg2[1].toInt(1)];//Same as above
				optype=false;
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
				break;
			case 4://NOT
				registers[arg1[1].toInt(1)]=~registers[arg1[1].toInt(1)];//Single argument instruction. In the machine code, the second argument is 0000000000000 but it is ignored by the processor
				optype=false;
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
				break;
			case 5://RET
				if(stack.empty())
				{
					isRunning=false;//cease execution if stack empty
					break;
				}
				//otherwise, return from procedure
				pc = stack.top();//return PC to the function call
				stack.pop();//pop function off stack
				optype=false;
				clearSREG();//this operation should not affect the status register - clear it
				break;
			case 6://ADD (without carry)
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]+registers[arg2[1].toInt(1)];//operator override as before
				optype=false;
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
				break;
			case 7://ADDI (add immediate)
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]+arg2[1].toInt(1);
				optype=false;
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
				break;
			case 8://SUB
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]-registers[arg2[1].toInt(1)];
				optype=false;
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
				break;
			case 9://SUBI
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]-arg2[1].toInt(1);
				optype=false;
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
				break;
			case 10://MUL
				temp=registers[arg1[1].toInt(1)]*registers[arg2[1].toInt(1)];//8 bit MUL produces 16 bit output - stored temporarily in the temp Byte array
				registers[arg1[1].toInt(1)]=temp[1];
				registers[arg1[1].toInt(1)+1]=temp[0];//result placed in R1 and R1+1
				optype=true;
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
				break;
			case 11://DIV
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]/registers[arg2[1].toInt(1)];
				optype=false;
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
				break;
			case 12://JMP
				//copy the argument (a label) to the program counter
				pc[0]=arg1[0];
				pc[1]=arg1[1];
				optype=false;
				clearSREG();
				continue;//don't break - we don't want PC to be incremented. Just execute the jumped-to instruction
			case 13://CP
				if(registers[arg1[1].toInt(1)]==registers[arg2[1].toInt(1)])
				{
					registers[31]=Byte(0);//If equal, clear sign and zero flags
				}
				else
				{
					if(registers[arg2[1].toInt(1)]>registers[arg1[1].toInt(1)])
					{
						registers[31] = Byte({1,1,0,0,0,0,0,0});//If R2>R1, set sign flag
					}
					else
					{
						registers[31] = Byte({1,0,0,0,0,0,0,0});//Otherwise reset sign flag. Not equal so set zero flag
					}
				}
				optype=false;
				break;
			case 14://CPI
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
			case 15://LOAD - Retrieves some data from RAM and places it in a register. See the Memory class for more details
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
			case 16://STORE - Stores a register's data in memory. See Memory class for details
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
			case 17://BNE - branch if zero flag set
				if(registers[31][0])//test zero flag
				{
					//branch
					pc[0]=arg1[0];
					pc[1]=arg1[1];
					continue;//don't want PC incremented...
				}
				optype=false;
				clearSREG();
				break;
			case 18://BREQ - branch if zero flag reset
				if(!registers[31][0])
				{
					pc[0]=arg1[0];
					pc[1]=arg1[1];
					continue;
				}
				optype=false;
				clearSREG();
				break;
			case 19://BRSN - branch if sign flag not set
				if(!registers[31][1])
				{
					pc[0]=arg1[0];
					pc[1]=arg1[1];
					continue;
				}
				optype=false;
				clearSREG();
				break;
			case 20://BRSS - branch if sign flag set
				if(registers[31][1])
				{
					pc[0]=arg1[0];
					pc[1]=arg1[1];
					continue;
				}
				optype=false;
				clearSREG();
				break;
			case 21://INC - increment
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]+1;
				optype=false;
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
				break;
			case 22://DEC - decrement
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]-1;
				optype=false;
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
				break;
			case 23://ADDL - 16 bit add - result placed in R1 and R1+1
				registers[arg1[1].toInt(1)]=registers[arg1[1].toInt(1)]+registers[arg2[1].toInt(1)];//add lower byte
				if(registers[arg1[1].toInt(1)].readCarry())//if a carry happens
				{
					registers[arg1[1].toInt(1)].setCarry(false);//reset carry for that byte
					registers[arg1[1].toInt(1)+1].setCarryIn(true);//carry in set
				}
				registers[arg1[1].toInt(1)+1]=registers[arg1[1].toInt(1)+1]+registers[arg2[1].toInt(1)+1];//add higher byte with carry
				optype=true;
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
				break;
			case 24://SUBL - 16 bit subtract
				//the processor subtracts by inverting the second argument and carrying in a bit - essentially converting the second argument to 2's complement
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
			case 25://MOD - 8-bit modulus operator
				registers[arg1[1].toInt(1)]=registers[arg2[1].toInt(1)]%registers[arg1[1].toInt(1)];
				optype=false;
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
				break;
			case 26://PRNT - Prints R1 to the screen, with some output options: R2 is an immediate value that specifies the output format
				if(arg2[1].toInt(1)==0)//print the raw binary
				{
					registers[arg1[1].toInt(1)].printData();
				}
				else if(arg2[1].toInt(1)==1)//convert to an int and print (8 bits)
				{
					std::cout<<registers[arg1[1].toInt(1)].toInt(1);
				}
				else if(arg2[1].toInt(1)==2)//convert R1 and R1+1 to a 16-bit int and print
				{
					std::cout<<registers[arg1[1].toInt(1)].toInt(1)+registers[arg1[1].toInt(1)+1].toInt(0);
				}
				else //treat R1 as an immediate value, and print straight to the screen
				{
					std::cout<<arg1[1].toInt(1);
				}
				std::cout<<std::endl;
				optype=false;
				clearSREG();
				break;
			case 27://STRG - Store byte in register at memory location specified in two other registers. Allows for dynamic RAM addressing
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
			case 28://ADCL - 16 bit add with carry flag. Allows 32 bit addition
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
			case 29://ASR - Shifts the register 1 bit to the right, placing a 0 on the MSB
				registers[arg1[1].toInt(1)]>>arg2[1].toInt(1);
				optype=false;
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
				break;
			case 30://ASL - Left shift, with a 0 on the LSB
				registers[arg1[1].toInt(1)]<<arg2[1].toInt(1);
				optype=false;
				updateSREG(&registers[arg1[1].toInt(1)],&registers[arg1[1].toInt(1)+1],optype);
				break;
			case 31://LDRG - Retrieve data at RAM address specified in arg2 and arg2+1
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
			case 32://SBRS R1,x - Skip next instruction if bit x in R1 is set
				if(registers[arg1[1].toInt(1)][arg2[1].toInt(1)])//test bit
				{
					//increment PC
					pc[1]=pc[1]+1;
					if(pc[1].readCarry())
					{
						pc[1].setCarry(false);
						pc[0]=pc[0]+1;
					}
				}
				//otherwise do nothing
				optype=false;
				clearSREG();
				break;//PC will be incremented again, so if bit x set next instruction skipped
			case 33://SBRN - Skip if bit in register not set
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
			case 34://CALL - Calls procedure specified in label
				stack.push(pc);//push pc onto the stack
				pc[0]=arg1[0];//jump to label
				pc[1]=arg1[1];
				optype = false;
				clearSREG();
				break;
			default: // Any other instruction counts as a NOP - no operation
				optype=false;
				clearSREG();
				break;
		}
		
		
		if(!isRunning)
		{
			//terminate program if RET called and stack empty
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
	//Convert PC to a word address - essentially shifts PC left by two bits. this allows PC to only increment by 1 each cycle, but still have 4 byte instructions
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
	std::array<bool,8> temp = {0,0,0,0,0,0,0,0};//output status reg
	if(!type)//8 bit operation occured
	{
		if(x->toInt(1)==0)//if the operation resulted in a zero
		{
			temp[0]=true;
		}
		if(x->getData()[0])//if sign bit (MSB) of result is set - allows both signed and unsigned operations
		{
			temp[1]=true;
		}
		if(x->readCarry())//set carry flag if carry resulted from arithmetic operation
		{
			temp[2]=true;
		}
	}
	else//16 bit operation occured
	{
		if((x->toInt(1)+y->toInt(1))==0)//if result from 16 bit operation was 0
		{
			temp[0]=true;
		}
		if(y->getData()[0])//same as above, but with higher (most significant) byte
		{
			temp[1]=true;
		}
		if(y->readCarry())
		{
			temp[2]=true;
		}
		y->setCarry(false);
	}
	x->setCarry(false);//unset carries in x and y bytes
	
	registers[31] = Byte(temp);//set SREG
}

void CPU::clearSREG()
{
	registers[31]=Byte(0);
}
