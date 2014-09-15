#include "stdafx.h"
#include "EightChipCPU.h"

/*
 * Making a singleton instance of our CPU.
 */
EightChipCPU* EightChipCPU::m_Instance = nullptr;
EightChipCPU* EightChipCPU::GetInstance(void)
{
	if(m_Instance == nullptr)
		m_Instance = new EightChipCPU();

	return m_Instance;
}

//------------------------------------------------------
EightChipCPU::EightChipCPU(void)
{}

//------------------------------------------------------
EightChipCPU::~EightChipCPU(void)
{}

//------------------------------------------------------
/**
 * The game is loaded into memory 0x200 as 0 - 1FFF is for interpreter.
 * All we need to do is to load the game into 0x200 and initialise
 * other variables.
 * The program counter is the variable used to point which part of memory
 * is currently executing. It should be set to the first instruction of the game
 * which is obviously, in this case, 0x200
 */
void EightChipCPU::CPUReset(void)
{
	m_AddressI = 0;
	m_ProgramCounter = 0x200;
	
	// Set all registers to 0
    memset(m_Registers, 0, sizeof(m_Registers)) ;
}

//------------------------------------------------------
/**
 * OpCodes are instructions that the CPU executes. We get OpCodes from the memory. 
 * The beginning of the game is located at the address 0x200 from where the program counter is initialized.
 * 0x200 is, therefore, the address from where we will get the first OpCode.
 * OpCodes are 16-bits long (WORD) while our memory is 8-bits.
 * In order to create a 2 bytes OpCode from an 1 byte address, we combine the byte
 * located in memory with the byte of the next element.
 *
 * Eg.
 * m_GameMemory[0x200] = 0xAB (10101011)
 * m_GameMemory[0x201] = 0xCD (11001101)
 * OpCode = 0xABCD (10101011 11001101).
 * 
 * Tip:
 * The first byte was shifted 8 places to make room for the second.
 */
WORD EightChipCPU::GetNextOpCode(void)
{
	WORD res = 0;

	// Retrieve the current byte
	res = m_GameMemory[m_ProgramCounter];

	// Shift the first byte by 8 to the MSB, 
	// and get the other byte on the LSB side.
	res = (res << 8) | m_GameMemory[m_ProgramCounter + 1];;

	// Move 2 bytes ahead (OpCode size) to the next 'instruction'.
	m_ProgramCounter += 2;

	return res;
}

//------------------------------------------------------
/*
 * Operation Codes are portion of instruction which indicates
 * the operation to be performed.
 * The Chip-8 has 35 Opcode which are all 16-bits long:
 *		- NNN	: address 
 *		- NN	: 8-bit constant
 *		- N	: 4-bit constant
 *		- X & Y: 4-bit register identifier.
 */

// CLS
// Clear the display
void EightChipCPU::Opcode00E0(void)
{
	// TBD: this should fill m_ScreenPixels with value 255 .
	// see. std::fill of <algorithm>
}

// RET
// Return from a subroutine
void EightChipCPU::Opcode00EE()
{
	// The interpreter sets the program counter to the address 
	// at the top of the stack. An element is substracted implicitly from stack.
	m_ProgramCounter = m_Stack.back();
	m_Stack.pop_back();
}

void EightChipCPU::OpcodeDXYN(WORD opcode)
{}

//------------------------------------------------------
void EightChipCPU::OpcodeEX9E(WORD opcode)
{}

void EightChipCPU::OpcodeEXA1(WORD opcode)
{}
		
//------------------------------------------------------
void EightChipCPU::Opcode8XY0(WORD opcode)
{}

void EightChipCPU::Opcode8XY1(WORD opcode)
{}

void EightChipCPU::Opcode8XY2(WORD opcode)
{}

void EightChipCPU::Opcode8XY3(WORD opcode)
{}

void EightChipCPU::Opcode8XY4(WORD opcode)
{}

void EightChipCPU::Opcode8XY5(WORD opcode)
{}

void EightChipCPU::Opcode8XY6(WORD opcode)
{}

void EightChipCPU::Opcode8XY7(WORD opcode)
{}

void EightChipCPU::Opcode8XYE(WORD opcode)
{}

//------------------------------------------------------
void EightChipCPU::OpcodeFX07(WORD opcode)
{}

void EightChipCPU::OpcodeFX0A(WORD opcode)
{}

void EightChipCPU::OpcodeFX15(WORD opcode)
{}

void EightChipCPU::OpcodeFX18(WORD opcode)
{}

void EightChipCPU::OpcodeFX1E(WORD opcode)
{}

void EightChipCPU::OpcodeFX29(WORD opcode)
{}

void EightChipCPU::OpcodeFX33(WORD opcode)
{}

void EightChipCPU::OpcodeFX55(WORD opcode)
{}

void EightChipCPU::OpcodeFX65(WORD opcode)
{}

//------------------------------------------------------
// JP addr
// Jump to location NNN
void EightChipCPU::Opcode1NNN(WORD opcode)
{
	// The interpreter sets the program counter to NNN.
	this->m_ProgramCounter = opcode & 0x0FFF;
}

// CALL addr
// Call subroutine at NNN
void EightChipCPU::Opcode2NNN(WORD opcode)
{
	// The interpreter increments the stack pointer (implicit using std::vector)
	// then puts the current PC on top of the stack.
	m_Stack.push_back(m_ProgramCounter);

	// The interpreter sets the program counter to NNN.
	m_ProgramCounter = opcode & 0x0FFF;
}

void EightChipCPU::Opcode3XNN(WORD opcode)
{}

void EightChipCPU::Opcode4XNN(WORD opcode)
{}

void EightChipCPU::Opcode5XY0(WORD opcode)
{}

void EightChipCPU::Opcode6XNN(WORD opcode)
{}

void EightChipCPU::Opcode7XNN(WORD opcode)
{}

void EightChipCPU::Opcode9XY0(WORD opcode)
{}

void EightChipCPU::OpcodeANNN(WORD opcode)
{}

void EightChipCPU::OpcodeBNNN(WORD opcode)
{}

void EightChipCPU::OpcodeCXNN(WORD opcode)
{}

//------------------------------------------------------
/**
 * Decode Operation Codes
 *
 *
 */
void EightChipCPU::DecodeOpCode0(void)
{}

void EightChipCPU::DecodeOpCode8(void)
{}

void EightChipCPU::DecodeOpCodeE(void)
{}

void EightChipCPU::DecodeOpCodeF(void)
{}

//------------------------------------------------------
/**
 * This method retrieves the OpCode existing within the next 
 * 16-bits memory block by evaluating the MSB then executes 
 * the corresponding OpCode function.
 * As it should be noted, the Most Significant Bit 'was' stored
 * first.
 */
void EightChipCPU::ExecuteNextOpCode(void)
{
	// Gets the value of the current OpCode
	WORD opcode = GetNextOpCode();

	// Evaluate and execute.
	switch(opcode & 0xF000)
	{
		case 0x0000: ; break;
		case 0x1000: ; break;
		case 0x2000: ; break;
		case 0x3000: ; break;
		case 0x4000: ; break;
		case 0x5000: ; break;
		case 0x6000: ; break;
		case 0x7000: ; break;
		case 0x8000: ; break;
		case 0x9000: ; break;
		case 0xA000: ; break;
		case 0xB000: ; break;
		case 0xC000: ; break;
		case 0xD000: ; break;
		case 0xE000: ; break;
		case 0xF000: ; break;
		default: break;
	}
}