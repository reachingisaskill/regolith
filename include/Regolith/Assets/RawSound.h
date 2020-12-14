
#ifndef REGOLITH_ASSETS_RAW_SOUND_H_
#define REGOLITH_ASSETS_RAW_SOUND_H_

#include "Regolith/Global/Global.h"


namespace Regolith
{

  // Function that loads the sound file into the Mix_Chunk object
  Mix_Chunk* loadSound( std::string );


  // Structure that owns the pointer to the Mix_Chunk object
  struct RawSound
  {
    Mix_Chunk* sound;
    int channel;

    RawSound( Mix_Chunk* s = nullptr, int c = 0 ) : sound( s ), channel( c ) {}
  };

}

#endif // REGOLITH_ASSETS_RAW_SOUND_H_

