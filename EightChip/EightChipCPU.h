/**
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 * Author: Mohammed Achraf LEBRAZI.
 */

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
		static	EightChipCPU* m_Instance;	// this instance

		BYTE	m_GameMemory[ROMSIZE];		// 0xFFF bytes of memory.
											// Memory Map:
											// 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
											// 0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
											// 0x200-0xFFF - Program ROM and work RAM

		BYTE	m_Registers[16];			// 16 registers, 8-bits (BYTE) each.
											// called V0 to VF. VF doubles as the carry flag.		
		
		BYTE	m_DelayTimer;				// Delay Timer active whenever register DT is non-zero.
											// This timer substracts 1 from DT at rate of 60Hz until 0 then desactivates.

		BYTE	m_SoundTimer;				// Sound Timer active whenever register ST is non-zero.
		BYTE	m_KeyState[16];				// This timer substracts 1 from ST at rate of 60Hz.
											// Chip-8 will buzz as long as ST is greater than 0.
	
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

		bool	LoadRom(const std::string& rom_filename);
		void	ExecuteNextOpCode(void);

		// Delay/Sound Timers decrements
		void	DecreaseTimers(void);

		// KeyStates
		void	KeyPressed(int key);
		void	KeyReleased(int key);

		// Screen
		BYTE	m_ScreenPixels[640][320][3];

	private:
		// Initialise CPU or Screen
		void	CPUReset(void);
		void	ClearScreen(void);

		// Play a sound. Only one tone is played, frequency is predefined.
		void	PlayBeep(void);

		// OpCodes reading and execution
		WORD	GetNextOpCode(void);

		//
		int		GetKeyPressed(void);

		// Clear screen and draw graphics. (DecodeOpCode0)
		void	OpCode00E0(void);
		void	OpCodeDXYN(WORD opcode);

		// Skips an instruction if keypressed or not. (DecodeOpCodeE)
		void	OpCodeEX9E(WORD opcode);
		void	OpCodeEXA1(WORD opcode);
		
		// Operations involving both VX and VY registers. (DecodeOpCode8)
		void	OpCode8XY0(WORD opcode);
		void	OpCode8XY1(WORD opcode);
		void	OpCode8XY2(WORD opcode);
		void	OpCode8XY3(WORD opcode);
		void	OpCode8XY4(WORD opcode);
		void	OpCode8XY5(WORD opcode);
		void	OpCode8XY6(WORD opcode);
		void	OpCode8XY7(WORD opcode);
		void	OpCode8XYE(WORD opcode);

		// Operations involving VX register. (DecodeOpCodeF)
		void	OpCodeFX07(WORD opcode);
		void	OpCodeFX0A(WORD opcode);
		void	OpCodeFX15(WORD opcode);
		void	OpCodeFX18(WORD opcode);
		void	OpCodeFX1E(WORD opcode);
		void	OpCodeFX29(WORD opcode);
		void	OpCodeFX33(WORD opcode);
		void	OpCodeFX55(WORD opcode);
		void	OpCodeFX65(WORD opcode);

		// Other operations
		void	OpCode1KKK(WORD opcode);
		void	OpCode2KKK(WORD opcode);
		void	OpCode3XKK(WORD opcode);
		void	OpCode4XKK(WORD opcode);
		void	OpCode5XY0(WORD opcode);
		void	OpCode6XKK(WORD opcode);
		void	OpCode7XKK(WORD opcode);
		void	OpCode9XY0(WORD opcode);
		void	OpCodeANNN(WORD opcode);
		void	OpCodeBNNN(WORD opcode);
		void	OpCodeCXKK(WORD opcode);
		void	OpCode00EE();
		
		// Decode OpCodes
		void	DecodeOpCode0(WORD opcode);
		void	DecodeOpCode8(WORD opcode);
		void	DecodeOpCodeE(WORD opcode);
		void	DecodeOpCodeF(WORD opcode);		
};										


#endif
