# GECK::Mapper - Fallout 2 map editor

![](https://github.com/JanSimek/geck-map-editor/workflows/Build/badge.svg) [![Codacy Badge](https://app.codacy.com/project/badge/Grade/50b6611a3e2246c6b07282f87aa5940a)](https://www.codacy.com/gh/JanSimek/geck-map-editor/dashboard?utm_source=github.com&utm_medium=referral&utm_content=JanSimek/geck-map-editor&utm_campaign=Badge_Grade)

At the current stage this is nothing more than a map viewer but one day it will hopefully become a fully-featured map editor. Any help is appreciated.

![Screenshot](https://github.com/JanSimek/geck-map-editor/blob/master/screenshot.jpg "Screenshot")

## Building from source

```
# Install dependencies (Ubuntu)
sudo apt install libopenal-dev libogg-dev libflac-dev

# Enter the cloned git repo folder
cd geck-map-editor
mkdir build && cd build
cmake ..
make

```

## Usage

It's currently not possible to load assets directly from master.dat and critter.dat. You must extract these archives using [dat-unpacker](https://github.com/falltergeist/dat-unpacker), for example. By default GECK::Mapper will load files from the `resources` subdirectory.

Use arrow keys or right-click and drag to pan the view.

## Compatibility

The goal is to be compatible with vanilla Fallout 2. That means loading maps made in the old Mapper

## Credits

I would have never been able to reach this stage without the code from [Falltergeist](https://github.com/falltergeist/falltergeist/), [Klamath](https://github.com/adamkewley/klamath), [FRM-Viewer](https://github.com/Primagen/Fallout-FRM-Viewer) and [darkfo](https://github.com/darkf/darkfo).

# Useful links

https://www.nma-fallout.com/threads/the-complete-fallout-1-2-artwork.191548/
