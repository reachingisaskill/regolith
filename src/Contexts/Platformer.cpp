
#include "Regolith/Contexts/Platformer.h"
#include "Regolith/Managers/Manager.h"


namespace Regolith
{


  Platformer::Platformer() :
    Context(),
    _defaultMusic( nullptr ),
    _pauseMenu(),
    _player( nullptr ),
    _spawnPoints( "spawn_points" ),
    _currentPlayerSpawn( 0 )
  {
    setPauseable( true );
  }


  Platformer::~Platformer()
  {
    INFO_LOG( "Destroying Platformer" );
    _spawnPoints.clear();
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Member functions

  void Platformer::onStart()
  {
    if ( _defaultMusic != nullptr )
      owner()->getAudioHandler().setSong( _defaultMusic->getTrack() );

    playerRespawn();
  }


  void Platformer::onStop()
  {
    owner()->getAudioHandler().stopSong();
  }


  void Platformer::onPause()
  {
    DEBUG_LOG( "PLATFORMER PAUSE" );
  }


  void Platformer::onResume()
  {
    DEBUG_LOG( "PLATFORMER RESUME" );
  }


  void Platformer::playerRespawn()
  {
    _player->respawn( _spawnPoints[_currentPlayerSpawn] );
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Configuration

  void Platformer::configure( Json::Value& json_data, ContextGroup& handler )
  {
    INFO_LOG( "Configuring Platformer Context" );

    // Call the base class variant first
    Context::configure( json_data, handler );

    INFO_LOG( "Configuring platformer specific data" );


    Utilities::validateJson( json_data, "character", Utilities::JSON_TYPE_OBJECT );
    Utilities::validateJson( json_data["character"], "name", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data["character"], "layer", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "spawn_points", Utilities::JSON_TYPE_OBJECT );


    if ( Utilities::validateJson( json_data, "default_music", Utilities::JSON_TYPE_STRING, false ) )
    {
      // Set the default music
      std::string default_music = json_data["default_music"].asString();
      INFO_STREAM << "Setting default music: " << default_music;
      _defaultMusic = dynamic_cast< MusicTrack* > ( owner()->getGameObject( default_music ) );

      if ( _defaultMusic == nullptr )
      {
        Exception ex( "LoadScreen::configure()", "Specified track is not a MusicTrack object" );
        ex.addDetail( "Name", default_music );
        throw ex;
      }
    }


    // Set the character
    std::string character_name = json_data["character"]["name"].asString();
    std::string character_layer = json_data["character"]["layer"].asString();

    INFO_STREAM << "Loading the character : " << character_name;

    _player = dynamic_cast< ControllableCharacter* >( Manager::getInstance()->getContextManager().getGlobalContextGroup()->getGameObject( character_name ) );
    if ( _player == nullptr )
    {
      Exception ex( "Platformer::configure()", "Could not locate player character as type: ControllableCharacter." );
      ex.addDetail( "Character Name", character_name );
      ex.addDetail( "Character Layer", character_layer );
      throw ex;
    }

    getLayer( character_layer ).cacheObject( _player );
    INFO_STREAM << "Register platformer character as: \"" << character_name << "\" in layer \"" << character_layer << "\"";


    // Spawn Points
    Json::Value& spawn_points = json_data["spawn_points"];
    for ( Json::Value::iterator spawn_it = spawn_points.begin(); spawn_it != spawn_points.end(); ++spawn_it )
    {
      Utilities::validateJsonArray( *spawn_it, 2, Utilities::JSON_TYPE_FLOAT );

      std::string name = spawn_it.key().asString();

      float x = (*spawn_it)[0].asFloat();
      float y = (*spawn_it)[1].asFloat();
      Vector pos( x, y );

      _spawnPoints.addObject( pos, name );
    }
    INFO_STREAM << "Found " << _spawnPoints.size() << " spawn points";
    

    // Load a pause menu if one is provided
    if ( Utilities::validateJson( json_data, "pause_context", Utilities::JSON_TYPE_STRING, false ) )
    {
      std::string pause = json_data["pause_context"].asString();

      _pauseMenu = owner()->requestContext( pause );

      INFO_STREAM << "Registered pause context: " << pause;
    }
  }


  void Platformer::validate() const
  {
  }

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Action Handling

  void Platformer::registerActions( InputHandler& handler )
  {
    handler.registerInputRequest( this, INPUT_ACTION_QUIT );
    handler.registerInputRequest( this, INPUT_ACTION_PAUSE );
  }


  void Platformer::inputAction( const InputAction& action )
  {
    switch ( action )
    {
      case INPUT_ACTION_QUIT :
        Manager::getInstance()->raiseEvent( REGOLITH_EVENT_QUIT );
        break;

      case INPUT_ACTION_PAUSE :
        DEBUG_STREAM << "PAUSE pressed : " << isPaused() << ", " << _pauseMenu;
        if ( ! isPaused() )
        {
          if ( _pauseMenu != 0 )
          {
            Manager::getInstance()->openContext( *_pauseMenu );
          }
          else
          {
            this->pauseContext();
          }
        }
        else
        {
          this->resumeContext();
        }
        break;
        
      default:
        break;
    }
  }


  void Platformer::booleanAction( const InputAction& action, bool value )
  {
    if ( ! value ) inputAction( action );
//    inputAction( action );
  }


  void Platformer::floatAction( const InputAction&, float )
  {
  }


  void Platformer::vectorAction( const InputAction&, const Vector& )
  {
  }


  void Platformer::mouseAction( const InputAction&, bool, const Vector& )
  {
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Event Handling

  void Platformer::registerEvents( InputManager& manager )
  {
//    manager.registerEventRequest( this, REGOLITH_EVENT_QUIT );
    manager.registerEventRequest( this, REGOLITH_EVENT_CONTEXT_END );
  }


  void Platformer::eventAction( const RegolithEvent& regolith_event, const SDL_Event& /*sdl_event*/ )
  {
    switch ( regolith_event )
    {
//      case REGOLITH_EVENT_QUIT :
      case REGOLITH_EVENT_CONTEXT_END :
        Manager::getInstance()->closeContext();
        break;

      default:
        break;
    }
  }

}

