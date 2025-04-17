# Control Surface Integrator

The Control Surface Integrator (CSI) is a Reaper plugin that aims to let you integrate your hardware control surfaces into Reaper.

## Information about the project

The [wiki](https://github.com/FunkybotsEvilTwin/CSIUserGuide/wiki) is the starting point for all the documentation about CSI.

## How to build

### Windows

#### Microsoft Visual Studio 2022

- Open the project directory in the IDE, so that it loads the CMakePresets and the CMakeLists
- You can choose between the Debug and Release configurations (and Win32 and x64 architectures) for the build
- Build the project

The binary will be generated in the `cmake-build-$(archi)\$(Configuration}` directory, where

- `$(archi)` is the architecture (win32 or win64)
- `$(Configuration)` is the build configuration (Debug or Release).

The "Install" command will copy the plugin to the Reaper plugin directory, using the `REAPER_PATH` environment variable.

#### Jetbrains Clion

- Open the project directory in the IDE, so that it loads the CMakePresets and the CMakeLists
- In the project wizard that pops-up, enable the two profiles corresponding to the Win64Debug and Win64Release presets (disable any other).
  For a 32-bit build, enable the Win32Debug and Win32Release presets instead (you may also enable all of them simultaneously).
  If the wizard does not show up or if you closed it, go to "Settings/Build, Execution, Deployment/CMake" to tweak this.
- Build the project

The binary will be generated in the `cmake-build-$(archi)\$(Configuration}` directory, where

- `$(archi)` is the architecture (win32 or win64)
- `$(Configuration)` is the build configuration (Debug or Release).

The "Install" command will copy the plugin to the Reaper plugin directory, using the `REAPER_PATH` environment variable.

### Linux

From the root directory of the project, run CMake to generate the build files, using the preset for Linux.
Then enter the build directory and run make:

```bash
cmake --preset=linux64
cmake --build --preset=linux64Release
```

### MacOS

From the root directory of the project, run CMake to generate the build files, using the preset for Mac.
Then enter the build directory and run make:

```bash
cmake --preset=macos-x86_64
cmake --build --preset=macos-x86_64Release
```
