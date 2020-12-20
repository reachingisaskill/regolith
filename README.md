# Regolith

## Introduction

### What it is

Regolith: From geology, refers to the layers of material above the bedrock but below the surface.
I chose this to represent a layer of abstraction and memory management architecture sitting on top of the Simple DirectMedia Layer bedrock.

It provides an API to support the development of 2D sprite-based games. The initial developement was focussed with goal of supporting a side-scrolling Sci-Fi RPG (in development), but with facilities to make it easily extensible.

It defines a multithreaded engine, handles asset and memory management and provides handlers for easy sound and input event functionality. Hopefully taking the most complicated stuff away from the actual game design.

There are two core class families for users to inherit from: Physical Objects and Contexts. See below for more details.

### What it isn't

It does not provide any actual gameplay! In order to acheive a playable level (not just my integration or physics tests) the user must specify all the details to enable gameplay.

## Installation

It is currently only available on Linux, my preferred development environment. Although a windows port is viable using MSYS2 and a MinGW build environment.
For best results users should clone the lastest version from the release branch. This is stable and reliable.

It requires other libraries to be available: 
  - Logtastic (https://github.com/reachingisaskill/logtastic-logging-library)
  - Testass (https://github.com/reachingisaskill/testass-testing-and-assertion)

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

If any errors are reported at this stage, please register an issue on github with a copy of the output and any log files produced (test_data/logs/).

At this stage my developments do not support integration into the Linux system - I have not finialised the install/uninstall proceedure yet, so I am currently working in the temporary development environment described above.

And that's it!


## Usage Overview

### Physical Objects

Anything that has a physical presence on the game. Characters, NPCs, backgrounds, loadzones, boss arenas, etc. Anything that could be interacted with or interact with the player. Functionanlity maybe provided by inheriting from one or more object interfaces.

The interfaces provide flags for the engine to apply required operations only to objects that require them. E.g. The background image requires no collision, only a texture, therefore simple non-interactable images should inherit from DrawableObject only. Load zones and area triggers only have a collision element, therefore they should inherit from CollidableObject only. The main character will likely inherit from all the object interfaces - this is as designed. They enable the following functionalities:

- DrawableObject : Interface must provide a texture for the engine to render to the window,
- CollidableObject : Interface must provide a collision for the engine to include in collision checking routines,
- NoisyObject : Interface configures and enables the object to trigger sound effects,
- AnimatedObject : Interface must provide an update function for frame-by-frame updates to be performed. 
- ButtonObject : Interface must provide functions to handle the giving and taking of focus typically for menu contexts,
- ControllableObject : Interface must register expected input actions and handle their occurance. Enables an event-based input handling system.
- CompositeObject : Interface must provide a vector of children objects. This interface is not yet supported.

Very simple examples of game objects my be found in the Test directory.


### Contexts
These are the core layer of the interface design. They each have unique Audio and Input handlers. i.e. A context is an environment with a unique and overriding input handler. A standard game level would be one context (using several layers), the pause menu would be a second stacked on top of the first. It overrides the character input controls so that you aren't controlling both the menu and the gameplay simultaneously. The underlying contexts may be paused, or continue using flags configured by the derived context classes. All context stack interactions have callback functions such that a context knows what state it is currently in. The game engine stops when all the contexts have flagged themselves for closure and have been popped from the context stack - the context stack is never empty otherwise.

Derived contexts are expected to provide various functionality. E.g. each frame an update function is called such that time/frame based events may be updated in this function. In addition Input Events may be registered allowing the context iteself to respond to user input. The design philosophy is to have each type of context, e.g. single level/area, pause menu, start menu, etc to be defined as a unique class to support the different functionalities required.

Note that contexts do no own any of their own asset data.


### Context Groups

This is the macro structure for handling data within the game. Currently games must be divided into groups of contexts and objects (note that different context groups can share the same object and context definition, e.g. a shared pause menu design), this forms a single ContextGroup. Each context group currently is loaded individually and bridged using a loading screen. There is additionally a global context group to hold objects and contexts that must be available at all times e.g. the main player character or all the load screens.

The context stack is designed such that it can be emptied an refilled in a single frame. i.e. the expected use is that ContextGroup corresponds to an area of the game, with multiple "level" contexts or "rooms" that the player moves between using load triggers. At the end of this structure a context group change is triggered to represent moving to a different game area and a load screen is used while the contrext groups in memory are exchanged in the background.

All the loading/unloading is handled by the engine. The only requirement is that the context used as a load screen checks for the completion of the ContextManager::isLoaded() and subsequently calls Manager::openEntryPoint() before closing itself. I have chosen to relinquish this control to the derived class such that more complex load screens may be implmented expecting user interaction etc.

Objects and Contexts within each context group access game assets through the context group's DataHandler. The data handler communicates with the global DataManager to find and load the raw asset data into memory in such a way that it is shared. Therefore, there only ever exists a single copy of each asset within a context group. Users should not worry about minimising level size to reduce ram usage because of this. Although I'm sure someone will find a way to cause problems eventually...


## Remarks

My inspiration for the platformer-style applications was drawn from Hollow Knight. Although my artistic ability is not yet up to that level, my goal was to recreate the level of precision and overall tightness in gameplay that is only acheivable through a well design engine and interface. It is my intention that what I have constructed will enable that.

I personally have an interest in real time strategy games particularly as a (somewhay mediocre unfortunately) Starcraft and Starcraft II player. I have tried to ensure that the underlying engine and architecture could be applied to both these vastly different platformer and real-time strategy concepts. Granted the game will change drastically, but I would hope for minimal changes to the engine in this case.


## To Do List / Known Bugs

### To Do list

- TESTING! Most of it seems to work with no obvious memory leaks or crashes, but it definitely needs some more serious stress testing at this stage.
- I would like to enable ContextGroup loading/unloading without using a load screen. Games would still be split into "rooms" but at least neighbouring areas could be loaded/unloaded on the fly, reducing the actual usage of loadscreens.
- A map object. Contexts must currently self organise with each other as to which one loads next. Having a global map would allow the engine to figure out which one to load next and help facilitate the previous request ^.
- Overhaul of threading. Although no use yet requires it, I am planning how to further improve the multithreaded the engine, particularly if AI-heavy objects are required. This is hopefully way off in the future as it may be quite a drastic change that will required extensive planning and testing.

### Known Bugs

None! I'm amazing!
But probably because we just haven't found them yet...

