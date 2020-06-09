
#ifndef REGOLITH_GAME_PLAY_NOISES_H_
#define REGOLITH_GAME_PLAY_NOISES_H_

#include "Regolith/Global/Global.h"


namespace Regolith
{

  RawMusic makeMusic( Json::Value& );
  RawSound makeSound( Json::Value& );

  struct RawMusic
  {
    Mix_Music* music;

    RawMusic( Mix_Music* m = nullptr ) : music( m ) {}
  };


  struct RawSound
  {
    Mix_Chunk* sound;
    int channel;

    RawSound( Mix_Chunk* s = nullptr, int c = 0 ) : sound( s ), channel( c ) {}
  };


}

#endif // REGOLITH_GAME_PLAY_NOISES_H_

