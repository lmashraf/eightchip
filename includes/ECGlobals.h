#ifndef _EIGHTCHIP_GLOBALS_INCLUDED_
#define _EIGHTCHIP_GLOBALS_INCLUDED_

#include <map>

//-------------------------------------------------------------------------------------------------
// We need variables of sizes 8-bits / 16-bits (word) which are given by the following typedefs
using BYTE = unsigned char;       // 1 byte  (0 - 255)
using WORD = unsigned short int;  // 2 bytes (0 - 65535)

//-------------------------------------------------------------------------------------------------
// Memory of 0xFFF bytes.
static const int ROMSIZE = 0xFFF;

//-------------------------------------------------------------------------------------------------
// Settings map
using SETTINGS_MAP = std::map< std::string, std::string >;

//-------------------------------------------------------------------------------------------------
// Rom name shouldn't be hard coded...
static const std::string ROM_NAME = "RomFile";

//-------------------------------------------------------------------------------------------------
// Window properties
static const char* WINDOW_CAPTION = "EightChip Emulator";
static const int WINDOW_HEIGHT = 640;
static const int WINDOW_WIDTH = 1280;

//-------------------------------------------------------------------------------------------------
// Error codes are faster in runtime than try/catch clauses

#define ERR00 "Unable to locate OpcodePerSecond paramaters in settings file."
#define ERR01 "ROM not found in settings file."
#define ERR02 "Error loading settings file."
#define ERR03 "Error loading ROM file: file does not exist."
#define ERR04 "Error creating SDL window."
#define ERR05 "Error initialising SDL."
#define ERR06 "Error setting SDL's view mode."
#define ERR07 "Error opening settings file."
#define ERR08 "Malformed settings file."
#define ERR09 "No settings found in settings file."

//-------------------------------------------------------------------------------------------------

#endif

//-------------------------------------------------------------------------------------------------