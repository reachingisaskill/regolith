
#include "Regolith/Contexts/Platformer.h"
#include "Regolith/Managers/Manager.h"

#include "logtastic.h"


namespace Regolith
{


  Platformer::Platformer() :
    Context(),
    _defaultMusic( 0 ),
    _pauseMenu( 0 ),
    _player( nullptr ),
    _spawnPoints( "spawn_points" ),
    _currentPlayerSpawn( 0 )
  {
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
    if ( _defaultMusic != 0 )
      audioHandler()->setSong( _defaultMusic );
  }


  void Platformer::onStop()
  {
    audioHandler()->stopSong();
  }


  void Platformer::onPause()
  {
  }


  void Platformer::onResume()
  {
  }


  void Platformer::playerRespawn()
  {
    _player->respawn( _spawnPoints[_currentPlayerSpawn] );
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Configuration

  void Platformer::configure( Json::Value& json_data )
  {
    INFO_LOG( "Configuring Platformer Context" );
    Utilities::validateJson( json_data, "default_music", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "spawn_points", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJsonArray( json_data["spawn_points"], 1, Utilities::JSON_TYPE_OBJECT );

    // Set the default music
    std::string default_music = json_data["default_music"].asString();
    INFO_STREAM << "Setting default music: " << default_music;
    _defaultMusic = audioHandler()->getMusicID( default_music );


    // Spawn Points
    Json::Value& spawn_points = json_data["spawn_points"];
    Json::ArrayIndex spawn_points_size = spawn_points.size();
    for ( Json::ArrayIndex i = 0; i < spawn_points_size; ++i )
    {
      Utilities::validateJson( json_data, "name", Utilities::JSON_TYPE_STRING );
      Utilities::validateJson( json_data, "position", Utilities::JSON_TYPE_ARRAY );
      Utilities::validateJsonArray( json_data["position"], 2, Utilities::JSON_TYPE_FLOAT );

      std::string name = spawn_points[i]["name"].asString();
      float x = spawn_points[i]["position"][0].asFloat();
      float y = spawn_points[i]["position"][1].asFloat();
      Vector pos( x, y );

      _spawnPoints.addObject( pos, name );
    }
    INFO_STREAM << "Found " << _spawnPoints.size() << " spawn points";
    

    // Load a pause menu if one is provided
    if ( Utilities::validateJson( json_data, "pause_context", Utilities::JSON_TYPE_STRING, false ) )
    {
      std::string pause = json_data["pause_context"].asString();

      _pauseMenu = Manager::getInstance()->requestContext( pause );

      INFO_STREAM << "Registered pause context: " << pause;
    }

    // Call the base class variant to finish the configuration
    Context::configure( json_data );
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
        if ( ! isPaused() )
        {
          if ( _pauseMenu != 0 )
          {
            Manager::getInstance()->openContext( _pauseMenu );
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
    if ( value ) inputAction( action );
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
    manager.registerEventRequest( this, REGOLITH_EVENT_QUIT );
    manager.registerEventRequest( this, REGOLITH_EVENT_CONTEXT_END );
  }


  void Platformer::eventAction( const RegolithEvent& regolith_event, const SDL_Event& /*sdl_event*/ )
  {
    switch ( regolith_event )
    {
      case REGOLITH_EVENT_QUIT :
      case REGOLITH_EVENT_CONTEXT_END :
        Manager::getInstance()->closeContext();
        break;

      default:
        break;
    }
  }

}

