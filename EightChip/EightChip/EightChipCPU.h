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
		BYTE	m_GameMemory[ROMSIZE];		// 0xFFF bytes of memory.
											// Memory Map:
											// 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
											// 0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
											// 0x200-0xFFF - Program ROM and work RAM

		BYTE	m_Registers[16];			// 16 registers, 8-bits (BYTE) each.
											// called V0 to VF. VF doubles as the carry flag.		
		
		WORD	m_AddressI;					// 16-bits address register used to access memory
		WORD	m_ProgramCounter;			// 16-bits program counter
		
		std::vector<WORD> m_Stack;			// 16-bits stack for return addresses to memory
											// which is used for subroutine return calls.
											// This is emulated thanks to the pushback/pop.
	private:
		EightChipCPU(void);
	public:
		~EightChipCPU(void);

		// Singleton
		static	EightChipCPU* GetInstance(void);
		static	EightChipCPU* m_Instance;

		// Screen
		BYTE	m_ScreenPixels[640][320][3];

		void	CPUReset(void);

		bool	LoadRom(const std::string& rom_filename);

		// OpCodes reading and execution
		WORD	GetNextOpCode(void);
		void	ExecuteNextOpCode(void);

		// Clear screen and draw graphics. (DecodeOpCode0)
		void	Opcode00E0(void);
		void	OpcodeDXYN(WORD opcode);

		// Skips an instruction if keypressed or not. (DecodeOpcodeE)
		void	OpcodeEX9E(WORD opcode);
		void	OpcodeEXA1(WORD opcode);
		
		// Operations involving both VX and VY registers. (DecodeOpcode8)
		void	Opcode8XY0(WORD opcode);
		void	Opcode8XY1(WORD opcode);
		void	Opcode8XY2(WORD opcode);
		void	Opcode8XY3(WORD opcode);
		void	Opcode8XY4(WORD opcode);
		void	Opcode8XY5(WORD opcode);
		void	Opcode8XY6(WORD opcode);
		void	Opcode8XY7(WORD opcode);
		void	Opcode8XYE(WORD opcode);

		// Operations involving VX register. (DecodeOpcodeF)
		void	OpcodeFX07(WORD opcode);
		void	OpcodeFX0A(WORD opcode);
		void	OpcodeFX15(WORD opcode);
		void	OpcodeFX18(WORD opcode);
		void	OpcodeFX1E(WORD opcode);
		void	OpcodeFX29(WORD opcode);
		void	OpcodeFX33(WORD opcode);
		void	OpcodeFX55(WORD opcode);
		void	OpcodeFX65(WORD opcode);

		// Other operations
		void	Opcode1NNN(WORD opcode);
		void	Opcode2NNN(WORD opcode);
		void	Opcode3XNN(WORD opcode);
		void	Opcode4XNN(WORD opcode);
		void	Opcode5XY0(WORD opcode);
		void	Opcode6XNN(WORD opcode);
		void	Opcode7XNN(WORD opcode);
		void	Opcode9XY0(WORD opcode);
		void	OpcodeANNN(WORD opcode);
		void	OpcodeBNNN(WORD opcode);
		void	OpcodeCXNN(WORD opcode);
		void	Opcode00EE();
		
		// Decode OpCodes
		void	DecodeOpCode0(void);
		void	DecodeOpCode8(void);
		void	DecodeOpCodeE(void);
		void	DecodeOpCodeF(void);		
};										


#endif
