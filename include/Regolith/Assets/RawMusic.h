
#ifndef REGOLITH_ASSETS_RAW_MUSIC_H_
#define REGOLITH_ASSETS_RAW_MUSIC_H_

#include "Regolith/Global/Global.h"


namespace Regolith
{

  // Function to load the Mix_Music object from the music file
  Mix_Music* loadMusic( std::string );


  // Raw music structure. Owns the pointer to the Mix_Music object
  struct RawMusic
  {
    Mix_Music* music;

    RawMusic( Mix_Music* m = nullptr ) : music( m ) {}
  };

}

#endif // REGOLITH_ASSETS_RAW_MUSIC_H_

