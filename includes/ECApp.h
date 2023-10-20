#ifndef _EIGHTCHIP_APP_INCLUDED_
#define _EIGHTCHIP_APP_INCLUDED_

#include "ECCpu.h"
#include "ECGlobals.h"

//-------------------------------------------------------------------------------------------------

namespace ecgfx
{
    void InitOpenGL( SDL_Window* window );
    bool InitGraphics( SDL_Window* window );

    void DrawGraphics( EightChipCPU* cpu, SDL_Window* window );
};

//-------------------------------------------------------------------------------------------------

namespace ecemulate
{
    bool LoadSettings( SETTINGS_MAP& settings );

    bool LoadRom( EightChipCPU* cpu, const SETTINGS_MAP& settings );

    void SetupInput( EightChipCPU* cpu, SDL_Event event );

    void EmulateCycle( EightChipCPU* cpu, const SETTINGS_MAP& settings, bool& status, SDL_Window* window );
};

//-------------------------------------------------------------------------------------------------

namespace ecsyst
{
    void LogError( std::string message );
};

//-------------------------------------------------------------------------------------------------

class EightChipApp
{
public:
    EightChipApp( );

public:
    // Runs the main loop of the emulator
    int Run( );

    // Initialises the emulator's state
    bool Initialise( );

    // Updates the state of the emulator
    void Update( );

    // Cleans up
    void Shutdown( );

private:
    // Flag to keep track of whether the emulator is still running
    bool statusRunning;

    // Pointer to the EightChip CPU
    EightChipCPU* eightchip_cpu;

    // Map of settings for the emulator
    SETTINGS_MAP settings;

    // Pointer to the SDL window
    SDL_Window* main_window;
};

//-------------------------------------------------------------------------------------------------

#endif

//-------------------------------------------------------------------------------------------------