// ECApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "ECApp.h"

EightChipApp::EightChipApp(void)
{
	statusRunning = true;
	
	// Chip8 CPU
	cpu = EightChipCPU::GetInstance();	
}

//--------------------------------------------------
int EightChipApp::OnExecute(void)
{
	// Initializes the emulator (gfx, cpu, etc.)
	if(OnInit() == false)
		return -1;
	
	// Emulator's main loop, executes the emulation cycles (opcodes, etc.)
	while(statusRunning)
	{
		OnLoop();
	}

	// Necessary clean up.
	OnCleanup();

	return 0;
}
//--------------------------------------------------

int _tmain(int argc, _TCHAR* argv[])
{
	EightChipApp myEmulator;

	return myEmulator.OnExecute();
}

