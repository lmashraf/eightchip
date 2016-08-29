#include "stdafx.h"

#include "ECApp.h"

//-------------------------------------------------------------------------------------------------

EightChipApp::EightChipApp( )
{
    statusRunning = true;

    eightchip_cpu = EightChipCPU::GetInstance( );
}

//-------------------------------------------------------------------------------------------------

int
EightChipApp::OnExecute( )
{
    // Initialise the emulator (GFX, CPU, ...)
    if ( OnInit( ) == false )
        return -1;

    // Executes the emulation cycles (opcodes, etc.) into a main loop
    while ( statusRunning )
    {
        OnLoop( );
    }

    // Clear
    OnCleanup( );

    return 0;
}

//-------------------------------------------------------------------------------------------------

int
_tmain( int argc, _TCHAR* argv[ ] )
{
    EightChipApp myEmulator;

    return myEmulator.OnExecute( );
}

//-------------------------------------------------------------------------------------------------