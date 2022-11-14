#include <iostream>
#include <bitset>
#include <stdio.h>
#include<stdlib.h>
#include <string>
using namespace std;

class instruction {
public:
	bitset<32> instr;//instruction
	instruction(bitset<32> fetch); // constructor
};

class RegisterFile{
private:
	int regFile[32]; // register file
public:
	RegisterFile(); //constructor
	int read(bitset<5> *rsaddr);
	void write(int *writedata, bitset<5> *rdaddr, bitset<1> *enable);
};

class Controller{
private:
    string state; 
public:
	Controller(string s="Initial"):state(s){};
	bitset<9> genControlSignal(bitset<7> *opcode);
	string getState();
};

class ImmGen{
public:
	int GenImmNumbers(bitset<32>* inst);
};

class ALU{
private:
	string operation; 
public:
	int Compute(int* inA, int* inB, bitset<4>* ALUop, bitset<1>* Less);
};

class ALUcontroller{
public:
	bitset<4> genALUcontrol(bitset<2>* ALUop, bitset<1>* imm30, bitset<3>* func3);
};

class Adder{
public:
	int Add(unsigned long* a, int* b);
};