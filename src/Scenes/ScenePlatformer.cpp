
#include "Scenes/ScenePlatformer.h"
#include "Managers/Manager.h"
#include "Components/Exception.h"
#include "Components/Utilities.h"
#include "Components/Collision.h"

#include "logtastic.h"

#include <fstream>


namespace Regolith
{


  ScenePlatformer::ScenePlatformer( std::string mapping ) :
    Scene( mapping ),
    _player( nullptr ),
    _pauseMenu( nullptr ),
    _spawnPoints(),
    _currentPlayerSpawn( 0.0 ),
    _triggers()
  {
  }


  ScenePlatformer::~ScenePlatformer()
  {
    INFO_LOG( "Destroying Scene - Platformer" );
    INFO_LOG( "Clearing caches" );

    INFO_LOG( "Clearing Spawn Points" );
    _spawnPoints.clear();
  }

////////////////////////////////////////////////////////////////////////////////////////////////////
  // ScenePlatformer update and render functions

  void ScenePlatformer::playerRespawn()
  {

    _player->setPosition( _currentPlayerSpawn );
  }


  void ScenePlatformer::_render()
  {
    // Draw the player on top of the environment
    _player->render( getCamera() );
  }


  void ScenePlatformer::_update( Uint32 )
  {
  }


  void ScenePlatformer::_resolveCollisions()
  {
    static Contact contact;

    // Make sure player is still within the scene
    if ( ! contains( this->getBackground(), _player ) )
    {
      DEBUG_LOG( "Player left scene - Respawning" );
      this->playerRespawn();
    }

    // Resolve the trigger collisions
    size_t numberTriggers = _triggers.size();
    for ( size_t i = 0; i < numberTriggers; ++i )
    {
      if ( collides( _player, _triggers[i], contact ) )
      {
        DEBUG_STREAM << "Collided with trigger: " << i;
        switch ( _triggers[i]->getTriggerType() )
        {
          default:
            break;
        }
      }
    }
  }



  void ScenePlatformer::raiseContextEvent( ContextEvent context_event )
  {
    static Manager* man = Manager::getInstance();

    switch ( context_event )
    {
      case CONTEXT_EVENT_QUIT :
        man->raiseEvent( REGOLITH_EVENT_QUIT );
        break;

      case CONTEXT_EVENT_RESPAWN :
        this->playerRespawn();
        break;

      case CONTEXT_EVENT_DEATH :
        this->playerRespawn();
        break;

      case CONTEXT_END :
        man->raiseEvent( REGOLITH_EVENT_CONTEXT_END );
        break;

      case CONTEXT_EVENT_NEXT :
      case CONTEXT_EVENT_PREV :
      case CONTEXT_EVENT_SKIP :

      default:
        break;
    }
  }

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Hooks and callbacks

  void ScenePlatformer::onPause()
  {
    DEBUG_LOG( "On pause called" );
    if ( _pauseMenu != nullptr )
    {
      DEBUG_LOG( "Opening Pause Menu" );
      Manager::getInstance()->openContext( _pauseMenu );
    }
  }

}

