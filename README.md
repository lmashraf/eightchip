Eight Chip Emulator
=========

EightChip is a chip8 emulator written in C++ using OpenGL and SDL for graphics. It is based on Laurence Muller and Codeslinger's tutorials and the Wikipedia's documentation of the Chip8.

Used keys: <br>
azer, qsdf, wxcv, uiop

To run a ROM, edit "settings.ini" with the following format:<br>

RomFile:ROMS\ROMFILE!<br>
OpcodesPerSecondes:NNNN!<br>

where RomFile should take the path to the ROM to be executed. The second argument is the number of instructions you'd wish to execute per second. It has a nice effect to it the lower it goes.


A lot of tweaking to make this easier will be done shortly. 
Stay tuned, and have fun!
