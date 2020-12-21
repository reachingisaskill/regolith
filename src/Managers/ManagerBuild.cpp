
#include "Regolith/Managers/Manager.h"
#include "Regolith/Components/Engine.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  void Manager::init( std::string json_file )
  {
    INFO_STREAM << "Manager::init : Initialising the manager using file : " << json_file;
    // Initialise the SDL subsystems
    if ( SDL_Init( SDL_INIT_EVERYTHING ) < 0 )
    {
      Exception ex( "Manager::init()", "Failed to initialise SDL" );
      ex.addDetail( "SDL Error", SDL_GetError() );
      throw ex;
    }

    if ( TTF_Init() == -1 )
    {
      Exception ex( "Manager::init()", "Failed to initialise TTF" );
      ex.addDetail( "TTF Error", TTF_GetError() );
      throw ex;
    }

    int imgFlags = IMG_INIT_PNG;
    if ( ! ( IMG_Init( imgFlags ) & IMG_INIT_PNG ) )
    {
      Exception ex( "Manager::init()", "Failed to initialise IMG" );
      ex.addDetail( "IMG Error", IMG_GetError() );
      throw ex;
    }

    int mixFlags = MIX_INIT_OGG;
    if ( ! ( Mix_Init( mixFlags ) & mixFlags ) )
    {
      Exception ex( "Manager::init()", "Failed to initialise Mixer" );
      ex.addDetail( "MIX Error", Mix_GetError() );
      throw ex;
    }


    try
    {
      // Load and parse the json config
      Json::Value json_data;
      Utilities::loadJsonData( json_data, json_file );

      // Validate the required keys
      Utilities::validateJson( json_data, "window", Utilities::JSON_TYPE_OBJECT );
      Utilities::validateJson( json_data, "input_device", Utilities::JSON_TYPE_OBJECT );
      Utilities::validateJson( json_data, "audio_device", Utilities::JSON_TYPE_OBJECT );
      Utilities::validateJson( json_data, "collision_teams", Utilities::JSON_TYPE_OBJECT );
      Utilities::validateJson( json_data, "font_data", Utilities::JSON_TYPE_OBJECT );
      Utilities::validateJson( json_data, "game_data", Utilities::JSON_TYPE_OBJECT );
      Utilities::validateJson( json_data, "contexts", Utilities::JSON_TYPE_OBJECT );


      // Load the input device configuration first so objects can register game-wide behaviours
      this->_loadInput( json_data["input_device"] );
      // Engine gets to register its events first
      _theEngine.registerEvents( _theInput );
      _theHardware.registerEvents( _theInput );


      // Load the audio device configuration
      this->_loadAudio( json_data["audio_device"] );


      // Configure the window
      this->_loadWindow( json_data["window"] );


      //Load all the collision teams
      this->_loadTeams( json_data["collision_teams"] );


      //Load all the collision types
      this->_loadTypes( json_data["collision_types"] );


      // Load all the game objects files
      this->_loadData( json_data["game_data"] );


      // Load all the game objects files
      this->_loadFonts( json_data["font_data"] );


      // Load all the contexts
      this->_loadContexts( json_data["contexts"] );

    }
    catch ( std::ios_base::failure& f ) // Thrown by ifstream
    {
      Exception ex( "Manager::init()", "IO-Stream Failure", false );
      ex.addDetail( "What", f.what() );
      throw ex;
    }
    catch ( std::runtime_error& rt ) // Thrown by jsoncpp
    {
      Exception ex( "Manager::init()", "Json parsing error", false );
      ex.addDetail( "What", rt.what() );
      throw ex;
    }

    this->configureEvents();
  }


  void Manager::_loadInput( Json::Value& json_data )
  {
    try
    {
      INFO_LOG( "Manager::_loadInput : Configuring the input manager" );
      _theInput.configure( json_data );
    }
    catch ( std::runtime_error& rt )
    {
      Exception ex( "Manager::_loadInput()", "Json reading failure" );
      ex.addDetail( "What", rt.what() );
      throw ex;
    }
  }


  void Manager::_loadAudio( Json::Value& json_data )
  {
    try
    {
      INFO_LOG( "Manager::_loadAudio : Configuring the audio manager" );
      _theAudio.configure( json_data );
    }
    catch ( std::runtime_error& rt )
    {
      Exception ex( "Manager::_loadAudio()", "Json reading failure" );
      ex.addDetail( "What", rt.what() );
      throw ex;
    }
  }


  void Manager::_loadWindow( Json::Value& json_data )
  {
    // Let the window configure itself from the json data
    _theWindow.configure( json_data );

    // Register the regolith events with the input manager
    _theWindow.registerEvents( _theInput );

    // Set the default colour
    Utilities::validateJson( json_data, "default_colour", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJsonArray( json_data["default_colour"], 4, Utilities::JSON_TYPE_INTEGER );

    Json::Value color = json_data["default_color"];
    _defaultColor.r = color[0].asInt();
    _defaultColor.g = color[1].asInt();
    _defaultColor.b = color[2].asInt();
    _defaultColor.a = color[3].asInt();
  }


  void Manager::_loadTeams( Json::Value& json_data )
  {
    INFO_LOG( "Manager::_loadTeams : Loading collision team map" );
    for ( Json::Value::const_iterator it = json_data.begin(); it != json_data.end(); ++it )
    {
      std::string team_name = it.key().asString();
      CollisionTeam id = (CollisionTeam) it->asInt();
      addCollisionTeam( team_name, id );
    }
  }


  void Manager::_loadTypes( Json::Value& json_data )
  {
    INFO_LOG( "Manager::_loadTeams : Loading collision type map" );
    for ( Json::Value::const_iterator it = json_data.begin(); it != json_data.end(); ++it )
    {
      std::string team_name = it.key().asString();
      CollisionType id = (CollisionType) it->asInt();
      addCollisionType( team_name, id );
    }
  }


  void Manager::_loadData( Json::Value& game_data )
  {
    INFO_LOG( "Manager::_loadData : Loading game data" );

    _theData.configure( game_data );
  }


  void Manager::_loadFonts( Json::Value& font_data )
  {
    INFO_LOG( "Manager::_loadFonts : Loading font data" );

    _theFonts.configure( font_data );
  }


  void Manager::_loadContexts( Json::Value& context_data )
  {
    INFO_LOG( "Manager::_loadContexts : Loading contexts" );

    _theContexts.configure( context_data );
  }

}

