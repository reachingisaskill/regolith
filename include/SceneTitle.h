
#ifndef REGOLITH_SCENE_TITLE_H_
#define REGOLITH_SCENE_TITLE_H_

#include "Definitions.h"
#include "Scene.h"
#include "Trigger.h"


namespace Regolith
{

  typedef std::vector< Trigger* > TriggerList;
  typedef std::vector< Vector > SpawnPointList;

  class SceneTitle : public Scene
  {
    private:
      Dialog* _startingDialog;

    protected:
      void onStart();

      // Override this function to allow the base class to build components specific to this scene type
      virtual void _loadSceneSpecificComponents( Json::Value& );

      // Load the options section
      void _loadOptions( Json::Value& );

      // Derived class render
      virtual void _render();

      // Derived class udpate
      virtual void _update( Uint32 );

      // Derived class render
      virtual void _resolveCollisions();

    public:
      // Set the necessary parameters
      SceneTitle( std::string );

      // Remove all the cached data and clean up
      ~SceneTitle();


      // When paused stop animations
      bool isAnimated() const { return ! isPaused(); }

      // Scane is visible behind other context windows
      bool isVisible() const { return true; }

      // Tell the scene to un-pause
      void returnFocus();

      // Raise an event in the current context
      void raiseContextEvent( ContextEvent );
  };

}


#endif // REGOLITH_SCENE_TITLE_H_

