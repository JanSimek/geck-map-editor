# GECK::Mapper - Fallout 2 map editor

![](https://github.com/JanSimek/geck-map-editor/workflows/Build/badge.svg) [![Codacy Badge](https://app.codacy.com/project/badge/Grade/50b6611a3e2246c6b07282f87aa5940a)](https://www.codacy.com/gh/JanSimek/geck-map-editor/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=JanSimek/geck-map-editor&amp;utm_campaign=Badge_Grade)

At the current stage this is nothing more than a map viewer but one day it will hopefully become a fully-featured map editor. Any help is appreciated.

![Screenshot](https://github.com/JanSimek/geck-map-editor/blob/master/screenshot.jpg "Screenshot")

## Known issues

The application will hang for several seconds when loading a new map or changing map elevation. Loading screen should be implemented in the future.

## Usage

It's currently not possible to load assets directly from master.dat and critter.dat. You must extract these archives using [dat-unpacker](https://github.com/falltergeist/dat-unpacker), for example. By default GECK::Mapper will load files from the `resources` subdirectory.

Use arrow keys or right-click and drag to pan the view.

## Credits

I would have never been able to reach this stage without the code from [Falltergeist](https://github.com/falltergeist/falltergeist/), [Klamath](https://github.com/adamkewley/klamath), [FRM-Viewer](https://github.com/Primagen/Fallout-FRM-Viewer) and [darkfo](https://github.com/darkf/darkfo).
