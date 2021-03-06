
#ifndef REGOLITH_ASSETS_RAW_MUSIC_H_
#define REGOLITH_ASSETS_RAW_MUSIC_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Assets/RawObjectDetails.h"


namespace Regolith
{

  // Raw music structure. Owns the pointer to the Mix_Music object
  struct RawMusic
  {
    Mix_Music* music;
  };


  // Function to load the Mix_Music object from the music file
  RawMusic loadRawMusic( AudioDetail );

}

#endif // REGOLITH_ASSETS_RAW_MUSIC_H_

