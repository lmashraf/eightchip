#include "ECApp.h"

//-------------------------------------------------------------------------------------------------
EightChipApp::EightChipApp( )
{
    statusRunning = true;

    eightchip_cpu = EightChipCPU::GetInstance( );
}

//-------------------------------------------------------------------------------------------------
bool
EightChipApp::Initialise( )
{
    if ( !ecemulate::LoadSettings( this->settings ) )
    {
        ecsyst::LogError( ERR09 );
        delete this->eightchip_cpu;

        return false;
    }

    if ( !ecgfx::InitGraphics( this->main_window ) )
    {
        ecsyst::LogError( ERR04 );
        delete this->eightchip_cpu;

        return false;
    }

    if ( !ecemulate::LoadRom( this->eightchip_cpu, this->settings ) )
    {
        ecsyst::LogError( ERR03 );
        SDL_Quit( );

        delete this->eightchip_cpu;

        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------
int
EightChipApp::Run( )
{
    // Initialise the emulator (GFX, CPU, ...)
    if ( Initialise( ) == false )
        return -1;

    // Executes the emulation cycles (opcodes, etc.) into a main loop
    while ( statusRunning )
    {
        Update( );
    }

    // Clear
    Shutdown( );

    return 0;
}

//-------------------------------------------------------------------------------------------------
void
EightChipApp::Update( )
{
    ecemulate::EmulateCycle( this->eightchip_cpu, this->settings, this->statusRunning, this->main_window );
}

//-------------------------------------------------------------------------------------------------
void
EightChipApp::Shutdown( )
{
    delete this->eightchip_cpu;
    SDL_Quit( );
}

//-------------------------------------------------------------------------------------------------
int
main( int argc, char* argv[ ] )
{
    EightChipApp myEmulator;

    return myEmulator.Run( );
}

//-------------------------------------------------------------------------------------------------