# f4silver
A plugin for Fallout 4 that allows to activate console commands through key presses.

### Implemented
* Batch file execution by key press
* Batch file description
* In-game messages (console, pop-up in top-left corner)
* Interior\Exterior and InteriorOnly\ExteriorOnly execution flow
* Random batch execution
* Turning commands on\off
* Configurable in-game messages
* Support for theoretically any game version
* Teleport command
* Timeout command
* Player death detection
* VATS detection
* Game pause detection
* Dialogue detection
* something else?

### Yet to be implemented
* Save\Load game commands
* Pre\Post batch execution save
* Support for steam version

### Compilation
Compilation is done as a single unit, just assemble hooks.asm, compile main.cpp, link them and it should work.
To compile hook library you will need DirectX SDK installed.
For more details see tools/build.bat file.

### Installation
* Extract contents of a zip file (see release page) into root Fallout 4 folder (the one with Fallout4.exe).

Note that that this plugin will not currently load into steam version (use external dll injector).

### Supported versions
Should work on most game versions (create an issue if it doesn't and specify problematic version).
List of versions that were tested is located below.

### Tested versions
* 1.10.111
* 1.10.106
* 1.10.98
* 1.10.82
* 1.10.75
* 1.10.64
* 1.10.50
* 1.10.40
* 1.10.26

### How it works
There are two dll files:
* **X3DAudio1_7** - hook library;
* **f4silver** - main library.

A hook library is loaded by game itself since it uses its functions (that's why this library is called specifically like that).
In order to successefully run and not crash this library loads the real one and redirects functions used by game to it. Basically, it's a proxy library. The goal of this is to load our own library into game's process memory space so we could modify game's executable that is loaded into memory and call functions from it.

The main library listens for keyboard input in dedicated thread (that way we can register whenever key was pressed or not independently from the game) and puts "batch" files in a queue at key presses. Queue execution happens in the game's main loop, to do so library hooks game's main loop so that its own main loop function is called by the game each frame.
Queue implementation is rather simple it just stores pointers to the data it suppossed to store, so one have to make sure that these pointers are valid at all time queue is used.
"Batch" file execution is implemented line-by-line, one after another and so on until the end of file.
Library performs execution of each line through game's scripting capabilities, it just calls to game to create and execute the script. It also calls to game to determine if player is currently in interior or exterior so that "batch" files marked as "exterior\interior only" would only execute when they actually should.
