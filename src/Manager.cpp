
#include "Manager.h"

#include "Utilities.h"
#include "Exception.h"
#include "Engine.h"
#include "ScenePlatformer.h"

#include "logtastic.h"


namespace Regolith
{

  Manager::Manager() :
    _theEngine( nullptr ),
    _theWindow( nullptr ),
    _theRenderer( nullptr ),
    _theInput( nullptr ),
    _currentContext( nullptr ),
    _theBuilder( new ObjectBuilder() ),
    _theSceneBuilder( new SceneBuilder() ),
    _scenes(),
    _fonts(),
    _title(),
    _defaultFont( nullptr ),
    _defaultColor( { 255, 255, 255, 255 } ),
    _eventStartIndex(0),
    _gameEvents(),
    _gravityConst( 0.0, 0.01 ),
    _dragConst( 0.005 )
  {
    // Set up the provided factories
    _theBuilder->addFactory( new SpriteFactory() );
    _theBuilder->addFactory( new FPSStringFactory() );

    // Set up the scene factories
    _theSceneBuilder->addFactory( new SceneFactory<ScenePlatformer>( "platformer" ) );
  }


  Manager::~Manager()
  {
    // Destroy the builder
    delete _theBuilder;
    _theBuilder = nullptr;

    // Destroy the scene builder
    delete _theSceneBuilder;
    _theSceneBuilder = nullptr;

    // Remove the engine
    Engine::killInstance();
    _theEngine = nullptr;

    // Remove each of scenes and clear the vector
    for ( size_t i = 0; i < _scenes.size(); ++i )
    {
      delete _scenes[i];
    }
    _scenes.clear();

    // Remove each of the fonts and clear the map
    for ( FontMap::iterator it = _fonts.begin(); it != _fonts.end(); ++it )
    {
      TTF_CloseFont( it->second );
    }
    _fonts.clear();

    _defaultFont = nullptr; // No longer valid


    // Remove the renderer object
    SDL_DestroyRenderer( _theRenderer );
    _theRenderer = nullptr;


    // Remove the window
    delete _theWindow;
    _theWindow = nullptr;



    // Close the SDL subsystems
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
  }


  void Manager::init( std::string json_file )
  {
    // Initialise the SDL subsystems
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
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


      // Load the input device configuration first so objects can register game-wide behaviours
      this->_loadInput( json_data["input"] );


      // Engine gets to register its events
      _theEngine->registerEvents( _theInput );



      // Load the window configuration
      int screen_width = json_data["screen_width"].asInt();
      int screen_height = json_data["screen_height"].asInt();
      std::string _title = json_data["title"].asString();

      // Create the window
      _theWindow = new Window( _title );
      _theRenderer = _theWindow->init( screen_width, screen_height );
      _theBuilder->setRenderer( _theRenderer );
      _theWindow->registerEvents( _theInput );

      // Set the default colour
      Json::Value color = json_data["default_color"];
      _defaultColor.r = color[0].asInt();
      _defaultColor.g = color[1].asInt();
      _defaultColor.b = color[2].asInt();
      _defaultColor.a = color[3].asInt();



      // Find out what the default font is called
      std::string default_font = json_data["default_font"].asString();

      // Load the listed fonts
      Json::Value font_data = json_data["fonts"];
      Json::ArrayIndex fonts_size = font_data.size();
      INFO_STREAM << "Loading " << fonts_size << " fonts";
      for ( Json::ArrayIndex i = 0; i < fonts_size; ++i )
      {
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
            FAILURE_LOG( "Manager::init() : Failed to load the default font" );
            Exception ex( "Manager::init()", "Can not load default font", false );
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
        FAILURE_LOG( "Manager::init() : The default font is not loaded by the config file" );
        Exception ex( "Manager::init()", "Default font not found", false );
        ex.addDetail( "Font name", default_font );
        throw ex;
      }
      else
      {
        // Set the default font pointer
        _defaultFont = _fonts[ default_font ];
        INFO_STREAM << "Default font, " << default_font << " identified " << _defaultFont;
      }


      // Load all the scenes into memory
      Json::Value scene_data = json_data["scenes"];
      Json::ArrayIndex scenes_size = scene_data.size();
      for ( Json::ArrayIndex i = 0; i < scenes_size; ++i )
      {
        Scene* new_scene = _theSceneBuilder->build( scene_data[i] );
        new_scene->configure( _theRenderer, _theWindow, _theBuilder );
        _scenes.push_back( new_scene );
      }

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
    Utilities::validateJson( json_data, "keymapping", Utilities::JSON_TYPE_ARRAY );

    _theInput = new InputManager();

    try
    {
      // Configure the input objects
      Json::Value required = json_data["require"];
      Json::Value keymaps = json_data["keymapping"];

      Json::ArrayIndex required_size = required.size();
      for ( Json::ArrayIndex i = 0; i != required_size; ++i )
      {
        if ( required[i].asString() == "controller" )
        {
          WARN_LOG( "Controllers are not yet supported!" );
        }
        else if ( required[i].asString() == "mouse" )
        {
          WARN_LOG( "Mouse interface is not yet supported!" );
        }
        else if ( required[i].asString() == "joystick" )
        {
          WARN_LOG( "Joystick is not yet supported!" );
        }
      }

      Json::ArrayIndex keymaps_size = required.size();
      for ( Json::ArrayIndex i = 0; i != keymaps_size; ++i )
      {
        Json::Value keymap = keymaps[i];
        Utilities::validateJson( keymap, "type", Utilities::JSON_TYPE_STRING );
        Utilities::validateJson( keymap, "mapping", Utilities::JSON_TYPE_OBJECT );

        INFO_LOG( "Loading Keyboard Mapping." );
        if ( keymap["type"].asString() == "keyboard" )
        {
          Json::Value keys = keymap["mapping"];
          Json::Value::const_iterator keys_end = keys.end();
          for ( Json::Value::const_iterator it = keys.begin(); it != keys_end; ++it )
          {
            SDL_Scancode code = getScancodeID( it.key().asString() );
            InputAction action = getActionID( it->asString() );
            _theInput->registerBehaviour( INPUT_TYPE_KEYBOARD, code, (InputBehaviour)action );
            INFO_STREAM << "Registered : " << it.key().asString() << "(" << code << ")" << " as action : " << it->asString() << "(" << action << ")";
          }
        }
        else
        {
          WARN_LOG( "Key mapping specified for an unsupported interface." );
          WARN_LOG( "Please try again with a future version." );
        }
      }

    }
    catch ( std::runtime_error& rt )
    {
      Exception ex( "Manager::_loadInput()", "Json reading failure" );
      ex.addDetail( "What", rt.what() );
      throw ex;
    }
  }


  void Manager::run()
  {
    if ( getNumberScenes() <= 0 )
    {
      FAILURE_LOG( "Scene list is empty - there are no scenes to render!" );
      Exception ex( "Manager::run()", "No scenes to render", false );
      throw ex;
    }
    // This function should be used to start the "story" class running
  }


  TTF_Font* Manager::getFontPointer( std::string name )
  {
    FontMap::iterator find = _fonts.find( name );
    if ( find == _fonts.end() )
    {
      ERROR_STREAM << "Could not find requested font : " << name;
      return _defaultFont;
    }
    return find->second;
  }


  Scene* Manager::getScene( size_t scene_num )
  {
    if ( scene_num >= _scenes.size() )
    {
      FAILURE_STREAM << "Request scene out of bounds : " << scene_num;
      Exception ex( "Manager::getScene()", "Scene number out of bounds", false );
      ex.addDetail( "Scene number", scene_num );
      throw ex;
    }
    return _scenes[ scene_num ];
  }


  Scene* Manager::loadScene( size_t scene_num )
  {
    if ( scene_num >= _scenes.size() )
    {
      FAILURE_STREAM << "Request scene out of bounds : " << scene_num;
      Exception ex( "Manager::getScene()", "Scene number out of bounds", false );
      ex.addDetail( "Scene number", scene_num );
      throw ex;
    }
    _scenes[ scene_num ]->load();
    return _scenes[ scene_num ];
  }


  void Manager::configureEvents()
  {
    // Load user events, etc
    Uint32 start_num = SDL_RegisterEvents( REGOLITH_EVENT_TOTAL );
    INFO_STREAM << "Registering " << REGOLITH_EVENT_TOTAL << " user events";

    if ( start_num == (unsigned int)-1 )
    {
      std::string error = SDL_GetError();
      FAILURE_STREAM << "Could not create required user events : " << error;
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
    /*
    // Load user events, etc
    Uint32 start_num = SDL_RegisterEvents( REGOLITH_EVENT_TOTAL );
    INFO_STREAM << "Registering " << REGOLITH_EVENT_TOTAL << " user events";

    if ( start_num == (unsigned int)-1 )
    {
      std::string error = SDL_GetError();
      FAILURE_STREAM << "Could not create required user events : " << error;
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
    */
  }


  void Manager::raiseEvent( RegolithEvent eventNum )
  {
    SDL_PushEvent( &_gameEvents[ eventNum ] );
  }
}

