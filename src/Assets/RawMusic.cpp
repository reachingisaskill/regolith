
#include "Regolith/Assets/RawMusic.h"


namespace Regolith
{

  RawMusic loadRawMusic( MusicDetail details )
  {
    RawMusic raw_music;

    raw_music.music = Mix_LoadMUS( details.filename.c_str() );

    if ( raw_music.music == nullptr )
    {
      Exception ex( "loadRawMusic()", "Failed to load music file" );
      ex.addDetail( "Path", filename );
      ex.addDetail( "Mix Error", Mix_GetError() );
      throw ex;
    }

    return raw_music;
  }

}

