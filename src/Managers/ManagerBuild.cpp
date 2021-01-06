
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
      loadJsonData( json_data, json_file );

      // Validate the required keys
      validateJson( json_data, "window", JsonType::OBJECT );
      validateJson( json_data, "input_device", JsonType::OBJECT );
      validateJson( json_data, "audio_device", JsonType::OBJECT );
      validateJson( json_data, "collision", JsonType::OBJECT );
//      validateJson( json_data, "font_data", JsonType::OBJECT );
      validateJson( json_data, "game_data", JsonType::OBJECT );
      validateJson( json_data, "contexts", JsonType::OBJECT );


      // Load the input device configuration first so objects can register game-wide behaviours
      this->_loadInput( json_data["input_device"] );
      // Engine gets to register its events first
      _theEngine.registerEvents( _theInput );
      _theHardware.registerEvents( _theInput );


      // Load the audio device configuration
      this->_loadAudio( json_data["audio_device"] );


      // Configure the window
      this->_loadWindow( json_data["window"] );


      //Load all the collision data
      this->_loadCollision( json_data["collision"] );


      // Load all the game objects files
      this->_loadData( json_data["game_data"] );


      // Currently Font Manager requires no global configuration
//      // Load all the font data
//      this->_loadFonts( json_data["font_data"] );


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

    this->_configureEvents();
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
    INFO_LOG( "Manager::_loadWindow : Loading window data" );

    // Let the window configure itself from the json data
    _theWindow.configure( json_data );

    // Register the regolith events with the input manager
    _theWindow.registerEvents( _theInput );
  }


  void Manager::_loadCollision( Json::Value& json_data )
  {
    INFO_LOG( "Manager::_loadCollision : Loading collision data" );

    _theCollision.configure( json_data );
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


  void Manager::_configureEvents()
  {
    // Load user events, etc
    Uint32 start_num = SDL_RegisterEvents( REGOLITH_EVENT_TOTAL );
    INFO_STREAM << "Manager::configureEvents : Registering " << REGOLITH_EVENT_TOTAL << " user events";

    if ( start_num == (unsigned int)-1 )
    {
      std::string error = SDL_GetError();
      FAILURE_STREAM << "Manager::configureEvents : Could not create required user events : " << error;
      Exception ex( "Manager::configureEvents()", "Could not create user events", true );
      ex.addDetail( "SDL Error", error );
      throw ex;
    }

    for ( unsigned int i = 0; i < (unsigned int)REGOLITH_EVENT_TOTAL; ++i )
    {
      SDL_memset( &_gameEvents[ i ], 0, sizeof(_gameEvents[ i ]) );
      _gameEvents[ i ].type = start_num; // This should be the same number as SDL_USEREVENT
      _gameEvents[ i ].user.code = i;
      _gameEvents[ i ].user.data1 = nullptr;
      _gameEvents[ i ].user.data2 = nullptr;
    }
  }

}

