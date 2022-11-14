#include "CPU.h"
#include "tool_function.h"
#include <iostream>
instruction::instruction(bitset<32> fetch)
{
	//cout << fetch << endl;
	instr = fetch;
	//cout << instr << endl;
}

CPU::CPU()
{
	PC = 0; //set PC to 0
	num_clk = 0; // initiate the statistics value
	num_r_type = 0;
	for (int i = 0; i < 4096; i++) //copy instrMEM
	{
		dmemory[i] = (0);
	}
}

bitset<32> CPU::Fetch(bitset<8> *instmem) {
	bitset<32> instr = ((((instmem[PC + 3].to_ulong()) << 24)) + ((instmem[PC + 2].to_ulong()) << 16) + ((instmem[PC + 1].to_ulong()) << 8) + (instmem[PC + 0].to_ulong()));  //get 32 bit instruction
	PC_plus_4 = PC + 4;
	return instr;
}


bool CPU::Decode(instruction* curr)
{
for(int i=0;i<rs1Addr.size();i++)
	rs1Addr[i] = (curr->instr)[15+i]; // generate rs1Addr field
for(int i=0;i<rs2Addr.size();i++)
	rs2Addr[i] = (curr->instr)[20+i]; // generate rs2Addr field
for(int i=0;i<rdAddr.size();i++)
	rdAddr[i] = (curr->instr)[7+i]; // generate rdAddr field
for(int i=0;i<opcode.size();i++)
	opcode[i] = (curr->instr)[i]; // generate opcode field
//cout << curr->instr << endl;
controlSignals = control.genControlSignal(&opcode); // generate control signals for datapath
if(control.getState() == "Rtype")
	num_r_type += 1;
//cout << controlSignals << endl;
readData1 = registers.read(&rs1Addr); // read registers
readData2 = registers.read(&rs2Addr);
immediate = immGen.GenImmNumbers(&(curr->instr)); // generate immediate number accordingly
//cout << immediate <<endl;
return true;
}


bool CPU::Execution(instruction* instr)
{
	int ALUin1;
	int ALUin2;
	bitset<4> ALUControlSignal;
	bitset<2> ALUop;  
	bitset<1> instr30;
	bitset<3> func3;
    
	/*Local Variables*/
	ALUin1 = readData1;
	ALUin2 = ((controlSignals.to_string()).substr(7,1) == "1") ? (immediate) : readData2; // 2-1 mux
	ALUop = bitset<2>((controlSignals.to_string()).substr(4,2));
	instr30 = bitset<1>(((instr->instr).to_string()).substr(1,1));
	func3 = bitset<3>(((instr->instr).to_string()).substr(17,3));
	ALUControlSignal = alucontroller.genALUcontrol(&ALUop, &instr30, &func3);
    
    /*Update CPU wires*/
	ALUresult = alu.Compute(&ALUin1, &ALUin2, &ALUControlSignal, &Less); // Perform ALU Computation
	PC_plus_offset = PC + immediate;
	//cout<< "ALUControlSignal: " << ALUControlSignal << "  ";
	//cout<< "ALUresult: " << ALUresult << endl;
	return true;
}

bool CPU::MemoryAccess()
{
	/*local variable*/
	string memWrite;
	string memRead;
	memWrite = (controlSignals.to_string()).substr(6,1);
	memRead = (controlSignals.to_string()).substr(2,1);
	
	/*Update CPU wires*/
	if(memWrite == "1")
		dmemory[ALUresult] = readData2;
	if(memRead == "1")
		data_from_mem = dmemory[ALUresult];

	/*next PC selection*/
	string select;
	select = (controlSignals.to_string()).substr(0,2);
	//cout<<controlSignals<<endl;
	if(select=="00") // no JALR, no Branch
		PC_next = PC_plus_4;
	else if(select=="10") // JALR
	    PC_next = ALUresult;
	else if((select == "01") && ((Less.to_string()) == "1" )) // Branch taken
		PC_next = PC_plus_offset;
	else    // branch not taken
		PC_next = PC_plus_4;
	
	/*update PC*/
	PC = PC_next;
	//cout<<Less<<endl;
	//cout << "next_PC is "<<PC<<endl;
	return true;
}
bool CPU::WriteBack()
{
	string MemtoReg;
	string JALR;
	bitset<1> RegWrite;
	int WriteData_mid;
	MemtoReg = (controlSignals.to_string()).substr(3,1);
	JALR = (controlSignals.to_string()).substr(0,1);

	WriteData_mid = (MemtoReg == "1") ? data_from_mem : ALUresult; // 2-1 mux
	WriteData = (JALR == "1") ? PC_plus_4 : WriteData_mid;
	RegWrite = bitset<1>((controlSignals.to_string()).substr(8,1));
	registers.write(&WriteData, &rdAddr, &RegWrite);
	return true;
}
unsigned long CPU::readPC()
{
	return PC;
}

int CPU::readRegister(int addr)
{
	bitset<5> addr_bit(addr);
	return registers.read(&addr_bit) ;
}

RegisterFile::RegisterFile()
{
	for(int i=0;i<32;i++)
		regFile[i] = 0;
}

int RegisterFile::read(bitset<5> *rsaddr)
{
	return regFile[(*rsaddr).to_ulong()];
}

void RegisterFile::write(int *writedata, bitset<5> *rdaddr, bitset<1> *enable)
{
	if((*enable).to_string() == "1") 
		regFile[(*rdaddr).to_ulong()] = *writedata;
}

bitset<9> Controller::genControlSignal(bitset<7> *opcode)
{
	bitset<9> signal;
	//cout << state << endl;
	switch((*opcode).to_ulong()){
		case 51: signal = bitset<9>(17); state = "Rtype";  break;
		case 19: signal = bitset<9>(27); state = "Itype";  break;
		case  3: signal = bitset<9>(99); state = "LW";     break;
		case 35: signal = bitset<9>(6);  state = "SW";     break;
		case 99: signal = bitset<9>(136);state = "BLT";    break;
		case 103:signal = bitset<9>(259);state = "JALR";   break;
		default: signal.reset(); break; // set to 0
	}
	return signal;
}

string Controller::getState()
{
	return state;
}


int ImmGen::GenImmNumbers(bitset<32>* inst)
{
	bitset<32> imm;
	bitset<7> opcode;
	string inst_string, sign_bit, sign_extension_20, sign_extension_19;
	for(int i=0;i<opcode.size();i++)
		opcode[i] = (*inst)[i]; // generate opcode field
	inst_string = (*inst).to_string();
	sign_bit = inst_string.substr(0,1);
	for(int i=0;i<20;i++)
		sign_extension_20 = sign_extension_20 + sign_bit;
	for(int i=0;i<19;i++)
		sign_extension_19 = sign_extension_19 + sign_bit;
	switch (opcode.to_ulong())  // generate immediate number based on instruction type 
	{
	case 51: imm.reset(); break; // R-type doesn't use imm field, just set 0s
    case 19: imm = bitset<32>(sign_extension_20 + inst_string.substr(0,12)); break; // I-type 
	case  3: imm = bitset<32>(sign_extension_20 + inst_string.substr(0,12)); break; // LW, same as I-type
	case 35: imm = bitset<32>(sign_extension_20 + inst_string.substr(0,7) + inst_string.substr(20,5)); break; // SW
	case 99: imm = bitset<32>(sign_extension_19 + inst_string.substr(0,1) +  inst_string.substr(24,1) 
	         + inst_string.substr(1,6) + inst_string.substr(20,4) + "0"); break; // BLT, immediate shift left 
	case 103:imm = bitset<32>(sign_extension_20 + inst_string.substr(0,12)); break; // JALR
	}
	bitset<32>* ptr_imm;
	ptr_imm = &imm;
	
	return bits_to_int(ptr_imm);
};

int ALU::Compute(int* inA, int* inB, bitset<4>* ALUcontrol, bitset<1>* Less)
{
	int ALUresult;
	switch ((*ALUcontrol).to_ulong())
	{
	case 0: ALUresult = *inA + *inB;      operation="add"; break; // 0000 addition
	case 1: ALUresult = *inA - *inB;      operation="sub"; break; // 0001 subtraction
	case 2: ALUresult = (*inA) ^ (*inB);  operation="xor"; break; // 0010 xor
	case 3: ALUresult = (*inA) >> (*inB); operation="sra"; break; // 0011 sra
	case 4: ALUresult = (*inA) & (*inB);  operation="and"; break; // 0100 and
	}
	*Less = bitset<1>(ALUresult < 0); // sign bit of the result, indicating whether Reg[rs1] is less than Reg[rs2]

	return ALUresult;
};

bitset<4> ALUcontroller::genALUcontrol(bitset<2>* ALUop, bitset<1>* instr30, bitset<3>* func3)
{
	bitset<4> ALUcontrol;
	if ((*ALUop) == bitset<2>("00")) // should perform addition for lw, sw, jalr
		ALUcontrol = bitset<4>("0000");
	else if((*ALUop) == bitset<2>("01")) // should perform subtraction for blt
		ALUcontrol = bitset<4>("0001");
	else if((*ALUop) == bitset<2>("11")) // I-type
	{ 
		if((*func3) == bitset<3>("000"))
			ALUcontrol = bitset<4>("0000"); // addi
		else
			ALUcontrol = bitset<4>("0100"); // andi
	}
	else // R-type  
	{
		if((*func3) == bitset<3>("000"))
		{
			if((*instr30) == bitset<1>("0"))
				ALUcontrol = bitset<4>("0000"); // add
			else
				ALUcontrol = bitset<4>("0001"); // sub
		}
			
		else if((*func3) == bitset<3>("100")) // xor
			ALUcontrol = bitset<4>("0010");
		else if((*func3) == bitset<3>("101")) // sra
			ALUcontrol = bitset<4>("0011");
	}
	return ALUcontrol;
};

int Adder::Add(unsigned long* a, int* b)
{
	int sum;
	sum = *a + *b;
	return sum;
};