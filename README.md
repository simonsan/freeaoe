freeaoe
=======

![screenshot](/doc/screenshot.png)

Freeaoe is a project to write a free engine capable of running Age of Empires 2
(and other Genie games).

The project is still in early development, but we can load basically all the
original data, and you can build, move around, load scenarios, show the
civilization history and stuff.

It (obviously) needs the original data files, but it should work with the data
files from the trial version (it's available e. g. from Archive.org).

It uses the same library as [AGE (Advanced Genie Editor)](https://github.com/Tapsa/AGE), so it should be able
to handle anything that can be edited by that (and therefore basically all
community content created for AoE2).

Windows
-------
Automated windows builds can be [downloaded from AppVeyor](https://ci.appveyor.com/project/sandsmark/freeaoe/build/artifacts).

In theory it should be able to automatically find the data files if AoE2 is installed, but I don't have windows so I haven't tested this.

Dependencies
------------
 - SFML
 - Modern C++ compiler

To build
--------
 - Clone with --recurse-submodules (e. g. `git clone --recurse-submodules https://github.com/sandsmark/freeaoe.git`)
 - `mkdir build && cd build`
 - `cmake .. && make`
 - `./freeaoe`

TODO (this is probably going to get outdated)
----
 - Map analysis stuff (for RMS and AI).
 - Unit grouping/formations.
 - Patrolling.
 - Performance (especially the path finding).
 - Various refactoring (move VisibilityMap out into separate file, rendering of units out from UnitManager, etc.)
