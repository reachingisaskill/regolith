
#include "Regolith/Assets/RawSound.h"


namespace Regolith
{

  RawSound loadRawSound( SoundDetail details )
  {
    RawSound raw_sound;

    raw_sound.sound = Mix_LoadWAV( details.filename.c_str() );

    if ( raw_sound.sound == nullptr )
    {
      Exception ex( "loadRawSound()", "Failed to load sound file" );
      ex.addDetail( "Path", filename );
      ex.addDetail( "Mix Error", Mix_GetError() );
      throw ex;
    }

    return raw_sound;
  }

}

