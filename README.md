# GECK::Mapper - Fallout 2 map editor

[![Build](https://github.com/JanSimek/geck-map-editor/workflows/Build/badge.svg)](https://github.com/JanSimek/geck-map-editor/actions) [![Codacy Badge](https://app.codacy.com/project/badge/Grade/50b6611a3e2246c6b07282f87aa5940a)](https://www.codacy.com/gh/JanSimek/geck-map-editor/dashboard?utm_source=github.com&utm_medium=referral&utm_content=JanSimek/geck-map-editor&utm_campaign=Badge_Grade)

At the current stage this is nothing more than a map viewer but one day it will hopefully become a fully-featured map editor. Any help is appreciated.

![Screenshot](https://github.com/JanSimek/geck-map-editor/blob/master/screenshot.jpg "Screenshot")

## Building from source

This repository contains dependencies as git submodules. When you clone the repository make sure to use the `--recursive` flag or once cloned download submodules with `git submodule update --init --recursive`

Make sure to use the Release and not the Debug version if you are not a developer as the Release version is much much faster in loading maps.

### Linux

```bash
# Install dependencies (Ubuntu)
sudo apt install libsfml-dev

# Enter the cloned git repo folder
cd geck-map-editor
mkdir build && cd build
cmake ..
make
```

### Windows

The easiest way to build GECK on Windows is to use [vcpkg](https://vcpkg.io/) for dependency management and the latest Visual Studio for compilation.

```bash
vcpkg.exe integrate install
vcpkg.exe install --triplet x64-windows sfml
```

Now just open the project in Visual Studio (File > Open > CMake...) and then build it (Build > Build All).

### OS X

The easiest way to install SFML is using Homebrew.

```bash
brew install sfml
```

## Usage

Copy `master.dat` and `critter.dat` from your Fallout 2 installation to the `resources` subdirectory.

If you want to extract data files from these archives use [dat-unpacker](https://github.com/falltergeist/dat-unpacker) and convert all files to _lowercase_ using the `--transform` option. By default, GECK::Mapper will load files from the `resources` subdirectory.

Use arrow keys or right-click and drag to pan the view.

Left mouse click on an object to select it followed by a left click on en empty tile to place it there. Right mouse click or left mouse click on the same object unselects it.

Left mouse click on a tile to select it. Right mouse click unselects all tiles.

<kbd>Ctrl</kbd> + <kbd>R</kbd> to rotate selected object

## Compatibility

The goal is to be compatible with vanilla Fallout 2. That means loading maps made in the old Mapper

## Credits

I would have never been able to reach this stage without the code from [Falltergeist](https://github.com/falltergeist/falltergeist/), [Klamath](https://github.com/adamkewley/klamath), [FRM-Viewer](https://github.com/Primagen/Fallout-FRM-Viewer), [Dims Mapper](https://github.com/FakelsHub/F2_Mapper_Dims) and [darkfo](https://github.com/darkf/darkfo).

# Useful links

https://www.nma-fallout.com/threads/the-complete-fallout-1-2-artwork.191548/
