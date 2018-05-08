These source files compose an 8-bit Processor emulator, which runs machine code. A compiler is included to convert Assembly code into machine code. The file extensions used by the programs are .saf (Assembly code) and .sbf (machine code runnable by the processor). This project is written in pure standard-compilant C++ 17, and must be compiled as such. This requires MinGW version 7.0.0 or equivalent.

----------------------------------------------------------------------------------------------

# A) How to write Assembly for the processor

The instruction set can be seen in part b) of this document. Take care to provide the correct number of arguments to every instruction. Most instructions operate on the general purpose CPU registers. The are specified by R followed by the register number, for example MOV R0,R4 copies the contents of R4 to R0. Do NOT attempt to write to R31, as this is the status register and using it for general storage will produce undefined behaviour. Any data written to it will also be erased every cycle. Currently there is no support for defining variables with a .def command or similar, such as in AVR ASM. For the instructions that operate on RAM, addresses must be specified with a % sign, for example LOAD R0,%4357 will fetch the value stored at address 4357 and place it in R0. Take care not to overwrite program memory, which is stored in addresses 0-2048. However, there is no forced restriction of this, so if you are confident in the length of your program any addresses after the final instruction of your program may be used as normal. There are 8192 bytes of addressable RAM available to the processor, with addresses %0 to %8191. Programs are always stored beginning at address 0. Programs longer than 512 instructions cannot be run on the processor. Labels are defined with a word followed by a colon, e.g. loop:. A newline is not necessary after labels, but may help readability. Newlines are necessary after instructions. Comments are supported in the compiler. Any text starting with a semi colon ; will be ignored. This includes comments on the same line as an instruction e.g. MOV R0,R5 ;move R5 to R0 is legal and will work as expected. 

----------------------------------------------------------------------------------------------

# B) Instruction set 

	000000 - MOV R1,R2 //Copy R2 to R1 
	000001 - MOVI R1,x //x->R1 
	000010 - AND R1,R2 //R1=R1.R2 
	000011 - OR R1,R2 //R1 = OR(R1,R2) 
	000100 - NOT R1 //R1=~R1 
	000101 - RET //return from procedure 
	000110 - ADD R1,R2 //R1=R1+R2 signed, no carry 
	000111 - ADDI R1,x //R1=R1+x signed 
	001000 - SUB R1,R2 //R1=R1-R2 signed 
	001001 - SUBI R1,x //R1=R1-x signed 
	001010 - MUL R1,R2 //R1HL=R1*R2 unsigned - 16 bit result placed in R1(lower) and R1+1(higher) 
	001011 - DIV R1,R2 //R1 = floor(R1/R2) unsigned 
	001100 - JMP label //jump to label 
	001101 - CP R1,R2 //compare R1 to R2, unset zero flag if equal, set otherwise, set sign flag if R2<R1 signed 
	001110 - CPI R1,x //compare R1 to x, unset zero if equal, set otherwise, set sign flag if x<R1 (signed)
	001111 - LOAD R1,ADD1 //load contents of RAM address ADD1 to R1 
	010000 - STORE ADD1,R1 //store contents of R1 at RAM address ADD1 
	010001 - BNE label //branch if zero flag set 
	010010 - BREQ label //branch if zero flag not set 
	010011 - BRSN label //branch if sign flag not set
	010100 - BRSS label //branch if sign flag set
	010101 - INC R1 //increment value in R1 
	010110 - DEC R1 //decrement value in R1 
	010111 - ADDL R1HL,R2HL //R1HL = R1HL+R2HL  (16 bits) 
	011000 - SUBL R1HL,R2HL //R1HL = R1HL-R2HL (16 bits) 
	011001 - MOD R1,R2 //R1 = R1%R2 
	011010 - PRNT R1,x //prints the value stored in R1 to the screen. If x=0 print in base 2, if x=1 print in base 10 (8 bit), if x=2 print in base 10 (16 bit), if x=3 R1 becomes an immediate value 
	011011 - STRG R1HL,R2 //Store byte stored in R2 at RAM address specified in R1(lower) and R1+1 (higher) 
	011100 - ADCL R1HL,R2HL //R1=R1+R2 (16 bits) with carry 
	011101 - ASR R1,x //arithmetic right shift R1<-R1 x times 
	011110 - ASL R1,x //arithmetic left shift R1<-R1 x times 
	011111 - LDRG R1,R2HL //Load byte stored at address R2HL in R1 
	100000 - SBRS R1,x//Skip next instruction if bit x in R1 is set (MSB = bit 0) 
	100001 - SBRN R1,x//Skip next instruction if bit x in R1 is reset 
	100010 - CALL label//Call procedure at label

# EXAMPLE:

### Showcase program 1: A counter loops through all the numbers from 0-99, and calculates the square of each, printing it out.

```
MOVI R0,0
loop:
MOV R1,R0
MUL R1,R1
PRNT R1,2
INC R0
CPI R0,100
BNE loop
RET
```


------------------------------------------------------------------------------------------------

# C) Command line


The compilation command for the emulator is

`c++ main.cpp cpu.cpp byte.cpp memory.cpp -o main.exe -std=c++17`

It cannot be stressed enough that the program requires full support for the C++ 17 standard, and WILL NOT compile without it. MinGW v7.0.0 or equivalent is required for this.

The compiliation command for the compiler program is

`c++ compiler.cpp -o compiler.exe`

C++ 17 support is NOT required to build the compiler successfully.

.saf Assembly files can be compiled with the command

`compiler <path_to_file>`

The file must have a .saf extension, and this must be included in the command.

Programs can be run with 

`main <path_to_file>`

The file must have a .sbf extension, and this must be included in the command.

-----------------------------------------------------------------------------------------------

# D) Debug mode


Debugging mode is a useful feature that will log the state of user specified registers at every machine cycle. The file that is produced will be named <filename>.sbf.log. It is readable with any text editor. 

Debugging mode is enabled with a command line flag:

`main myprog.sbf -debug <registers> <max_cycles>`

The registers parameter is required when using debug mode and specifies which registers should be logged. These are specified in a comma separated list, but can also include ranges. The following are all valid options:

0-4,5,6-8
4,6,7,20
12,4-6,9
14,16,5-7

The status register is logged automatically, so there is no need to include 31 in the list if the status register needs to be viewed.

The max_cycles parameter limits the number of cycles to log, which can be useful when running large and complex .sbf programs. The emulator will only log up to n machine cycles of the emulator, where n is given my max_cycles. This is optional, and does not need to be specified. If it is not, the emulator will attempt to log an infinite number of cycles.

Example:

`main myprog.sbf -debug 0-4,6,10 2500`
 
The above command will run myprog.sbf and log R0, R1, R2, R3, R4, R6, R10 and the status register for a maximum of 2500 machine cycles.

-----------------------------------------------------------------------------------------------

# E) Troubleshooting

### i) When trying to compile the emulator, the compiler gives this error: optional is not a member of std.

A. Compilation must be done using the `-std=c++17` flag using a compiler with C++ 17 support such as MinGW v7.0.0. 

### ii) After trying to compile a .saf program, the compiler did nothing but print "stoi"!

Check your instructions are correct. The compiler performs a syntactic check but absolutely no semantic checks. It assumes the instructions are correct.

### iii) I'm trying to run debug mode. Why is the emulator printing "Unrecognised flag: debug"?

The debug flag must be prefixed with a hyphen like other cmd flags.

### iv) The emulator crashes when trying to run my program!
Make sure your last instruction is a RET instruction. If it is not, the program will never stop executing and the program counter will eventually point to a value outside the RAM data array, causing a crash.

### v) The compiler will not compile my program as it says the file could not be found!

The compilation command must include the full path to the file unless the file is in the same directory as the emulator executable. If it is in a subdirectory, then a shortened path may be used, 
	   for example `compiler My_Programs\myprog.saf` will work fine.


















