
#include "Regolith/Managers/Manager.h"
#include "Regolith/Components/Engine.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  void Manager::init( std::string json_file )
  {
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
      Utilities::validateJson( json_data, "fonts", Utilities::JSON_TYPE_OBJECT );
      Utilities::validateJson( json_data, "input_device", Utilities::JSON_TYPE_OBJECT );
      Utilities::validateJson( json_data, "audio_device", Utilities::JSON_TYPE_OBJECT );
      Utilities::validateJson( json_data, "collision_teams", Utilities::JSON_TYPE_OBJECT );
      Utilities::validateJson( json_data, "game_data", Utilities::JSON_TYPE_OBJECT );
      Utilities::validateJson( json_data, "contexts", Utilities::JSON_TYPE_OBJECT );


      // Load the input device configuration first so objects can register game-wide behaviours
      this->_loadInput( json_data["input_device"] );
      // Engine gets to register its events first
      _theEngine.registerEvents( _theInput );
      _theHardware.registerEvents( _theInput );


      // Load the audio device configuration
      this->_loadAudio( json_data["audio_device"] );


      // Load the Font data
      this->_loadFonts( json_data["fonts"] );


      // Configure the window
      this->_loadWindow( json_data["window"] );
      _theWindow.registerEvents( _theInput );
      _theEngine.setRenderer( _theRenderer );


      //Load all the collision teams
      this->_loadTeams( json_data["collision_teams"] );


      // Load all the game objects files
      this->_loadData( json_data["game_data"] );


      // Load all the contexts
      this->_loadContexts( json_data["contexts"] );

    }
    catch ( std::ios_base::failure& f ) // Thrown by ifstream
    {
      Exception ex( "Manager::init()", "Default font not found", false );
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

    // Last configuration operation - validating the mass produced objects
    // Validate all the game objects
    _theData.validate();

    // Validate all the contexts
    _theContexts.validate();
  }


  void Manager::_loadInput( Json::Value& json_data )
  {
    try
    {
      INFO_LOG( "Configuring the input manager" );
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
      INFO_LOG( "Configuring the audio manager" );
      _theAudio.configure( json_data );
    }
    catch ( std::runtime_error& rt )
    {
      Exception ex( "Manager::_loadAudio()", "Json reading failure" );
      ex.addDetail( "What", rt.what() );
      throw ex;
    }
  }


  void Manager::_loadFonts( Json::Value& json_data )
  {
    Utilities::validateJson( json_data, "default_font", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "font_list", Utilities::JSON_TYPE_ARRAY );


    // Find out what the default font is called
    std::string default_font = json_data["default_font"].asString();

    // Load the listed fonts
    Json::Value font_data = json_data["font_list"];
    Json::ArrayIndex fonts_size = font_data.size();
    INFO_STREAM << "Loading " << fonts_size << " fonts";
    for ( Json::ArrayIndex i = 0; i < fonts_size; ++i )
    {
      Utilities::validateJson( font_data[i], "name", Utilities::JSON_TYPE_STRING );
      Utilities::validateJson( font_data[i], "path", Utilities::JSON_TYPE_STRING );
      Utilities::validateJson( font_data[i], "size", Utilities::JSON_TYPE_INTEGER );

      // Load the font details
      std::string font_name = font_data[i]["name"].asString();
      std::string font_path = font_data[i]["path"].asString();
      int font_size = font_data[i]["size"].asInt();


      INFO_STREAM << "Opening TTF file from: " << font_path;
      // Try to open the TTF File
      TTF_Font* theFont = TTF_OpenFont( font_path.c_str(), font_size );
      if ( theFont == nullptr ) // Failed to open
      {
        if ( font_name == default_font ) // Can't continue without the default font
        {
          FAILURE_LOG( "Manager::_loadFonts() : Failed to load the default font" );
          Exception ex( "Manager::_loadFonts()", "Can not load default font", false );
          ex.addDetail( "TTF Error", TTF_GetError() );
          throw ex;
        }
        // Ignore the other fonts - use the default instead
        ERROR_STREAM << "Could not load font: " << font_name;
      }
      _fonts[ font_name ] = theFont;
      INFO_STREAM << "Added font: " << font_name;
    }

    if ( _fonts.find( default_font ) == _fonts.end() ) // Default font must be loaded correctly
    {
      FAILURE_LOG( "Manager::_loadFonts() : The default font is not loaded by the config file" );
      Exception ex( "Manager::_loadFonts()", "Default font not found", false );
      ex.addDetail( "Font name", default_font );
      throw ex;
    }

    // Set the default font pointer
    _defaultFont = _fonts[ default_font ];
    INFO_STREAM << "Default font, " << default_font << " identified " << _defaultFont;
  }


  void Manager::_loadWindow( Json::Value& json_data )
  {
    Utilities::validateJson( json_data, "screen_width", Utilities::JSON_TYPE_INTEGER );
    Utilities::validateJson( json_data, "screen_height", Utilities::JSON_TYPE_INTEGER );
    Utilities::validateJson( json_data, "default_colour", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJson( json_data, "title", Utilities::JSON_TYPE_STRING );

    Utilities::validateJsonArray( json_data["default_colour"], 4, Utilities::JSON_TYPE_INTEGER );

    // Load the window configuration before we start loading texture data
    int screen_width = json_data["screen_width"].asInt();
    int screen_height = json_data["screen_height"].asInt();
    std::string title = json_data["title"].asString();

    // Initialise the window
    _theWindow.init( title, screen_width, screen_height );
    _theWindow.registerEvents( _theInput );

    // Set the default colour
    Json::Value color = json_data["default_color"];
    _defaultColor.r = color[0].asInt();
    _defaultColor.g = color[1].asInt();
    _defaultColor.b = color[2].asInt();
    _defaultColor.a = color[3].asInt();

  }


  void Manager::_loadTeams( Json::Value& json_data )
  {
    Json::Value::const_iterator data_end = json_data.end();
    for ( Json::Value::const_iterator it = json_data.begin(); it != data_end; ++it )
    {
      std::string team_name = it.key().asString();
      TeamID id = (TeamID) it->asInt();
      addTeam( team_name, id );
    }
  }


  void Manager::_loadData( Json::Value& game_data )
  {
    INFO_LOG( "Loading game data" );

    _theData.configure( game_data );
  }


  void Manager::_loadContexts( Json::Value& context_data )
  {
    INFO_LOG( "Loading contexts" );

    _theContexts.configure( context_data );
  }

}

