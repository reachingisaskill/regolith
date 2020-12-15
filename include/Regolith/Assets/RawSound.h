
#ifndef REGOLITH_ASSETS_RAW_SOUND_H_
#define REGOLITH_ASSETS_RAW_SOUND_H_

#include "Regolith/Global/Global.h"


namespace Regolith
{

  // Structure that owns the pointer to the Mix_Chunk object
  struct RawSound
  {
    Mix_Chunk* sound;
  };


  // Function that loads the sound file into the RawSound proxy
  RawSound loadRawSound( SoundDetail );

}

#endif // REGOLITH_ASSETS_RAW_SOUND_H_

