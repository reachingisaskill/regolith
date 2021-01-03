
#include "Regolith/Assets/RawMusic.h"


namespace Regolith
{

  RawMusic loadRawMusic( AudioDetail details )
  {
    RawMusic raw_music;

    raw_music.music = Mix_LoadMUS( details.filename.c_str() );
    DEBUG_STREAM << "loadRawMusic : Loaded music data : " << details.filename << " @ " << raw_music.music;

    if ( raw_music.music == nullptr )
    {
      Exception ex( "loadRawMusic()", "Failed to load music file" );
      ex.addDetail( "Path", details.filename );
      ex.addDetail( "Mix Error", Mix_GetError() );
      throw ex;
    }

    return raw_music;
  }

}

