#ifndef _EIGHTCHIP_APP_INCLUDED_
#define _EIGHTCHIP_APP_INCLUDED_

#include "stdafx.h"

#include "ECCpu.h"
#include "ECCpu_Defs.h"

//-------------------------------------------------------------------------------------------------

class EightChipApp
{
public:
    EightChipApp( );

public:
    int OnExecute( );
    bool OnInit( );
    void OnLoop( );
    void OnCleanup( );

private:
    bool statusRunning;

    EightChipCPU* eightchip_cpu;

    SETTINGS_MAP settings;
};

//-------------------------------------------------------------------------------------------------

#endif

//-------------------------------------------------------------------------------------------------