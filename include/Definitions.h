
#ifndef __REGOLITH__DEFINITIONS_H__
#define __REGOLITH__DEFINITIONS_H__


#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>


#define REGOLITH_VERSION_NUMBER "0.1"


namespace Regolith
{

  const float pi = 3.14159265358979323846;

  const float e = 2.7182818284590452353602874;

  const float epsilon = 1.0E-6;


  enum ObjectProperty
  {
    OBJECT_SIMPLE = 0,
    OBJECT_ANIMATED = 1 << 1,
    OBJECT_HAS_COLLISION = 1 << 2,
    OBJECT_HAS_INPUT = 1 << 3
  };


  enum GameEvents
  {
    REGOLITH_TOTAL_SCENE_END,
    REGOLITH_EVENT_TOTAL
  };


}

#endif // __REGOLITH__DEFINITIONS_H__

