#include "stdafx.h"

#include "ECApp.h"

//-------------------------------------------------------------------------------------------------

void
EightChipApp::OnLoop( )
{
    ecemulate::EmulateCycle( this->eightchip_cpu, this->settings, this->statusRunning );
}

//-------------------------------------------------------------------------------------------------