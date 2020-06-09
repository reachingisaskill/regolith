
#include "Regolith/GamePlay/Noises.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Construction functions

  RawMusic makeMusic( Json::Value& json_data )
  {
    Utilities::validateJson( json_data, "name", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "path", Utilities::JSON_TYPE_STRING );

    std::string name = json_data["name"].asString();
    std::string path = json_data["path"].asString();

    Mix_Music* new_music = Mix_LoadMUS( path.c_str() );

    if ( new_music == nullptr )
    {
      std::string error = Mix_GetError();
      Exception ex( "AudioManager::addMusic()", "Failed to load music file" );
      ex.addDetail( "Name", name );
      ex.addDetail( "Path", path );
      ex.addDetail( "Mix Error", error );
      throw ex;
    }

    return RawMusic( new_music );
  }


  RawSound makeSound( Json::Value& json_data )
  {
    Utilities::validateJson( json_data, "name", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "path", Utilities::JSON_TYPE_STRING );

    std::string name = json_data["name"].asString();
    std::string path = json_data["path"].asString();

    Mix_Chunk* new_chunk = Mix_LoadWAV( path.c_str() );

    if ( new_chunk == nullptr )
    {
      std::string error = Mix_GetError();
      Exception ex( "AudioHandler::addEffect()", "Failed to load effect file" );
      ex.addDetail( "Name", name );
      ex.addDetail( "Path", path );
      ex.addDetail( "Mix Error", error );
      throw ex;
    }

    return RawSound( new_chunk );
  }

}

