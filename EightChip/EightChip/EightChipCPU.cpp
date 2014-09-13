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

	// Shift the current byte by 8 elements
	res = res << 8;

	// Get the next byte of the OpCode
	res = res | m_GameMemory[m_ProgramCounter + 1];

	// Move 2 bytes ahead (OpCode size).
	m_ProgramCounter = m_ProgramCounter + 2;

	return res;
}
