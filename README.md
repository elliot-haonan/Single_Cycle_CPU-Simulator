# Single_Cycle_CPU-Simulator
This program implements a RISC-V single cycle CPU simulator
## Design Schematic
![image](https://user-images.githubusercontent.com/107291837/201617723-5d8f4f5d-635f-4ee9-ace9-e2fa84408525.png)
## Introduction
This CPU supports 10 instructions from RV32I instruction set. The control signals for each of the instruction is shown as follows:
<img width="754" alt="image" src="https://user-images.githubusercontent.com/107291837/201617923-ddc6aa86-e82a-4ff0-b0da-4f6b33c547ee.png">
## Running
First, compile with the following command:
```bash
g++ *.cpp -o cpusim
```
To run the CPU simulator, run the following command:
```bash
./cpusim <inputfile.txt>
```
## Input 
An example of input file is given in the "trace.txt". Each line is an unsigned integer and is 1 byte of an RISC-V instruction stored in little-endian. 

## Output
The output of the program is given by (a0,a1), which is the value of register a0 and a1.
