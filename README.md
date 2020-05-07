# Regolith

## Introduction

### What it is

Regolith: From geology, refers to the layers of material above the bedrock but below the surface.
I chose this to represent a layer of abstraction and memory management architecture sitting on top of the Simple DirectMedia Layer bedrock.

It provide an API to support the development of 2D sprite-based games. The initial developement was focussed with goal of supporting a side-scrolling Sci-Fi RPG (in development), but with facilities to make it easily extensible.

The primary method for adding functionality is through factories. The user can define new classes, inheriting from existing ones, or the base interface classes, and define an associated factory build method and off you go.

My inspiration for platformer-style abilites was drawn from Hollow Knight. Although my artistic ability is not yet up to that level, my goal was to recreate the level of precision and overall tightness in gameplay that is only acheivable through a well design engine and interface.

### What it isn't

It does not provide any actual gameplay! In order to acheive a playable "Scene" (not just my integration test!) the user must generally specify certain derived classes to support gameplay.
For example, 2D platformer requires the "Player" model, including how it reacts to inputs, etc, to be fully defined with an associated factory method. Similar, unsupported Scene configurations and dialog menus must be supplied by the user.

## Installation

It is currently only available on Linux, my preferred development environment. Although a windows port is in development (come back later...).

It requires other libraries to be available: 
  - Logtastic (http://github.com/reachingisaskill/logtastic-logging-library)
  - Testass (http://github.com/reachingisaskill/testass-testing-and-assertion)

They must both be built somewhere and their setup files "source"ed.

Regolith relies on two thrid party libraries:
  - Jsoncpp - This is available from all good package managers and is regularly installed by other software anyway.
  - SDL2 - Again, available from all good package managers.

Then Regolith can be built with make:

```
$ cd regolith/
$ make
$ ./scripts/run_all_tests.bash
```

If any errors are reported at this stage, please register an issue on github with a copy of the output.

At this stage my developments do not support integration into the Linux system - I have not finialised the install/uninstall proceedure yet, so I am currently working in the temporary development environment described above.

And that's it!


## To Do List / Known Bugs

### Architecture Improvements
- I'd like to redesign the fundamental class structure using a single GameObject base class and a range of Interface classes to define functionality.
I hit a snag as a lot of the functionality has overlapping definitions/dependencies.
This will generally produce a lot of inheritence triangles if I'm not careful and may actually be overkill, but I'm thinking about it.

### Portability
- Move all SDL include statements into the Defitions file and wrap them with #if-#else statements to select for operating system

