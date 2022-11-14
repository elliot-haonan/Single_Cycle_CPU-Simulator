#include <iostream>
#include <bitset>
#include <stdio.h>
#include<stdlib.h>
#include <string>
#include "modules.h"
using namespace std;

class CPU {
private:
    /* Belows are all the instantiaions of the fucntional modules inside the CPU */
	int dmemory[4096]; //data memory byte addressable in little endian fashion;
	unsigned long PC; //pc 
	RegisterFile registers; //register file  
	Controller control; // controller
	ImmGen immGen; // immediate number generator
	ALU alu; // Arithmetic-Logical Unit
	ALUcontroller alucontroller; // controller for alu
	Adder adder; // adder to compute PC+offset
	 
	/* Belows are all the wires inside the CPU */
	bitset<5> rs1Addr,rs2Addr,rdAddr; // resource register and destination register address
	int readData1,readData2,WriteData; // read data and write data
	bitset<7> opcode; // opcode
	bitset<9> controlSignals; // encoded as {JALR,Branch,MemRead,MemtoReg,ALUOp[1:0],MemWrite,ALUSrc,RegWrite} 
    int immediate; // immediate number
	bitset<1> Less; // indicator
	int ALUresult;
	int data_from_mem; // data loaded from memory
	unsigned long PC_plus_4; 
	unsigned long PC_plus_offset; 
	unsigned long PC_next; 

public:
	/* Statistics */
	int num_clk, num_r_type;
	
	CPU(); //constructor
	int readRegister(int addr);
	unsigned long readPC();
	bitset<32> Fetch(bitset<8> *instmem);
	bool Decode(instruction* instr);
	
	bool Execution(instruction* instr);
	bool MemoryAccess();
	bool WriteBack();
};



