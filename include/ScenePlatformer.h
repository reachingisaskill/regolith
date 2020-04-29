
#ifndef __REGOLITH__SCENE_PLATFORMER_H__
#define __REGOLITH__SCENE_PLATFORMER_H__

#include "Definitions.h"
#include "Scene.h"
#include "Trigger.h"


namespace Regolith
{

  typedef std::vector< Trigger* > TriggerList;
  typedef std::vector< Vector > SpawnPointList;

  class ScenePlatformer : public Scene
  {
    private:
      // All memory is owned by the base resource list
      Drawable* _player;

      // Data for player health, death, spawning, etc.
      SpawnPointList _spawnPoints;
      Vector _currentPlayerSpawn;

      // Event triggers
      TriggerList _triggers;

    protected:

      // Function to build the camera objects
      void _loadSpawnPoints( Json::Value& );

      // Override this function to allow the base class to build components specific to this scene type
      virtual void _loadSceneSpecificComponents( Json::Value& );

      // Load the player model
      void _loadPlayer( Json::Value& );


      // Derived class render
      virtual void _render();

      // Derived class udpate
      virtual void _update( Uint32 );

      // Derived class render
      virtual void _resolveCollisions();

    public:
      // Set the necessary parameters
      ScenePlatformer( std::string );

      // Remove all the cached data and clean up
      ~ScenePlatformer();

      // Respawn the player at the last spawn point
      void playerRespawn();

      // Raise an event in the current context
      void raiseContextEvent( ContextEvent );
  };

}


#endif // __REGOLITH__SCENE_PLATFORMER_H__

