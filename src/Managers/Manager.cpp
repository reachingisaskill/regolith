
#include "Regolith/Managers/Manager.h"
#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/Managers/InputManager.h"
#include "Regolith/Managers/AudioManager.h"
#include "Regolith/Managers/HardwareManager.h"
#include "Regolith/Managers/CollisionManager.h"
#include "Regolith/Managers/DataManager.h"
#include "Regolith/Managers/ThreadManager.h"
#include "Regolith/Managers/ContextManager.h"
#include "Regolith/Managers/FontManager.h"
#include "Regolith/Managers/WindowManager.h"
#include "Regolith/Managers/EngineManager.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  // Forward declare the signal handler
  void deathSignals( int );


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Con/Destruction

  Manager::Manager() :
    _theThreads( nullptr ),
    _theWindow( nullptr ),
    _theInput( nullptr ),
    _theAudio( nullptr ),
    _theHardware( nullptr ),
    _theCollision( nullptr ),
    _theData( nullptr ),
    _theContexts( nullptr ),
    _theFonts( nullptr ),
    _theEngine( nullptr ),
    _objectFactory(),
    _contextFactory(),
    _eventStartIndex(0),
    _gameEvents()
  {
    DEBUG_LOG( "Manager::Manager : Contruction" );
    // Set up signal handlers
    logtastic::registerSignalHandler( SIGABRT, deathSignals );
    logtastic::registerSignalHandler( SIGFPE, deathSignals );
    logtastic::registerSignalHandler( SIGILL, deathSignals );
    logtastic::registerSignalHandler( SIGINT, deathSignals );
    logtastic::registerSignalHandler( SIGSEGV, deathSignals );
    logtastic::registerSignalHandler( SIGTERM, deathSignals );


    INFO_LOG( "Manager::Manager : Allocating memory for the managers." );
    this->_allocateManagers();
  }


  Manager::~Manager()
  {
    DEBUG_LOG( "Manager::~Manager : Destruction" );

    INFO_LOG( "Manager::~Manager : Clearing collision data" );
    _theCollision->clear();

    INFO_LOG( "Manager::~Manager : Clearing font data" );
    _theFonts->clear();

    INFO_LOG( "Manager::~Manager : Clearing audio manager" );
    _theAudio->clear();

    INFO_LOG( "Manager::~Manager : Clearing hardware manager" );
    _theHardware->clear();


    INFO_LOG( "Manager::~Manager : Deallocating managers" );
    this->_deallocateManagers();


    INFO_LOG( "Manager::~Manager : Closing the SDL subsystems" );
    TTF_Quit();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Run!

  bool Manager::run()
  {
    bool success = true;
    try
    {
      // Start all the waiting threads
      INFO_LOG( "Manager::run : Starting worker threads" );
      _theThreads->startAll();


      try
      {
        // Load the first context group blocking this thread until completion
        INFO_LOG( "Manager::run : Loading entry point" );
        ContextGroup* entryPoint = _theContexts->loadEntryPoint();

        // Reset the stack to the first context
        INFO_LOG( "Manager::run : Loading the first context" );
//        this->openEntryPoint();
        _theEngine->openContextStack( entryPoint->getEntryPoint() );

        // Start the engine!
        INFO_LOG( "Manager::run : Starting the engine." );
        _theEngine->run();
      }
      catch ( Exception& ex )
      {
        success = false;
        ERROR_LOG( "Manager::run : A Regolith error occured during runtime" );
        ERROR_STREAM << ex.elucidate();
        std::cerr << ex.elucidate();
      }


      try
      {
        // Unload everything
        INFO_LOG( "Manager::run : Unloading data" );
        _theContexts->clear();
      }
      catch ( Exception& ex )
      {
        success = false;
        ERROR_LOG( "Manager::run : A Regolith exception occured unloading data" );
        throw ex;
      }

        // Stop all the threads
      INFO_LOG( "Manager::run : Stopping all worker threads" );
      _theThreads->stopAll();

    }
    catch ( std::exception& ex )
    {
      success = false;
      ERROR_LOG( "Manager::run : A unexpected exception occured." );
      ERROR_STREAM << ex.what();
      _theThreads->error();
    }

    // Join all the threads
    INFO_LOG( "Manager::run : Joining all worker threads" );
    _theThreads->join();

    return success;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Functions accessible during game play

  // Events

  void Manager::error()
  {
    _theThreads->error();
  }


  void Manager::raiseEvent( RegolithEvent eventNum )
  {
    SDL_PushEvent( &_gameEvents[ eventNum ] );
  }


  // Collision Interface

  CollisionTeam Manager::getCollisionTeam( std::string name )
  {
    return _theCollision->getCollisionTeam( name );
  }


  CollisionType Manager::getCollisionType( std::string name )
  {
    return _theCollision->getCollisionType( name );
  }


  // Music Interface

  void Manager::loadPlaylist( Playlist* pl )
  {
    _theAudio->load( pl );
  }


  void Manager::pauseTrack()
  {
    _theAudio->pauseTrack();
  }


  void Manager::resumeTrack()
  {
    _theAudio->resumeTrack();
  }


  void Manager::stopRepeatTrack()
  {
    _theAudio->stopRepeatTrack();
  }


  void Manager::nextTrack()
  {
    _theAudio->nextTrack();
  }


  void Manager::nextRepeatTrack()
  {
    _theAudio->nextRepeatTrack();
  }


  // Window Interface

  void Manager::setWindowTitle( std::string t )
  {
    _theWindow->setTitle( t );
  }


  // Fonts interface

  Pen Manager::requestPen( std::string n, unsigned int s, SDL_Color c )
  {
    return _theFonts->requestPen( n, s, c );
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Initialisation and configuration

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
      _theEngine->registerEvents( *_theInput );
      _theHardware->registerEvents( *_theInput );


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


  void Manager::_allocateManagers()
  {
    _theInput = new InputManager();
    _theAudio = new AudioManager();
    _theHardware = new HardwareManager();
    _theCollision = new CollisionManager();
    _theData = new DataManager();
    _theThreads = new ThreadManager();
    _theContexts = new ContextManager();
    _theFonts = new FontManager();
    _theWindow = new WindowManager();
    _theEngine = new EngineManager();
  }


  void Manager::_deallocateManagers()
  {
    delete _theInput;
    delete _theAudio;
    delete _theHardware;
    delete _theCollision;
    delete _theData;
    delete _theThreads;
    delete _theContexts;
    delete _theFonts;
    delete _theWindow;
    delete _theEngine;

    _theInput = nullptr;
    _theAudio = nullptr;
    _theHardware = nullptr;
    _theCollision = nullptr;
    _theData = nullptr;
    _theThreads = nullptr;
    _theContexts = nullptr;
    _theFonts = nullptr;
    _theWindow = nullptr;
    _theEngine = nullptr;
  }


  void Manager::_loadInput( Json::Value& json_data )
  {
    try
    {
      INFO_LOG( "Manager::_loadInput : Configuring the input manager" );
      _theInput->configure( json_data );

      // Register the regolith events
      _theInput->registerEvents( *_theInput );
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
      _theAudio->configure( json_data );

      // Register the regolith events
      _theAudio->registerEvents( *_theInput );
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
    _theWindow->configure( json_data );

    // Register the regolith events with the input manager
    _theWindow->registerEvents( *_theInput );
  }


  void Manager::_loadCollision( Json::Value& json_data )
  {
    INFO_LOG( "Manager::_loadCollision : Loading collision data" );
    _theCollision->configure( json_data );

    // Register the regolith events
    _theCollision->registerEvents( *_theInput );
  }


  void Manager::_loadData( Json::Value& game_data )
  {
    INFO_LOG( "Manager::_loadData : Loading game data" );
    _theData->configure( game_data );

    // Register the regolith events
    _theData->registerEvents( *_theInput );
  }


  void Manager::_loadFonts( Json::Value& font_data )
  {
    INFO_LOG( "Manager::_loadFonts : Loading font data" );
    _theFonts->configure( font_data );

    // Register the regolith events
    _theFonts->registerEvents( *_theInput );
  }


  void Manager::_loadContexts( Json::Value& context_data )
  {
    INFO_LOG( "Manager::_loadContexts : Loading contexts" );
    _theContexts->configure( context_data );

    // Register the regolith events
    _theContexts->registerEvents( *_theInput );
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


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Signal handler

#ifdef __linux__
#include <execinfo.h>
#include <stdio.h>
#include <unistd.h>
#endif

  void deathSignals( int signal )
  {
    FAILURE_STREAM << "DEATHSIGNAL : Regolith received signal: " << signal;
    FAILURE_LOG( "DEATHSIGNAL : Trying to die gracefully..." );

#ifdef __linux__
    void* array[20];
    size_t size;

    size = backtrace( array, 50 );

    fprintf( stderr, "Error Signal %d:\n", signal );
    backtrace_symbols_fd( array, size, STDERR_FILENO );
#endif

    Manager::getInstance()->error();
  }

}

