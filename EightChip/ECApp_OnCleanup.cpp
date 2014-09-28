#include "stdafx.h"

#include "ECApp.h"

void EightChipApp::OnCleanup(void)
{
	delete this->cpu;
	SDL_Quit();
}