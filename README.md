# SparCraft-Evo

The goal of this project is to use offline evolution to make more flexible scripts that are expected to perform better in micro battles compared to fixed scripts. 

The project uses the SparCraft framework, which is open source StarCraft combat simulation package developed by [David Churchill](https://github.com/davechurchill/ualbertabot/tree/master/SparCraft).

## Environment Setup Guide

1. Clone this repo

1. Install Microsoft Visual Studio 2013 Community Edition (either from Microsoft site or [here](https://drive.google.com/file/d/1dDvoAIyiA9RkJ0zL5SLEKaGHnDYafn81/view?usp=sharing))

1. Open the solution file `SparCraft-Evo/VisualStudio/SparCraft.sln` 

1. Include source and lib files: 
   * Download the sources (either `.zip` or `.tar.gz`) from [SDL](https://www.libsdl.org/download-2.0.php) and [SDL_image](https://www.libsdl.org/projects/SDL_image/) and extract them
   * In the *Solution Explorer* menu of Visual Studio, right-click on the projects and select `Properties > Configuration Properties > VC++ Directories`
   * In the *Include Directories* option, add the `include` folder from the extracted sources above
   * Repeat the same steps for *Libraries Directories* option by adding `SparCraft-Evo/lib`, and for *Source Directories* option by adding `SparCraft-Evo/source`

1. To build the project, choose the options to build as *Release* and *Win32* 

1. After the build is done, run `bin/SparCraft.exe` from command line (recommend Windows PowerShell). Notes: must include these files in the same folder:
   * `SDL2.dll` and `SDL2_image.dll` (downloaded from the same site of the sources above)
   * `<config-file-name>.txt` file. Notes: must update all the paths to be consistent with the test system