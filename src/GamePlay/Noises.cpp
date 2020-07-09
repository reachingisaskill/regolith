
#include "Regolith/GamePlay/Noises.h"
#include "Regolith/Utilities/JsonValidation.h"
#include "Regolith/Managers/RawObjectDetails.h"


namespace Regolith
{

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Construction functions

  Mix_Music* loadMusic( std::string filename )
  {
    Mix_Music* new_music = Mix_LoadMUS( filename.c_str() );

    if ( new_music == nullptr )
    {
      std::string error = Mix_GetError();
      Exception ex( "AudioManager::addMusic()", "Failed to load music file" );
      ex.addDetail( "Path", filename );
      ex.addDetail( "Mix Error", error );
      throw ex;
    }

    return new_music;
  }


  Mix_Chunk* loadSound( std::string filename )
  {
    Mix_Chunk* new_chunk = Mix_LoadWAV( filename.c_str() );

    if ( new_chunk == nullptr )
    {
      std::string error = Mix_GetError();
      Exception ex( "AudioHandler::addEffect()", "Failed to load effect file" );
      ex.addDetail( "Path", filename );
      ex.addDetail( "Mix Error", error );
      throw ex;
    }

    return new_chunk;
  }


//  RawMusic makeMusic( Json::Value& json_data )
//  {
//    Utilities::validateJson( json_data, "path", Utilities::JSON_TYPE_STRING );
//
//    std::string path = json_data["path"].asString();
//
//    Mix_Music* new_music = Mix_LoadMUS( path.c_str() );
//
//    if ( new_music == nullptr )
//    {
//      std::string error = Mix_GetError();
//      Exception ex( "AudioManager::addMusic()", "Failed to load music file" );
//      ex.addDetail( "Path", path );
//      ex.addDetail( "Mix Error", error );
//      throw ex;
//    }
//
//    return RawMusic( new_music );
//  }
//
//
//  RawSound makeSound( Json::Value& json_data )
//  {
//    Utilities::validateJson( json_data, "path", Utilities::JSON_TYPE_STRING );
//
//    std::string path = json_data["path"].asString();
//
//    Mix_Chunk* new_chunk = Mix_LoadWAV( path.c_str() );
//
//    if ( new_chunk == nullptr )
//    {
//      std::string error = Mix_GetError();
//      Exception ex( "AudioHandler::addEffect()", "Failed to load effect file" );
//      ex.addDetail( "Path", path );
//      ex.addDetail( "Mix Error", error );
//      throw ex;
//    }
//
//    return RawSound( new_chunk );
//  }

}

