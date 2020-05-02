
#include "ScenePlatformer.h"

#include "Exception.h"
#include "Utilities.h"
#include "Manager.h"
#include "Collision.h"

#include "logtastic.h"

#include <fstream>


namespace Regolith
{


  ScenePlatformer::ScenePlatformer( std::string json_file ) :
    Scene( json_file ),
    _player( nullptr ),
    _spawnPoints(),
    _currentPlayerSpawn( 0.0 )
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
  // All the loading functions to import Json data and build the necessary objects


  void ScenePlatformer::_loadSceneSpecificComponents( Json::Value& json_data )
  {
    if ( ! json_data.isMember( "player" ) )
    {
      FAILURE_LOG( "Could not find player data in json data file" );
      Exception ex( "Scene::_loadSceneSpecificComponents()", "No player member found in Json Data", false);
      ex.addDetail( "File name", getSceneFile() );
      throw ex;
    }
    INFO_LOG( "Loading spawn points" );
    this->_loadPlayer( json_data["player"] );

    if ( ! json_data.isMember( "spawn_points" ) )
    {
      FAILURE_LOG( "Could not find spawn_points data in json data file" );
      Exception ex( "Scene::_loadSceneSpecificComponents()", "No spawn_points member found in Json Data", false);
      ex.addDetail( "File name", getSceneFile() );
      throw ex;
    }
    INFO_LOG( "Loading spawn points" );
    this->_loadSpawnPoints( json_data["spawn_points"] );
  }


  void ScenePlatformer::_loadPlayer( Json::Value& player )
  {
    // Load the scene player character
    INFO_LOG( "Building the player" );
    Utilities::validateJson( player, "position", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJson( player, "resource_name", Utilities::JSON_TYPE_STRING );

    std::string player_resource = player["resource_name"].asString();
    int player_x = player["position"][0].asInt();
    int player_y = player["position"][1].asInt();
    Vector pos( player_x, player_y );

    _player = Manager::getInstance()->getResource( player_resource );
    _player->setPosition( pos );
    _currentPlayerSpawn = pos;

    // Add object to collision caches, etc, and register actions
    this->addSpawned( _player );

    // Tell the current camera object to follow the player
    this->getCamera()->followMe( _player );
  }


  void ScenePlatformer::_loadSpawnPoints( Json::Value& spawnpoints )
  {
    INFO_LOG( "Loading spawn points into queue" );
    if ( ! spawnpoints.isArray() )
    {
      FAILURE_LOG( "Spawn point list is not an array" );
      Exception ex( "ScenePlatformer::_loadSpawnPoints()", "Expected spawn point list to be an array" );
      ex.addDetail( "File name", getSceneFile() );
      throw ex;
    }

    Json::ArrayIndex spawnpoints_size = spawnpoints.size();
    for ( Json::ArrayIndex i = 0; i != spawnpoints_size; ++i )
    {
      Utilities::validateJsonArray( spawnpoints[i], 2, Utilities::JSON_TYPE_FLOAT );

      float x = spawnpoints[i][0].asFloat();
      float y = spawnpoints[i][1].asFloat();

      _spawnPoints.push_back( Vector( x, y ) );
    }
    INFO_STREAM << "Loaded " << spawnpoints_size << " spawn points into scene";
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


  void ScenePlatformer::_update( Uint32 time )
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

}

