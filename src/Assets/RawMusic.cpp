
#include "Regolith/Assets/RawMusic.h"


namespace Regolith
{

  Mix_Music* loadMusic( std::string filename )
  {
    Mix_Music* new_music = Mix_LoadMUS( filename.c_str() );

    if ( new_music == nullptr )
    {
      std::string error = Mix_GetError();
      Exception ex( "loadMusic()", "Failed to load music file" );
      ex.addDetail( "Path", filename );
      ex.addDetail( "Mix Error", error );
      throw ex;
    }

    return new_music;
  }

}

