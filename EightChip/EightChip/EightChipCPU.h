#pragma once
#ifndef _EIGHTCHIP_CPU_INCLUDED_
#define _EIGHTCHIP_CPU_INCLUDED_

#include <string>
#include <vector>

//---------------------------------
// We need variables of sizes 8-bits / 16-bits (word)
// which are given by the following typedefs

typedef unsigned char		BYTE;	// 1 byte  (0 - 255)
typedef unsigned short int	WORD;	// 2 bytes (0 - 65535)

//---------------------------------
// Memory of 0xFFF bytes.
const int ROMSIZE = 0xFFF;

//---------------------------------
class EightChipCPU
{
	private:
		EightChipCPU(void);
	public:
		~EightChipCPU(void);

	public:
		static	EightChipCPU* GetInstance(void);
		static	EightChipCPU* m_Instance;

		/**
		* The graphics work by drawing sprites at a specific position (X,Y)
		* on screen. 
		* Every sprite has a width of 8 pixels and variable height.
		* Sprite data is stored in game memory at the address I register points to.
		* Each byte of this memory represents 1 line of the sprite to draw.
		* Each pixel can be represented by 1 bit of the byte (8-bits).
		* If the pixel bit is set to 0, it stays at the same state. Otherwise, it toggles.
		* If it toggles on to off, the flag register gets set to 1. Otherwise, to 0.
		* OpCode DXYN is responsible for the graphics drawings.
		* The resolution can be represented as an array.
		*/
		BYTE	m_ScreenPixels[640][320][3];

		void	CPUReset(void);
		WORD	GetNextOpCode(void);

	private:
		BYTE	m_GameMemory[ROMSIZE];		// 0xFFF bytes of memory.		
		BYTE	m_Registers[16];			// 16 registers, 8-bits (BYTE) each.
											// called V0 to VF. VF doubles as the carry flag.		
		
		WORD	m_AddressI;					// 16-bits address register used to access memory
		WORD	m_ProgramCounter;			// 16-bits program counter
		
		std::vector<WORD> m_Stack;			// 16-bits stack for return addresses to memory
											// which is used for subroutine return calls.
											// This is emulated thanks to the pushback/pop.
};										


#endif
