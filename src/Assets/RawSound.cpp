
#include "Regolith/Assets/RawSound.h"


namespace Regolith
{

  Mix_Chunk* loadSound( std::string filename )
  {
    Mix_Chunk* new_chunk = Mix_LoadWAV( filename.c_str() );

    if ( new_chunk == nullptr )
    {
      std::string error = Mix_GetError();
      Exception ex( "loadSound()", "Failed to load sound file" );
      ex.addDetail( "Path", filename );
      ex.addDetail( "Mix Error", error );
      throw ex;
    }

    return new_chunk;
  }

}

