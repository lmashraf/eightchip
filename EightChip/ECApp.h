#ifndef _EIGHTCHIP_APP_INCLUDED_
#define _EIGHTCHIP_APP_INCLUDED_

#include "stdafx.h"

#include "ECCpu.h"
#include "ECCpu_Defs.h"

class EightChipApp
{
	private:
		bool			statusRunning;

		EightChipCPU*	cpu;
		SETTINGS_MAP	settings;

	public:
		EightChipApp();
			int OnExecute(void);

	public:
		bool OnInit(void);

		void OnLoop(void);

		void OnCleanup(void);
};

#endif