Eight Chip Emulator
=========

EightChip is a chip8 emulator written in C++ using OpenGL and SDL for graphics. It is based on Laurence Muller and Codeslinger's tutorials and the Wikipedia's documentation of the Chip8.

Used keys: <br>
azer, qsdf, wxcv, uiop

To run a ROM, edit "settings.ini" with the following format:<br>

RomName:ROMS\ROMFILE!<br>
OpcodesPerSecondes:NNNN!<br>

where RomName should be given to the global variable ROM_NAME in "ECCPU_Defs.h" for execution.


A lot of tweaking to make this easier will be done shortly. 
Stay tuned, and have fun!
