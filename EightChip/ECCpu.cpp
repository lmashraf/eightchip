#include "stdafx.h"

#include "windows.h"

#include "ECCpu.h"

//-------------------------------------------------------------------------------------------------

EightChipCPU* EightChipCPU::m_Instance = nullptr;

//-------------------------------------------------------------------------------------------------

EightChipCPU*
EightChipCPU::GetInstance( )
{
    if ( m_Instance == nullptr )
        m_Instance = new EightChipCPU( );

    return m_Instance;
}

//-------------------------------------------------------------------------------------------------

EightChipCPU::EightChipCPU( )
{
}

//-------------------------------------------------------------------------------------------------

EightChipCPU::~EightChipCPU( )
{
}

//-------------------------------------------------------------------------------------------------
/** Initializes the Rom into memory to be loaded. */
bool
EightChipCPU::InitRom( const std::string& rom_filename )
{
    // Reset CPU
    CPUReset( );

    // CLS: Clear screen
    OpCode00E0( );

    // Load the game
    FILE* rom;
    rom = fopen( rom_filename.c_str( ), "rb" );

    // Check if the rom exists
    if ( rom == NULL )
        return false;

    // Reads the rom starting 0x200
    fread( &m_GameMemory[ 0x200 ], ROMSIZE, 1, rom );
    fclose( rom );

    return true;
}

//-------------------------------------------------------------------------------------------------
/** Decreases the timers. */
void
EightChipCPU::DecreaseTimers( )
{
    if ( m_DelayTimer > 0 )
        m_DelayTimer--;

    if ( m_SoundTimer > 0 )
    {
        m_SoundTimer--;
        PlayBeep( );
    }
}

//-------------------------------------------------------------------------------------------------
/** Both methods below are self-explanatory. */
void
EightChipCPU::KeyDown( int key )
{
    m_KeyState[ key ] = 1;
}

void
EightChipCPU::KeyUp( int key )
{
    m_KeyState[ key ] = 0;
}

//-------------------------------------------------------------------------------------------------

void
EightChipCPU::PlayBeep( )
{
// This part could be replaced later by using OpenAL or SDL_mixer
#ifdef _WIN32
    Beep( 440, 300 );
#endif
}

//-------------------------------------------------------------------------------------------------
/** The game is loaded into memory 0x200, since the interval 0 - 1FFF is reserved for the
 * interpreter.
 * All we need to do is to load the game into 0x200 and initialise other variables.
 * The program counter is the variable used to point which part of memory is currently executing.
 * It should be set to the first instruction of the game which is obviously, in this case, 0x200
 */
void
EightChipCPU::CPUReset( )
{
    m_AddressI = 0;
    m_ProgramCounter = 0x200;

    // Set all registers to 0
    memset( m_Registers, 0, sizeof( m_Registers ) );
    memset( m_GameMemory, 0, sizeof( m_GameMemory ) );
    memset( m_KeyState, 0, sizeof( m_KeyState ) );

    // Initialise timers
    m_DelayTimer = 0;
    m_SoundTimer = 0;
}

//-------------------------------------------------------------------------------------------------
/** The Chip8 has a HEX based keypad (0x0 to 0xF): Gets the current state of the key. */
int
EightChipCPU::GetKeyPressed( )
{
    // initialise key value
    int key = -1;

    //  skim through the keypad
    for ( int i = 0; i < 10; i++ )
    {
        // checks if key is pressed, if true, return its value.
        if ( m_KeyState[ i ] > 0 )
            return i;
    }

    // -1 is returned if no key is pressed.
    return key;
}

//-------------------------------------------------------------------------------------------------
/**
 * OpCodes are instructions that the CPU executes. We get OpCodes from the memory.
 * The beginning of the game is located at the address 0x200 from where the program counter is
 * initialized.
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
WORD
EightChipCPU::GetNextOpCode( )
{
    WORD res = 0;

    // Retrieve the current byte
    res = m_GameMemory[ m_ProgramCounter ];

    // Shift the first byte by 8 to the MSB,
    // and get the other byte on the LSB side.
    //	res = (res << 8) | m_GameMemory[m_ProgramCounter + 1];
    res <<= 8;
    res |= m_GameMemory[ m_ProgramCounter + 1 ];

    // Move 2 bytes ahead (OpCode size) to the next 'instruction'.
    m_ProgramCounter += 2;

    return res;
}

//-------------------------------------------------------------------------------------------------
/*
 * Operation Codes are portion of instruction which indicates
 * the operation to be performed.
 * The Chip-8 has 35 OpCode which are all 16-bits long:
 *		- KKK	: address
 *		- KK	: 8-bit constant
 *		- K	: 4-bit constant
 *		- X & Y: 4-bit register identifier.
 */
//-------------------------------------------------------------------------------------------------
// CLS
// Clear the display
void
EightChipCPU::OpCode00E0( )
{
    for ( int x = 0; x < WINDOW_WIDTH; x++ )
    {
        for ( int y = 0; y < WINDOW_HEIGHT; y++ )
        {
            m_ScreenPixels[ y ][ x ][ 0 ] = 0xFF;  // R
            m_ScreenPixels[ y ][ x ][ 1 ] = 0xFF;  // G
            m_ScreenPixels[ y ][ x ][ 2 ] = 0xFF;  // B
        }
    }
}

//-------------------------------------------------------------------------------------------------
// RET
// Return from a subroutine
void
EightChipCPU::OpCode00EE( )
{
    // The interpreter sets the program counter to the address
    // at the top of the stack. An element is subtracted implicitly from stack.
    m_ProgramCounter = m_Stack.back( );
    m_Stack.pop_back( );
}

//-------------------------------------------------------------------------------------------------
// DRW Vx, Vy, nibble
// Display n-byte sprite starting at memory location I
// at (Vx, Vy), set VF = collision
void
EightChipCPU::OpCodeDXYN( WORD opcode )
{
    // We will be scaling the window up 10 times.
    // ie. 64x32px to 1280x640px
    const int SCALE = 20;

    // Masks off the Vx and Vy registers
    int Vx = opcode & 0x0F00;
    Vx = Vx >> 8;

    int Vy = opcode & 0x00F0;
    Vy = Vy >> 4;

    // Calculate coordinates based on Vx, Vy
    // while taking account of scaling
    int spriteX = m_Registers[ Vx ] * SCALE;
    int spriteY = m_Registers[ Vy ] * SCALE;
    int spriteHeight = ( opcode & 0x000F );

    // Set collisions to 0
    m_Registers[ 0xF ] = 0x00;

    for ( int y_line = 0; y_line < spriteHeight; y_line++ )
    {
        // The interpreter reads n bytes from memory starting
        // the address stored in I.
        BYTE pixel = ( m_GameMemory[ m_AddressI + y_line ] );

        int x_line = 0;
        int x_line_inv = 7;

        for ( x_line = 0; x_line < 8; x_line++, x_line_inv-- )
        {
            // These bytes are then displayed as sprites on screen
            // at coordinates (Vx, Vy).
            int mask = 1 << x_line_inv;
            if ( pixel & mask )
            {
                // initialize coordinates and colours
                int x = ( x_line * SCALE ) + spriteX;
                int y = ( y_line * SCALE ) + spriteY;
                int colour = 0x00;  // black

                // If this causes any pixels to be erased, VF is set to 1
                // Otherwise it is set to 0.
                if ( m_ScreenPixels[ y ][ x ][ 0 ] == 0 )
                {
                    colour = 0xFF;  // white
                    m_Registers[ 0xF ] = 1;
                }

                // If the sprite is positioned so part of it is outside the
                // coordinates of the display, it wraps around  to opposite
                // direction of the screen.
                // Sprites are XOR'd onto existing screen,
                // (see. 8XY3 for XOR)
                for ( int i = 0; i < SCALE; i++ )
                {
                    for ( int j = 0; j < SCALE; j++ )
                    {
                        m_ScreenPixels[ y + i ][ x + j ][ 0 ] = colour;  // R
                        m_ScreenPixels[ y + i ][ x + j ][ 1 ] = colour;  // G
                        m_ScreenPixels[ y + i ][ x + j ][ 2 ] = colour;  // B
                    }
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------
// SKP Vx
// Skip next instruction if key with the value of Vx is pressed.
void
EightChipCPU::OpCodeEX9E( WORD opcode )
{
    // Masks off Vx register
    int Vx = opcode & 0x0F00;
    Vx >>= 8;

    // Checks the keyboard,
    int keypressed = m_Registers[ Vx ];

    // and if the key corresponding to the value of Vx
    // is currently down, PC is increased by 2
    if ( m_KeyState[ keypressed ] == 1 )
        m_ProgramCounter += 2;
}

//-------------------------------------------------------------------------------------------------
// SKNP Vx
// Skip next instruction if key with the value of Vx is pressed.
void
EightChipCPU::OpCodeEXA1( WORD opcode )
{
    // Masks off Vx register
    int Vx = opcode & 0x0F00;
    Vx >>= 8;

    // Checks the keyboard,
    int keypressed = m_Registers[ Vx ];

    // and if the key corresponding to the value of Vx
    // is currently in the up position, PC is incremented by 2
    if ( m_KeyState[ keypressed ] == 0 )
        m_ProgramCounter += 2;
}

//-------------------------------------------------------------------------------------------------
// LD Vx, Vy
// Set Vx = Vy
void
EightChipCPU::OpCode8XY0( WORD opcode )
{
    // Masks off the Vx and Vy registers
    int Vx = opcode & 0x0F00;
    int Vy = opcode & 0x00F0;
    Vx >>= 8;
    Vy >>= 4;

    // Stores the value of register Vy in register Vx
    m_Registers[ Vx ] = m_Registers[ Vy ];
}

//-------------------------------------------------------------------------------------------------
// OR Vx, Vy
// Set Vx = Vx OR Vy
void
EightChipCPU::OpCode8XY1( WORD opcode )
{
    // Masks off the Vx and Vy registers
    int Vx = opcode & 0x0F00;
    int Vy = opcode & 0x00F0;
    Vx >>= 8;
    Vy >>= 4;

    // Performs a bitwise OR on the values of Vx and Vy,
    // then stores the result in Vx.
    m_Registers[ Vx ] = m_Registers[ Vx ] | m_Registers[ Vy ];
}

//-------------------------------------------------------------------------------------------------
// AND Vx, Vy
// Set Vx = Vx AND Vy
void
EightChipCPU::OpCode8XY2( WORD opcode )
{
    // Masks off the Vx and Vy registers
    int Vx = opcode & 0x0F00;
    int Vy = opcode & 0x00F0;
    Vx >>= 8;
    Vy >>= 4;

    // Performs a bitwise AND on the values of Vx and Vy,
    // then stores the result in Vx.
    m_Registers[ Vx ] = m_Registers[ Vy ] & m_Registers[ Vy ];
}

//-------------------------------------------------------------------------------------------------
// XOR Vx, Vy
// Set Vx = Vx XOR Vy
void
EightChipCPU::OpCode8XY3( WORD opcode )
{
    // Masks off the Vx and Vy registers
    int Vx = opcode & 0x0F00;
    int Vy = opcode & 0x00F0;
    Vx >>= 8;
    Vy >>= 4;

    // Performs a bitwise XOR on the values of Vx and Vy,
    // then stores the result in Vx.
    m_Registers[ Vx ] = m_Registers[ Vx ] ^ m_Registers[ Vy ];
}

//-------------------------------------------------------------------------------------------------
// ADD Vx, Vy
// Set Vx = Vx + Vy, set VF = carry
void
EightChipCPU::OpCode8XY4( WORD opcode )
{
    // VF is set to 0, see below.
    m_Registers[ 0xF ] = 0;

    // Masks off the Vx and Vy registers
    int Vx = opcode & 0x0F00;
    int Vy = opcode & 0x00F0;
    Vx >>= 8;
    Vy >>= 4;

    int value = m_Registers[ Vx ] + m_Registers[ Vy ];

    // The values of Vx and Vy are added together.
    // If the result is greater than 8 bits (0xFF = 255)
    // VF is set to 1, otherwise it's set to 0
    // Only the lowest (LSB) 8 bits of the result are kept.
    if ( value > 0xFF )
        m_Registers[ 0xF ] = 1;

    m_Registers[ Vx ] = m_Registers[ Vx ] + m_Registers[ Vy ];
}

//-------------------------------------------------------------------------------------------------
// SUB Vx, Vy
// Set Vx = Vx - Vy, set VF = NOT borrow
void
EightChipCPU::OpCode8XY5( WORD opcode )
{
    // VF is set to 0, see below.
    m_Registers[ 0xF ] = 1;

    // Masks off the Vx and Vy registers
    int Vx = opcode & 0x0F00;
    int Vy = opcode & 0x00F0;
    Vx >>= 8;
    Vy >>= 4;

    // If Vx > Vy, then VF is set to 1, otherwise 0.
    if ( m_Registers[ Vx ] < m_Registers[ Vy ] )
        m_Registers[ 0xF ] = 0;

    // Then subtract Vy from Vy, and store the result in Vx.
    m_Registers[ Vx ] = m_Registers[ Vx ] - m_Registers[ Vy ];
}

//-------------------------------------------------------------------------------------------------
// SHR Vx {, Vy}
// Set Vx = Vx SHR 1
void
EightChipCPU::OpCode8XY6( WORD opcode )
{
    // Masks off the Vx register
    int Vx = opcode & 0x0F00;
    Vx >>= 8;

    // If the LSB of Vx is 1, the VF is set to 1
    // Otherwise, it's set to 0.
    m_Registers[ 0xF ] = m_Registers[ Vx ] & 0x1;

    // Vx is, then, divided by 2.
    m_Registers[ Vx ] >>= 1;
}

//-------------------------------------------------------------------------------------------------
// SUBN Vx, Vy
// Set Vx = Vy - Vx, set VF = NOT borrow.
void
EightChipCPU::OpCode8XY7( WORD opcode )
{
    // VF is set to 0, see below.
    m_Registers[ 0xF ] = 1;

    // Masks off the Vx and Vy registers
    int Vx = opcode & 0x0F00;
    int Vy = opcode & 0x00F0;
    Vx >>= 8;
    Vy >>= 4;

    // If Vy > Vx, then VF is set to 1, otherwise 0.
    // Then Vx is subtracted from Vy, and the results stored in Vx.
    if ( m_Registers[ Vx ] > m_Registers[ Vy ] )
        m_Registers[ 0xF ] = 0;

    m_Registers[ Vx ] = m_Registers[ Vy ] - m_Registers[ Vx ];
}

//-------------------------------------------------------------------------------------------------
// SHL Vx {, Vy}
// Set Vx = Vx SHL Vy
void
EightChipCPU::OpCode8XYE( WORD opcode )
{
    // Masks off the Vx register
    int Vx = opcode & 0x0F00;
    Vx >>= 8;

    // If the MSB of Vx is 1, then VF is set to 1, otherwise to 0.
    m_Registers[ 0xF ] = m_Registers[ Vx ] >> 7;

    // Then Vx is multiplied by 2.
    m_Registers[ Vx ] <<= 1;
}

//-------------------------------------------------------------------------------------------------
// LD Vx, DT
// Set Vx = delay timer value
void
EightChipCPU::OpCodeFX07( WORD opcode )
{
    int Vx = opcode & 0x0F00;
    Vx >>= 8;

    // The value of DT is placed into Vx
    m_Registers[ Vx ] = m_DelayTimer;
}

//-------------------------------------------------------------------------------------------------
// LD Vx, K
// Wait for a key press, store the value of the key in Vx
void
EightChipCPU::OpCodeFX0A( WORD opcode )
{
    // masks off Vx register
    int Vx = opcode & 0x0F00;
    Vx >>= 8;

    // Retrieve the current keypad's state.
    int keypressed = GetKeyPressed( );

    if ( keypressed = -1 )
    {
        // All execution stops until a key is pressed,
        m_ProgramCounter -= 2;
    }
    else
    {
        // then the value of that key is stored in Vx.
        m_Registers[ Vx ] = keypressed;
    }
}

//-------------------------------------------------------------------------------------------------
// LD DT, Vx
// Set delay timer = Vx
void
EightChipCPU::OpCodeFX15( WORD opcode )
{
    // masks off Vx register
    int Vx = opcode & 0x0F00;
    Vx >>= 8;

    // DT is set equal to the value of Vx
    m_DelayTimer = m_Registers[ Vx ];
}

//-------------------------------------------------------------------------------------------------
// LD ST, Vx
// Set sound timer = Vx
void
EightChipCPU::OpCodeFX18( WORD opcode )
{
    // masks off Vx register
    int Vx = opcode & 0x0F00;
    Vx >>= 8;

    // ST is set equal to the value of Vx
    m_SoundTimer = m_Registers[ Vx ];
}

//-------------------------------------------------------------------------------------------------
// ADD I, Vx
// Set I = I + Vx
void
EightChipCPU::OpCodeFX1E( WORD opcode )
{
    // masks off Vx register
    int Vx = opcode & 0x0F00;
    Vx >>= 8;

    // The values of I and Vx are summed,
    // The result is stored in I.
    m_AddressI += m_Registers[ Vx ];
}

//-------------------------------------------------------------------------------------------------
// LD F, Vx
// Set I = location of sprite of digit Vx
void
EightChipCPU::OpCodeFX29( WORD opcode )
{
    // masks off Vx register
    int Vx = opcode & 0x0F00;
    Vx >>= 8;

    // The value of I is set to the location for the hexadecimal sprite
    // corresponding to the value in Vx.
    // Characters 0-F (in hexadecimal) are represented by a 4x5 font.
    m_AddressI = m_Registers[ Vx ] * 5;
}

//-------------------------------------------------------------------------------------------------
// LD B, Vx
// Store Binary-coded decimal representation of Vx in memory locations I, I+1 and I+2
void
EightChipCPU::OpCodeFX33( WORD opcode )
{
    // masks off Vx register
    int Vx = opcode & 0x0F00;
    Vx >>= 8;

    // The interpreter takes the decimal value of Vx,
    int value = m_Registers[ Vx ];

    // and places the hundreds digit in memory at location in I,
    int hundreds = value / 100;
    // and tens digits in I+1,
    int tens = ( value / 10 ) % 10;
    // and the units digits in I+2.
    int units = value % 10;

    m_GameMemory[ m_AddressI ] = hundreds;
    m_GameMemory[ m_AddressI + 1 ] = tens;
    m_GameMemory[ m_AddressI + 2 ] = units;
}

//-------------------------------------------------------------------------------------------------
// LD [I], Vx
// Stores registers V0 through Vx in memory starting at location I
void
EightChipCPU::OpCodeFX55( WORD opcode )
{
    // masks off Vx register
    int Vx = opcode & 0x0F00;
    Vx >>= 8;

    // The interpreter copies the values of registers V0 through Vx
    // into memory, starting at the address in I
    for ( int i = 0; i <= Vx; i++ )
    {
        m_GameMemory[ m_AddressI + i ] = m_Registers[ i ];
    }

    m_AddressI = m_AddressI + Vx + 1;
}

//-------------------------------------------------------------------------------------------------
// LD Vx, [I]
// Read registers V0 through Vx from memory starting at location I
void
EightChipCPU::OpCodeFX65( WORD opcode )
{
    // masks off Vx register
    int Vx = opcode & 0x0F00;
    Vx >>= 8;

    // The interpreter reads values from memory starting at location I
    // into registers V0 through Vx.
    for ( int i = 0; i <= Vx; i++ )
    {
        m_Registers[ i ] = m_GameMemory[ m_AddressI + i ];
    }

    m_AddressI = m_AddressI + Vx + 1;
}

//-------------------------------------------------------------------------------------------------
// JP addr
// Jump to location KKK
void
EightChipCPU::OpCode1KKK( WORD opcode )
{
    // The interpreter sets the program counter to kkk.
    this->m_ProgramCounter = opcode & 0x0FFF;
}

//-------------------------------------------------------------------------------------------------
// CALL addr
// Call subroutine at KKK
void
EightChipCPU::OpCode2KKK( WORD opcode )
{
    // The interpreter increments the stack pointer (implicit using std::vector)
    // then puts the current PC on top of the stack.
    m_Stack.push_back( m_ProgramCounter );

    // The interpreter sets the program counter to KKK.
    m_ProgramCounter = opcode & 0x0FFF;
}

//-------------------------------------------------------------------------------------------------
// SE Vx, byte
// Skip next instruction if Vx = kk
void
EightChipCPU::OpCode3XKK( WORD opcode )
{
    // masks off kk value
    int kk = opcode & 0x00FF;
    // masks off register Vx, then shifts it across.
    int Vx = opcode & 0x0F00;
    Vx >>= 8;

    // The interpreter compares the register Vx to kk
    // If they are equal, the PC is incremented by 2 bytes (to the next OpCode).
    if ( m_Registers[ Vx ] == kk )
        m_ProgramCounter += 2;
}

//-------------------------------------------------------------------------------------------------
// SNE Vx, byte
// Skip next instruction if Vx != kk
void
EightChipCPU::OpCode4XKK( WORD opcode )
{
    // masks off kk value
    int kk = opcode & 0x00FF;
    // masks off register Vx, then shifts it across.
    int Vx = opcode & 0x0F00;
    Vx >>= 8;

    // The interpreter compares the register Vx to kk
    // If they are not equal, the PC is incremented by 2 bytes.
    if ( m_Registers[ Vx ] != kk )
        m_ProgramCounter += 2;
}

//-------------------------------------------------------------------------------------------------
// SE Vx, Vy
// Skip next instruction if Vx = Vy
void
EightChipCPU::OpCode5XY0( WORD opcode )
{
    // masks off registers Vx and Vy
    int Vx = opcode & 0x0F00;
    int Vy = opcode & 0x00F0;
    // shifts Vx and Vy across adequately
    Vx >>= 8;
    Vy >>= 4;

    // The interpreter compares registers Vx and Vy
    // If they are equal, the PC is incremented by 2 bytes.
    if ( m_Registers[ Vx ] == m_Registers[ Vy ] )
        m_ProgramCounter += 2;
}

//-------------------------------------------------------------------------------------------------
// LD Vx, byte
// Set Vx = kk
void
EightChipCPU::OpCode6XKK( WORD opcode )
{
    // masks off KK value and register Vx
    int kk = opcode & 0x00FF;
    int Vx = opcode & 0x0F00;
    // shifts across register Vx
    Vx >>= 8;

    // The interpreter puts the value of kk into register Vx
    m_Registers[ Vx ] = kk;
}

//-------------------------------------------------------------------------------------------------
// ADD Vx, byte
// Set Vx = Vx + kk, the carry is not affected.
void
EightChipCPU::OpCode7XKK( WORD opcode )
{
    // masks off kk and Vx values.
    int kk = opcode & 0x00FF;
    int Vx = opcode & 0x0F00;
    Vx >>= 8;

    // Adds the value of kk to the value of the register Vx,
    // then stores the result in Vx.
    m_Registers[ Vx ] += kk;
}

//-------------------------------------------------------------------------------------------------
// LD Vx, Vy
// Skip the next instruction if Vx != Vy
void
EightChipCPU::OpCode9XY0( WORD opcode )
{
    // masks off registers Vx and Vy
    int Vx = opcode & 0x0F00;
    int Vy = opcode & 0x00F0;
    // shifts Vx and Vy across adequately
    Vx >>= 8;
    Vy >>= 4;

    // Increment program counter if Vx != Vy
    if ( m_Registers[ Vx ] != m_Registers[ Vy ] )
        m_ProgramCounter += 2;
}

//-------------------------------------------------------------------------------------------------
// LD I, addr
// Set I = nnn
void
EightChipCPU::OpCodeANNN( WORD opcode )
{
    // masks off nnn
    int nnn = opcode & 0x0FFF;

    // The value of register I is set to nnn
    m_AddressI = nnn;
}

//-------------------------------------------------------------------------------------------------
// JP V0, addr
// Jump to location nnn + V0
void
EightChipCPU::OpCodeBNNN( WORD opcode )
{
    // masks off nnn
    int nnn = opcode & 0x0FFF;

    // The program counter is set to nnn plus the value of V0
    m_ProgramCounter = m_Registers[ 0 ] + nnn;
}

//-------------------------------------------------------------------------------------------------
// RND Vx, byte
// Set Vx = random byte AND kk
void
EightChipCPU::OpCodeCXKK( WORD opcode )
{
    // masks off kk and the Vx register
    int kk = opcode & 0x00FF;
    int Vx = opcode & 0x0F00;
    Vx >>= 8;

    // The interpreter generates a random number from 0 to 255
    // which is then AND'd with the value of kk.
    // The results are stored in Vx. (see. 8XY2 for more details on AND)
    //	m_Registers[Vx] = (rand() % 256) & kk;
    m_Registers[ Vx ] = rand( ) & kk;
}

//-------------------------------------------------------------------------------------------------
/**
 * Decode OpCodes grouped in 0, 8, E and F.
 */
//-------------------------------------------------------------------------------------------------
// Execute OpCodes of MSB matching the value 0
void
EightChipCPU::DecodeOpCode0( WORD opcode )
{
    switch ( opcode & 0xF )
    {
    case 0x0:
        OpCode00E0( );
        break;  // CLS
    case 0xE:
        OpCode00EE( );
        break;  // RET
    default:
        break;
    }
}

//-------------------------------------------------------------------------------------------------
// Execute OpCodes of MSB matching the value 8
void
EightChipCPU::DecodeOpCode8( WORD opcode )
{
    switch ( opcode & 0xF )
    {
    case 0x0:
        OpCode8XY0( opcode );
        break;  // OR  Vx, Vy
    case 0x1:
        OpCode8XY1( opcode );
        break;  // AND Vx, Vy
    case 0x2:
        OpCode8XY2( opcode );
        break;  // XOR Vx, Vy
    case 0x3:
        OpCode8XY3( opcode );
        break;  // ADD Vx, Vy
    case 0x4:
        OpCode8XY4( opcode );
        break;  // SUB Vx,  Vy
    case 0x5:
        OpCode8XY5( opcode );
        break;  // SHR Vx {, Vy}
    case 0x6:
        OpCode8XY6( opcode );
        break;  // SUBN Vx, Vy
    case 0x7:
        OpCode8XY7( opcode );
        break;  // SHL Vx {, Vy}
    case 0xE:
        OpCode8XYE( opcode );
        break;  // SNE Vx, Vy
    default:
        break;
    }
}

//-------------------------------------------------------------------------------------------------
// Execute OpCodes of MSB matching the value E
void
EightChipCPU::DecodeOpCodeE( WORD opcode )
{
    switch ( opcode & 0xF )
    {
    case 0xE:
        OpCodeEX9E( opcode );
        break;  // SKP Vx
    case 0x1:
        OpCodeEXA1( opcode );
        break;  // SKNP Vx
    default:
        break;
    }
}

//-------------------------------------------------------------------------------------------------
// Execute OpCodes of last two MSB matching the value F
void
EightChipCPU::DecodeOpCodeF( WORD opcode )
{
    switch ( opcode & 0xFF )
    {
    case 0x07:
        OpCodeFX07( opcode );
        break;  // LD Vx, DT
    case 0x0A:
        OpCodeFX0A( opcode );
        break;  // LD Vx, k
    case 0x15:
        OpCodeFX15( opcode );
        break;  // LD DT, Vx
    case 0x18:
        OpCodeFX18( opcode );
        break;  // LD ST, Vx
    case 0x1E:
        OpCodeFX1E( opcode );
        break;  // ADD I, Vx
    case 0x29:
        OpCodeFX29( opcode );
        break;  // LD F, Vx
    case 0x33:
        OpCodeFX33( opcode );
        break;  // LD B, Vx
    case 0x55:
        OpCodeFX55( opcode );
        break;  // LD [I], Vx
    case 0x65:
        OpCodeFX65( opcode );
        break;  // LD Vx, [I]
    default:
        break;
    }
}

//-------------------------------------------------------------------------------------------------
/**
 * This method reads the OpCode from the next 16-bits memory block by evaluating the MSB
 * then executes the corresponding OpCode function.
 * As it should be noted, the Most Significant Bit 'was' stored first.
 */
void
EightChipCPU::ExecuteNextOpCode( )
{
    // Gets the value of the current OpCode
    WORD opcode = GetNextOpCode( );

    // Evaluate and execute.
    switch ( opcode & 0xF000 )
    {
    case 0x0000:
        DecodeOpCode0( opcode );
        break;  // see. DecodeOpCode0 definition
    case 0x1000:
        OpCode1KKK( opcode );
        break;  // JP addr
    case 0x2000:
        OpCode2KKK( opcode );
        break;  // CALL addr
    case 0x3000:
        OpCode3XKK( opcode );
        break;  // SE Vx, byte
    case 0x4000:
        OpCode4XKK( opcode );
        break;  // SNE Vx, byte
    case 0x5000:
        OpCode5XY0( opcode );
        break;  // SE Vx, Vy
    case 0x6000:
        OpCode6XKK( opcode );
        break;  // LD Vx, byte
    case 0x7000:
        OpCode7XKK( opcode );
        break;  // ADD Vx, byte
    case 0x8000:
        DecodeOpCode8( opcode );
        break;  // see. DecodeOpCode8 definition
    case 0x9000:
        OpCode9XY0( opcode );
        break;  // SNE Vx, Vy
    case 0xA000:
        OpCodeANNN( opcode );
        break;  // LD I, addr
    case 0xB000:
        OpCodeBNNN( opcode );
        break;  // JP V0, addr
    case 0xC000:
        OpCodeCXKK( opcode );
        break;  // RND Vx, byte
    case 0xD000:
        OpCodeDXYN( opcode );
        break;  // DRW Vx, Vy, nibble
    case 0xE000:
        DecodeOpCodeE( opcode );
        break;  // see. DecodeOpCodeE definition
    case 0xF000:
        DecodeOpCodeF( opcode );
        break;  // see. DecodeOpCodeF definition
    default:
        break;
    }
}

//-------------------------------------------------------------------------------------------------