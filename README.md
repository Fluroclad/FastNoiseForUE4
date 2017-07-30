# FastNoiseForUE4

FastNoiseForUE4 is a modified version of [FastNoise](https://github.com/Auburns/FastNoise) by Jordan Peck. This project currently works for Unreal Engine 4 Version 4.16.2. Although this project works for both Blueprints and C++, your game project needs to be set to a C++ project.

# Usage
To use in your project replace the following:

* Place the files in a folder within you C++ Source tree.
```
../Source/<GameProjectName>/<FolderName>/FastNoiseLibrary.h
../Source/<GameProjectName>/<FolderName>/FastNoiseLibrary.cpp
```
* Rename the header file Game.h, located in FastNoiseLibrary.cpp, with your projects main header file.
```c++
#include "Game.h"
```
* Rename the macro GAME_API, located in FastNoiseLibrary.h, with your projects API macro.
```c++
UCLASS()
class GAME_API UFastNoiseLibrary : public UBlueprintFunctionLibrary
```
