# Regolith

## TL;DR

Regolith: From geology, refers to the layers of material above the bedrock but below the surface.

Provides an API wrapping SDL to compose 2D sprite driven games. Performs all the memory management and event handling.
The user can extend the framework to include their own classes that do most things and Regolith will clean up after them.


# To Do List

## Architecture Improvements
- Implement a (doubly) linked list class to allow fast deletion and linear iteration for things like projectiles with timed lives.
- Implement a "story" class. A state machine that organises the order in which scenes are loaded.

## Portability
- Move all SDL include statements into the Defitions file and wrap them with #if-#else statements to select for operating system

