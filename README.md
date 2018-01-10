# SparCraft-Evo

Use offline evolution to improve scripts in micro battles

SparCraft is open source StarCraft combat simulation package developed by [David Churchill](https://github.com/davechurchill/ualbertabot/tree/master/SparCraft). This project started from the fork of the SparCraft repo and was then separately developed in this repo for easier maintenance.  

## Installation Notes
- Fix `SDL.h not found` error: 
  - Download source of [SDL](https://www.libsdl.org/download-2.0.php) and [SDL_image](https://www.libsdl.org/projects/SDL_image/)
  - Add directories by right-click on project `Properties > Configuration Properties > VC++ Directories`. Update `Include Directories` by adding `include` folder from the sources. Do the same for `Libraries Directories` by adding `SparCraft/lib`  and `Source Directories` by adding `SparCraft/source`.
- Build as `Release` and `Win32`. 
- To run `bin/SparCraft.exe` from `Cmd`, need to include these in the same folder:
  - `SDL2.dll` and `SDL2_image.dll` (downloaded from the same site of the source codes)
  - `<experiment-config>.txt` file. Must change the paths in the experiment config file. 