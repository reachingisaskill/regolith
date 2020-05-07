
#ifndef REGOLITH_SCENE_PLATFORMER_H_
#define REGOLITH_SCENE_PLATFORMER_H_

#include "Managers/Definitions.h"
#include "Scenes/Scene.h"
#include "Signals/Trigger.h"


namespace Regolith
{

  typedef std::vector< Trigger* > TriggerList;
  typedef std::vector< Vector > SpawnPointList;

  class ScenePlatformer : public Scene
  {
    friend class PlatformerSceneFactory;

    private:
      // All memory is owned by the base resource list
      Drawable* _player;
      Dialog* _pauseMenu;

      // Data for player health, death, spawning, etc.
      SpawnPointList _spawnPoints;
      Vector _currentPlayerSpawn;

      // Event triggers
      TriggerList _triggers;

    protected:
      // Function for derived classes to implement when the pause signal is received
      virtual void onPause();

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


      // When paused stop animations
      bool isAnimated() const { return ! isPaused(); }

      // Scane is visible behind other context windows
      bool isVisible() const { return true; }

      // Tell the scene to un-pause
      void returnFocus() { this->resume(); }

      // Raise an event in the current context
      void raiseContextEvent( ContextEvent );
  };

}


#endif // REGOLITH_SCENE_PLATFORMER_H_

