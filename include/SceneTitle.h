
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
    friend class TitleSceneFactory;

    private:
      Dialog* _startingDialog;

    protected:
      void onStart();

      // Derived class render
      virtual void _render();

      // Derived class udpate
      virtual void _update( Uint32 );

      // Derived class render
      virtual void _resolveCollisions();

    public:
      // Name of input mapping
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

