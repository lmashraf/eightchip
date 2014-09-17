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
void EightChipCPU::ClearScreen(void)
{}

//------------------------------------------------------
bool EightChipCPU::LoadRom(const std::string& rom_filename)
{
	// NIY:
	return false;
}

//------------------------------------------------------
void EightChipCPU::DecreaseTimers(void)
{}

//------------------------------------------------------
void EightChipCPU::KeyPressed(int key)
{}
//------------------------------------------------------
void EightChipCPU::KeyReleased(int key)
{}

//-----------------------------------------------------
void EightChipCPU::PlayBeep(void)
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
	memset(m_GameMemory, 0, sizeof(m_GameMemory));
	memset(m_KeyState, 0, sizeof(m_KeyState));

	// Initialise timers
	m_DelayTimer = 0;
	m_SoundTimer = 0;
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
 *		- KKK	: address 
 *		- KK	: 8-bit constant
 *		- K	: 4-bit constant
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
//------------------------------------------------------
void EightChipCPU::OpcodeDXYK(WORD opcode)
{}

//------------------------------------------------------
void EightChipCPU::OpcodeEX9E(WORD opcode)
{}
//------------------------------------------------------
void EightChipCPU::OpcodeEXA1(WORD opcode)
{}
		
//------------------------------------------------------
void EightChipCPU::Opcode8XY0(WORD opcode)
{}
//------------------------------------------------------
void EightChipCPU::Opcode8XY1(WORD opcode)
{}
//------------------------------------------------------
void EightChipCPU::Opcode8XY2(WORD opcode)
{}
//------------------------------------------------------
void EightChipCPU::Opcode8XY3(WORD opcode)
{}
//------------------------------------------------------
void EightChipCPU::Opcode8XY4(WORD opcode)
{}
//------------------------------------------------------
void EightChipCPU::Opcode8XY5(WORD opcode)
{}
//------------------------------------------------------
void EightChipCPU::Opcode8XY6(WORD opcode)
{}
//------------------------------------------------------
void EightChipCPU::Opcode8XY7(WORD opcode)
{}
//------------------------------------------------------
void EightChipCPU::Opcode8XYE(WORD opcode)
{}

//------------------------------------------------------
// LD Vx, DT
// Set Vx = delay timer value
void EightChipCPU::OpcodeFX07(WORD opcode)
{
	int regVx = opcode & 0x0F00;
	regVx >>= 8;

	// The value of DT is placed into Vx
	m_Registers[regVx] = m_DelayTimer;
}
//------------------------------------------------------
// LD Vx, K
// Wait for a key press, store the value of the key in Vx
void EightChipCPU::OpcodeFX0A(WORD opcode)
{
	// masks off Vx register
	int regVx = opcode & 0x0F00;
	regVx >>= 8;

	// All execution stops until a key is pressed,
	// then the value of that key is stored in Vx.
}
//------------------------------------------------------
// LD DT, Vx
// Set delay timer = Vx
void EightChipCPU::OpcodeFX15(WORD opcode)
{
	// masks off Vx register
	int regVx = opcode & 0x0F00;
	regVx >>= 8;

	// DT is set equal to the value of Vx
	m_DelayTimer = m_Registers[regVx];
}
//------------------------------------------------------
// LD ST, Vx
// Set sound timer = Vx
void EightChipCPU::OpcodeFX18(WORD opcode)
{
	// masks off Vx register
	int regVx = opcode & 0x0F00;
	regVx >>= 8;

	// ST is set equal to the value of Vx
	m_SoundTimer = m_Registers[regVx];
}
//------------------------------------------------------
// ADD I, Vx
// Set I = I + Vx
void EightChipCPU::OpcodeFX1E(WORD opcode)
{
	// masks off Vx register
	int regVx = opcode & 0x0F00;
	regVx >>= 8;

	// The values of I and Vx are summed,
	// The result is stored in I.
	m_AddressI += m_Registers[regVx];
}
//------------------------------------------------------
// LD F, Vx
// Set I = location of sprite of digit Vx
void EightChipCPU::OpcodeFX29(WORD opcode)
{
	// masks off Vx register
	int regVx = opcode & 0x0F00;
	regVx >>= 8;

	// The value of I is set to the location for the hexadecimal sprite
	// corresponding to the value in Vx.
	// Characters 0-F (in hexadecimal) are represented by a 4x5 font.
	m_AddressI = m_Registers[regVx] * 5;

}
//------------------------------------------------------
// LD B, Vx
// Store Binary-coded decimal representation of Vx in memory locations I, I+1 and I+2
void EightChipCPU::OpcodeFX33(WORD opcode)
{
	// masks off Vx register
	int regVx = opcode & 0x0F00;
	regVx >>= 8;

	// The interpreter takes the decimal value of Vx,
	int Vx = m_Registers[regVx];

	// and places the hundreds digit in memory at location in I,
	int hundreds			= Vx / 100;
	m_GameMemory[m_AddressI]	= hundreds;
	// and tens digits in I+1, 
	int tens				= (Vx / 10) % 10;
	m_GameMemory[m_AddressI+1]= tens;
	//and the units digists in I+2.
	int units	 = Vx % 10;	
	m_GameMemory[m_AddressI+2]= units;
}
//------------------------------------------------------
// LD [I], Vx
// Stores registers V0 through Vx in memory starting at location I
void EightChipCPU::OpcodeFX55(WORD opcode)
{
	// masks off Vx register
	int regVx = opcode & 0x0F00;
	regVx >>= 8;

	// The interpreter copies the values of registers V0 through Vx
	// into memory, starting at the address in I
	for(int i = 0; i <= regVx; i++)
	{
		m_GameMemory[m_AddressI + i] = m_Registers[i];
	}

	m_AddressI = m_AddressI + regVx + 1;
}
//------------------------------------------------------
// LD Vx, [I]
// Read registers V0 through Vx from memory starting at location I
void EightChipCPU::OpcodeFX65(WORD opcode)
{
	// masks off Vx register
	int regVx = opcode & 0x0F00;
	regVx >>= 8;

	// The interpreter reads values from memory starting at location I
	// into registers V0 through Vx.
	for (int i = 0 ; i <= regVx; i++)
	{
		m_Registers[i] = m_GameMemory[m_AddressI+i]  ;
	}

	m_AddressI= m_AddressI + regVx + 1 ;
}

//------------------------------------------------------
// JP addr
// Jump to location KKK
void EightChipCPU::Opcode1KKK(WORD opcode)
{
	// The interpreter sets the program counter to kkk.
	this->m_ProgramCounter = opcode & 0x0FFF;
}
//------------------------------------------------------
// CALL addr
// Call subroutine at KKK
void EightChipCPU::Opcode2KKK(WORD opcode)
{
	// The interpreter increments the stack pointer (implicit using std::vector)
	// then puts the current PC on top of the stack.
	m_Stack.push_back(m_ProgramCounter);

	// The interpreter sets the program counter to KKK.
	m_ProgramCounter = opcode & 0x0FFF;
}
//------------------------------------------------------
// SE Vx, byte
// Skip next instruction if Vx = kk
void EightChipCPU::Opcode3XKK(WORD opcode)
{
	// masks off kk value
	int kk	 = opcode & 0x00FF;
	// masks off register Vx, then shifts it across.
	int regVx = opcode & 0x0F00;
	regVx >>= 8;

	// The interpreter compares the register Vx to kk
	// If they are equal, the PC is incremented by 2 bytes (to the next Opcode).
	if(m_Registers[regVx] == kk)
		m_ProgramCounter += 2;
}
//------------------------------------------------------
// SNE Vx, byte
// Skip next instruction if Vx != kk
void EightChipCPU::Opcode4XKK(WORD opcode)
{
	// masks off kk value
	int kk    = opcode & 0x00FF;
	// masks off register Vx, then shifts it across.
	int regVx = opcode & 0x0F00;
	regVx >>= 8;

	// The interpreter compares the register Vx to kk
	// If they are not equal, the PC is incremented by 2 bytes.
	if(m_Registers[regVx] != kk)
		m_ProgramCounter += 2;
}
//------------------------------------------------------
// SE Vx, Vy
// Skip next instruction if Vx = Vy
void EightChipCPU::Opcode5XY0(WORD opcode)
{
	// masks off registers Vx and Vy
	int regVx = opcode & 0x0F00;
	int regVy = opcode & 0x00F0;
	// shifts Vx and Vy across adequately
	regVx >>= 8;
	regVy >>= 4;

	// The interpreter compares registers Vx and Vy
	// If they are equal, the PC is incremented by 2 bytes.
	if(m_Registers[regVx] == m_Registers[regVy])
		m_ProgramCounter += 2;
}
//------------------------------------------------------
// LD Vx, byte
// Set Vx = kk
void EightChipCPU::Opcode6XKK(WORD opcode)
{
	// masks off KK value and register Vx
	int kk    = opcode & 0x00FF;
	int regVx = opcode & 0x0F00;
	// shifts across register Vx
	regVx >>= 8 ;

	// The interpreter puts the value of kk into register Vx
	m_Registers[regVx] = kk;
}
//------------------------------------------------------
// ADD Vx, byte
// Set Vx = Vx + kk, the carry is not affected.
void EightChipCPU::Opcode7XKK(WORD opcode)
{
	// masks off kk and Vx values.
	int kk	  = opcode && 0x00FF;
	int regVx = opcode && 0x0F00;
	regVx >>= 8 ;
	
	// Adds the value of kk to the value of the register Vx,
	// then stores the result in Vx.
	m_Registers[regVx] += kk;
}
//------------------------------------------------------
// LD Vx, Vy
// Set Vx = Vy
void EightChipCPU::Opcode9XY0(WORD opcode)
{
	// masks off registers Vx and Vy
	int regVx = opcode & 0x0F00;
	int regVy = opcode & 0x00F0;
	// shifts Vx and Vy across adequately
	regVx >>= 8;
	regVy >>= 4;

	// Stores the value of register Vy in register Vx.
	m_Registers[regVx] = m_Registers[regVy];
}
//------------------------------------------------------
// LD I, addr
// Set I = nnn
void EightChipCPU::OpcodeANNN(WORD opcode)
{
	// masks off nnn
	int nnn = opcode & 0x0FFF;

	// The value of register I is set to nnn
	m_AddressI = nnn;
}
//------------------------------------------------------
// JP V0, addr
// Jump to location nnn + V0
void EightChipCPU::OpcodeBNNN(WORD opcode)
{
	// masks off nnn
	int nnn = opcode & 0x0FFF;

	// The program counter is set to nnn plus the value of V0
	m_ProgramCounter = m_Registers[0] + nnn;

}
//------------------------------------------------------
// RND Vx, byte
// Set Vx = random byte AND kk
void EightChipCPU::OpcodeCXKK(WORD opcode)
{
	// masks off kk and the Vx register
	int kk		= opcode & 0x00FF;
	int regVx   = opcode & 0x0F00;
	
	regVx >>= 8,

	// The interpreter generates a random number from 0 to 255
	// which is then AND'd with the value of kk.
	// The results are stored in Vx. (see. 8XY2 for more details on AND)
	m_Registers[regVx] = (rand() % 256) & kk;
}

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