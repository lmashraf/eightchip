#ifndef _EIGHTCHIP_DEFS_INCLUDED_
#define _EIGHTCHIP_DEFS_INCLUDED_

#include <map>

//---------------------------------
// We need variables of sizes 8-bits / 16-bits (word)
// which are given by the following typedefs

typedef unsigned char		BYTE;	// 1 byte  (0 - 255)
typedef unsigned short int	WORD;	// 2 bytes (0 - 65535)

//---------------------------------
// Memory of 0xFFF bytes.
static const int ROMSIZE = 0xFFF;

//----------------------------------
// Settings map
typedef std::map<std::string, std::string> SETTINGS_MAP;

//----------------------------------
// Rom name shouldn't be hardcoded...
static const std::string ROM_NAME = "TicTacToe";

//----------------------------------
// Window properties
static const char* WINDOW_CAPTION	= "EightChip Emulator";
static const int   WINDOW_HEIGHT	= 640;
static const int   WINDOW_WIDTH		= 320;
//----------------------------------
// ERRORS
// Error codes are faster in runtime than try/catch clauses...
#define ERR000 "OpcodePerSecond paramaters could not be found in the settings file!"
#define ERR001 "The rom could not be found in the settings file!"
#define ERR002 "Error loading settings.ini file!"
#define ERR003 "Error loading the rom file: file does not exist!"
#define ERR004 "Error creating SDL window!"
#define ERR005 "Error initializing SDL!"
#define ERR006 "Error setting SDL's view mode!"
#define ERR007 "Error opening settings file game.ini !"
#define ERR008 "game.ini appears to be malformed!"
#define ERR009 "No settings could be found in game.ini"

#endif