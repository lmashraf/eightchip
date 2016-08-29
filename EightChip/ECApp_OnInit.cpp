#include "stdafx.h"

#include "ECApp.h"

bool EightChipApp::OnInit(void)
{
	if(!ecEmulate::LoadSettings(this->settings))
	{
		ecSys::LogMessage(ERR009);
		delete this->cpu;

		return false;
	}

	if(!ecGfx::SetupSDL())
	{
		ecSys::LogMessage(ERR004);
		delete this->cpu;

		return false;
	}

	if(!ecEmulate::LoadRom(this->cpu, this->settings))
	{
		ecSys::LogMessage(ERR003);
		SDL_Quit();

		delete this->cpu;

		return false;
	}

	return true;
}