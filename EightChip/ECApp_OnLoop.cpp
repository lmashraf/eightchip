#include "stdafx.h"

#include "ECApp.h"

void EightChipApp::OnLoop(void)
{
	ecEmulate::EmulateCycle(this->cpu, this->settings, this->statusRunning);
}