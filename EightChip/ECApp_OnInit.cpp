#include "stdafx.h"

#include "ECApp.h"

//-------------------------------------------------------------------------------------------------

bool
EightChipApp::OnInit( )
{
    if ( !ecemulate::LoadSettings( this->settings ) )
    {
        ecsyst::LogMessage( ERR009 );
        delete this->eightchip_cpu;

        return false;
    }

    if ( !ecgfx::SetupSDL( ) )
    {
        ecsyst::LogMessage( ERR004 );
        delete this->eightchip_cpu;

        return false;
    }

    if ( !ecemulate::LoadRom( this->eightchip_cpu, this->settings ) )
    {
        ecsyst::LogMessage( ERR003 );
        SDL_Quit( );

        delete this->eightchip_cpu;

        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------