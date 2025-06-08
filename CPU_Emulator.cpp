#include <iostream>
#include <vector>
#include <cstdint>
#define WORD_SIZE 16
#define MEM_SIZE 4096

/*
Registri:

AC (Accumulator)
IR (Instruction Register)
PC (Program Counter)

*/

uint16_t Memory[MEM_SIZE];

// Registri
uint16_t PC = 0;
uint16_t IR = 0;
uint16_t AC = 0;
uint16_t SA = 0;

// LIA Binary Format: [0111 4-Bit Opcode] [6-bit value 000000] [6-bit address 000000]
enum Opcodes {
    HLT = 0x0,
    LTA = 0x1,
    SFA = 0x2,
    ADD = 0x3,
    JMP = 0x4,
    JEZ = 0x5,
    JNZ = 0x6,
    LIA = 0x7,
    LAC = 0x8,
    SUB = 0x9,
    NOP = 0xA,
    ADDI = 0xB,
    SUBI = 0xC
};


uint16_t program[] = {
    0x7294, 
    0x4002,  
    0xA000,  
    0x1014,  
    0x8014,  
    0xC001,  
    0x2014,  
    0x5009,  
    0x4003,  
    0x0000 
};

void dumpmem() {
    for(int i = 0; i < MEM_SIZE; i++) {
        if(Memory[i] != 0 && !(i < (sizeof(program)/sizeof(program[0])))) {
            std::cout << "Memory[ADDRESS: " << i << "] holds value " << Memory[i] << std::endl;
        }
    }
}

void dumpreg() {
    std::cout << "Program Counter (PC): " << PC << "\n";
    std::cout << "Instruction Register (IR): " << IR << "\n";
    std::cout << "Accumulator (AC): " << AC << "\n";
}

void exec() {
    bool on = true;
    while(on) {
        IR = Memory[PC];
        PC++;
        uint16_t opcode = (IR >> 12) & 0xF;
        uint16_t address = IR & 0x0FFF;
        switch(opcode)
        {
            case HLT:
                std::cout << "HALT (HLT) Opcode: 0x0\n";
                on = false; 
                break;
            case LTA:
                std::cout << "LOAD TO ACCUMULATOR (LTA) Opcode: 0x1 | Address: " << address << std::endl;
                AC = Memory[address];
                break;
            case SFA:
                std::cout << "STORE FROM ACCUMULATOR (SFA) Opcode: 0x2 | Address: " << address << " Value: " << AC <<std::endl;
                Memory[address] = AC;
                break;
            case ADD:
                std::cout << "ADD (ADD) Opcode: 0x3 | Address: " << address << std::endl;
                AC += Memory[address];
                break;
            case JMP:
                std::cout << "JUMP TO INSTRUCTION (JMP) Opcode: 0x4 | Address: " << address << " | PC: " << PC << std::endl;
                PC = address;
                break;
            case JEZ:
                std::cout << "JUMP IF EQUAL TO ZERO (JEZ) Opcode: 0x5 | Address: " << address << " | Address Value: " << Memory[address] << " | PC: " << PC << " | SA: " << SA << " | SA Value: " << Memory[SA] << std::endl;
                if(Memory[SA] == 0) PC = address;
                break;
            case JNZ:
                std::cout << "JUMP IF NOT EQUAL TO ZERO (JNZ) Opcode: 0x6 | Address: " << address << " | Address Value: " << Memory[address] << " | PC: " << PC << " | SA: " << SA << std::endl;
                if(Memory[SA] != 0) PC = address;
                break;
            case LIA: {
                uint16_t value = (IR >> 6) & 0x3F; 
                uint16_t addr = IR & 0x3F;       
                std::cout << "LOAD IMMEDIATE TO ADDRESS (LIA) Opcode: 0x7 | Value: " << value << " to Memory[" << addr << "]\n";
                Memory[addr] = value;
                break;
            }
            case LAC:
                std::cout << "LOAD ADDRESS TO COMPARISON REGISTER (LAC) Opcode: 0x8 | Address: " << address << "\n";
                SA = address;
                break;
            case SUB:
                std::cout << "SUBCTRACT (SUB) Opcode: 0x9 | Address: " << address << std::endl;
                AC -= Memory[address];
                break;
            case SUBI:
                std::cout << "SUBCTRACT IMMEDIATE (SUBI) Opcode: 0xC | Value: " << address << std::endl;
                AC -= address;
                break;
            case ADDI:
                std::cout << "ADD IMMEDIATE (ADDI) Opcode: 0xB | Value: " << address << std::endl;
                AC += address;
                break;        
            case NOP:
                std::cout << "DO NOTHING (NOP) Opcode: 0xA\n";
                break;
            default:
                std::cout << "Unknow opcode " << opcode << ". Halting CPU";
                on = false;    
        }
    }
}


void load_program() {
    for (size_t i = 0; i < sizeof(program)/sizeof(program[0]); ++i) {
        Memory[i] = program[i];
    }
}


int main() {
    load_program();
    exec();
    dumpmem();
    dumpreg();
    return 0;
}
