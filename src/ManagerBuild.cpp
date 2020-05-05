
#include "Manager.h"
#include "Utilities.h"
#include "Engine.h"


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

    // Setup an engine object
    _theEngine = Engine::createInstance();


    try
    {
      // Load and parse the json config
      std::ifstream input( json_file );
      Json::Value json_data;
      Json::CharReaderBuilder reader_builder;
      Json::CharReader* reader = reader_builder.newCharReader();
      std::string errors;
      bool result = Json::parseFromStream( reader_builder, input, &json_data, &errors );
      if ( ! result )
      {
        ERROR_LOG( "Manager::init() : Found errors parsing json" );
        ERROR_STREAM << "\"" << errors << "\"";
      }
      delete reader;

      // Validate the required keys
      Utilities::validateJson( json_data, "window", Utilities::JSON_TYPE_OBJECT );
      Utilities::validateJson( json_data, "fonts", Utilities::JSON_TYPE_OBJECT );
      Utilities::validateJson( json_data, "input_device", Utilities::JSON_TYPE_OBJECT );
      Utilities::validateJson( json_data, "audio_device", Utilities::JSON_TYPE_OBJECT );
      Utilities::validateJson( json_data, "textures", Utilities::JSON_TYPE_OBJECT );
      Utilities::validateJson( json_data, "story", Utilities::JSON_TYPE_OBJECT );


      // Load the input device configuration first so objects can register game-wide behaviours
      this->_loadInput( json_data["input_device"] );
      // Engine gets to register its events first
      _theEngine->registerEvents( _theInput );


      // Load the audio device configuration
      this->_loadAudio( json_data["audio_device"] );


      // Load the Font data
      this->_loadFonts( json_data["fonts"] );


      // Configure the window
      this->_loadWindow( json_data["window"] );


      // Load all the texture files
      this->_loadTextures( json_data["textures"] );


      // Load all the texture files
      this->_loadResources( json_data["resources"] );


      // Load all the texture files
      this->_loadStory( json_data["story"] );

    }
    catch ( std::ios_base::failure& f ) // Thrown by ifstream
    {
      FAILURE_LOG( "Manager::init() : IFStream error occured" );
      FAILURE_STREAM << f.what();
      Exception ex( "Manager::init()", "Default font not found", false );
      ex.addDetail( "What", f.what() );
      throw ex;
    }
    catch ( std::runtime_error& rt ) // Thrown by jsoncpp
    {
      FAILURE_LOG( "Manager::init() : Parsing/exploring error in json files" );
      FAILURE_STREAM << rt.what();
      Exception ex( "Manager::init()", "Json parsing error", false );
      ex.addDetail( "What", rt.what() );
      throw ex;
    }

    this->configureEvents();
    _theEngine->configure( _theRenderer, _theWindow, _theInput );
  }


  void Manager::_loadInput( Json::Value& json_data )
  {
    Utilities::validateJson( json_data, "require", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJson( json_data, "keymappings", Utilities::JSON_TYPE_ARRAY );

    _theInput = new InputManager();

    // Configure the input objects
    Json::Value required = json_data["require"];
    Json::Value keymaps = json_data["keymappings"];

    // Quick vallidation - TODO: add a hardware check function to makesure hardware actually exists
    Json::ArrayIndex required_size = required.size();
    for ( Json::ArrayIndex i = 0; i != required_size; ++i )
    {
      if ( required[i].asString() == "controller" )
      {
        WARN_LOG( "Controllers are not yet supported!" );
      }
      else if ( required[i].asString() == "joystick" )
      {
        WARN_LOG( "Joystick is not yet supported!" );
      }
    }


    Json::ArrayIndex keymaps_size = required.size();
    for ( Json::ArrayIndex i = 0; i != keymaps_size; ++i )
    {
      Utilities::validateJson( keymaps[i], "name", Utilities::JSON_TYPE_STRING );
      Utilities::validateJson( keymaps[i], "keymapping", Utilities::JSON_TYPE_ARRAY );

      std::string mapping_name = keymaps[i]["name"].asString();
      Json::Value keymapping = keymaps[i]["keymapping"];

      _theInput->requestMapping( mapping_name );

      // Loop over the key mapping hardware
      Json::ArrayIndex keymapping_size = keymapping.size();
      for ( Json::ArrayIndex j = 0; j != keymapping_size; ++j )
      {
        Utilities::validateJson( keymapping[j], "type", Utilities::JSON_TYPE_STRING );
        Utilities::validateJson( keymapping[j], "mapping", Utilities::JSON_TYPE_OBJECT );

        std::string type = keymapping[j]["type"].asString();
        Json::Value keys = keymapping[j]["mapping"];

        if ( type == "keyboard" )
        {
          INFO_LOG( "Loading Keyboard Mapping." );

          Json::Value::const_iterator keys_end = keys.end();
          for ( Json::Value::const_iterator it = keys.begin(); it != keys_end; ++it )
          {
            SDL_Scancode code = getScancodeID( it.key().asString() );
            InputAction action = getActionID( it->asString() );
            _theInput->registerInputAction( mapping_name, INPUT_TYPE_KEYBOARD, code, action );
            INFO_STREAM << "Key Registered to map: " << mapping_name << " -- " << it.key().asString() << "(" << code << ")" << " as action : " << it->asString() << "(" << action << ")";
          }
        }

        else if ( type == "mouse_buttons" )
        {
          INFO_LOG( "Loading Mouse Button Mapping." );

          Json::Value::const_iterator keys_end = keys.end();
          for ( Json::Value::const_iterator it = keys.begin(); it != keys_end; ++it )
          {
            MouseButton code = getMouseButtonID( it.key().asString() );
            InputAction action = getActionID( it->asString() );
            _theInput->registerInputAction( mapping_name, INPUT_TYPE_MOUSE_BUTTON, code, action );
            INFO_STREAM << "Mouse Button Registered to map: " << mapping_name << " -- " << it.key().asString() << "(" << code << ")" << " as action : " << it->asString() << "(" << action << ")";
          }
        }

        else if ( type == "mouse_movement" )
        {
          INFO_LOG( "Loading Mouse Movement." );

          // Only require the movement input - it must be present if a mouse movement mapping is provided
          Utilities::validateJson( keys, "movement", Utilities::JSON_TYPE_STRING );

          unsigned code = 0; // Dummy variable - only one thing to map with mouse movement!
          InputAction action = getActionID( keys["movement"].asString() );
          _theInput->registerInputAction( mapping_name, INPUT_TYPE_MOUSE_MOVE, code, action );
          INFO_STREAM << "Registered : mouse movement as action to map: " << mapping_name << " -- " << keys["movement"].asString() << "(" << action << ")";
        }
        else
        {
          WARN_LOG( "Key mapping specified for an unsupported interface." );
          WARN_LOG( "Please try again with a future version." );
        }
      }
    }
  }


  void Manager::_loadAudio( Json::Value& json_data )
  {
//    Utilities::validateJson( json_data, "require", Utilities::JSON_TYPE_ARRAY );

    try
    {
      INFO_LOG( "Configuring the audio manager" );
      Utilities::validateJson( json_data, "sample_frequency", Utilities::JSON_TYPE_INTEGER );
      Utilities::validateJson( json_data, "audio_channels", Utilities::JSON_TYPE_INTEGER );
      Utilities::validateJson( json_data, "chunk_size", Utilities::JSON_TYPE_INTEGER );
      Utilities::validateJson( json_data, "music_volume", Utilities::JSON_TYPE_FLOAT );
      Utilities::validateJson( json_data, "effect_volume", Utilities::JSON_TYPE_FLOAT );
      Utilities::validateJson( json_data, "music_files", Utilities::JSON_TYPE_ARRAY );
      Utilities::validateJson( json_data, "effect_files", Utilities::JSON_TYPE_ARRAY );

      int sample_freq = json_data["sample_frequency"].asInt();
      int audio_channels = json_data["audio_channels"].asInt();
      int chunk_size = json_data["chunk_size"].asInt();
      float music_volume = json_data["music_volume"].asFloat();
      float effect_volume = json_data["effect_volume"].asFloat();

      // Create the audio handler
      _theAudio = new AudioManager( sample_freq, audio_channels, chunk_size );

      // Set the volumes
      _theAudio->setVolumeMusic( music_volume );
      _theAudio->setVolumeEffects( effect_volume );

      // Load the music & effect files
      INFO_LOG( "Loading sound files" );
      Json::Value music_files = json_data["music_files"];
      Json::Value effect_files = json_data["effect_files"];

      _theAudio->configure( music_files, effect_files );
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

    // Create the window
    _theWindow = new Window( title );
    _theRenderer = _theWindow->init( screen_width, screen_height );
    _theBuilder->setRenderer( _theRenderer );
    _theWindow->registerEvents( _theInput );

    // Set the default colour
    Json::Value color = json_data["default_color"];
    _defaultColor.r = color[0].asInt();
    _defaultColor.g = color[1].asInt();
    _defaultColor.b = color[2].asInt();
    _defaultColor.a = color[3].asInt();
  }


  void Manager::_loadTextures( Json::Value& json_data )
  {
    Utilities::validateJson( json_data, "texture_files", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJson( json_data, "texture_strings", Utilities::JSON_TYPE_ARRAY );

    // Load and cache the individual texture files
    Json::Value texture_files = json_data["texture_files"];
    Json::ArrayIndex texture_files_size = texture_files.size();
    for ( Json::ArrayIndex i = 0; i != texture_files_size; ++i )
    {
      RawTexture texture = makeTextureFromFile( texture_files[i] );
      std::string name = texture_files[i]["name"].asString();
      _rawTextures[name] = texture;
    }

    // Load and cache the individual text textures
    Json::Value texture_strings = json_data["texture_strings"];
    Json::ArrayIndex texture_strings_size = texture_strings.size();
    for ( Json::ArrayIndex i = 0; i != texture_strings_size; ++i )
    {
      RawTexture texture = makeTextureFromText( texture_strings[i] );
      std::string name = texture_strings[i]["name"].asString();
      _rawTextures[name] = texture;
    }
  }


  void Manager::_loadResources( Json::Value& resources )
  {
    INFO_LOG( "Loading resource objects" );
    Json::ArrayIndex resources_size = resources.size();
    for ( Json::ArrayIndex i = 0; i != resources_size; ++i )
    {
      Utilities::validateJson( resources[i], "resource_name", Utilities::JSON_TYPE_STRING );
      Utilities::validateJson( resources[i], "resource_type", Utilities::JSON_TYPE_STRING );
      Utilities::validateJson( resources[i], "team_name", Utilities::JSON_TYPE_STRING );

      std::string resource_name = resources[i]["resource_name"].asString();
      std::string resource_type = resources[i]["resource_type"].asString();
      std::string team_name = resources[i]["team_name"].asString();

      DEBUG_STREAM << "  Building resource: " << resource_name << " as " << resource_type << " for team: " << team_name;

      Drawable* newResource =  _theBuilder->build( resources[i] );
      _resources.addObject( newResource, resource_name );

      // See if its already been used.
      if ( _teamNames.find( team_name ) == _teamNames.end() )
      {
        // Create it otherwise
        _teamNames[ team_name ] = _teamNames.size();
      }

      DEBUG_LOG( "Setting team name" );
      int team_id = _teamNames[ team_name ];
      newResource->setTeam( team_id );
    }
  }


  void Manager::_loadStory( Json::Value& json_data )
  {
    Utilities::validateJson( json_data, "scenes", Utilities::JSON_TYPE_ARRAY );

    // Load all the scenes into memory
    Json::Value scene_data = json_data["scenes"];
    Json::ArrayIndex scenes_size = scene_data.size();
    for ( Json::ArrayIndex i = 0; i < scenes_size; ++i )
    {
      Utilities::validateJson( scene_data[i], "name", Utilities::JSON_TYPE_STRING );
      std::string name = scene_data[i]["name"].asString();

      _scenes.addName( name );
    }

    for ( Json::ArrayIndex i = 0; i < scenes_size; ++i )
    {
      Utilities::validateJson( scene_data[i], "name", Utilities::JSON_TYPE_STRING );
      std::string name = scene_data[i]["name"].asString();

      Scene* new_scene = _theSceneBuilder->build( scene_data[i] );
      _scenes.set( name, new_scene );
      INFO_STREAM << "Configured Scene: " << name;
    }
  }

}

