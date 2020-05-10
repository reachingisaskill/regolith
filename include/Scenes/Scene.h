
#ifndef REGOLITH_SCENE_H_
#define REGOLITH_SCENE_H_

#include "Managers/Definitions.h"
#include "Architecture/Context.h"
#include "Architecture/Controllable.h"
#include "Architecture/Drawable.h"
#include "GameObjects/ObjectBuilder.h"
#include "Dialogs/Dialog.h"
#include "Components/NamedVector.h"
#include "Components/Window.h"
#include "Components/Camera.h"

#include <vector>
#include <queue>
#include <map>


namespace Regolith
{


  // Useful Typedefs
  typedef std::list< Drawable* > ElementList;
  typedef std::vector< Drawable* > ElementVector;
  typedef std::vector< ElementList > TeamsList;


  class Scene : public Context
  {
    friend class SceneFactory;

    private:
      // Scene state information
      bool _isLoaded;
      bool _paused;
      int _defaultMusic;

      // Containers storing drawable objects - the scene and the hud elements
      // All memory is owned by Scene
      Drawable* _background;
      Drawable* _foreground;
      ElementList _sceneElements;
      ElementList _hudElements;

      // Daughter contexts
      NamedVector<Dialog, true> _dialogWindows;

      // Can be change to accelerator structures in the future
      // These do not own their memory. They are shortcut lists
      TeamsList _collisionElements;
      ElementList _animatedElements;

      // Cameras for rendering
      Camera* _theCamera;
      Camera* _theHUD;

    protected:
      // Returns the default music track
      int defaultMusic() { return _defaultMusic; }

      // Function for derived classes to implement when the pause signal is received
      virtual void onPause() {}

      // Function for derived classes to implement when the scene starts
      virtual void onStart();

      // Function for derived classes to implement when the pause is over
      virtual void onResume() {}

      // Function for derived classes to implement when the quit signal is received (auto saving?)
      virtual void onStop() {}



      // Derived class rendering function
      virtual void _render() = 0;

      // Update the time-dependent scene elements with the No. ticks
      virtual void _update( Uint32 ) = 0;

      // Derived class collisions function
      virtual void _resolveCollisions() = 0;


      // Return a reference to the dialog window container
      NamedVector<Dialog, true>& dialogWindows() { return _dialogWindows; }


      // Return the pointer to the background object
      Drawable* getBackground() { return _background; }

    public:
      // Set the input mapping name
      Scene( std::string );

      // Remove all the cached data and clean up
      virtual ~Scene();



      // Start the scene
      void start() { this->onStart(); }

      // Stop the scene
      void stop() { this->onStop(); }

      // Pause logic
      void pause() { _paused = true; this->onPause(); }
      void resume() { _paused = false; this->onResume(); }
      bool isPaused() const { return _paused; }


      // Update the time-dependent scene elements with the No. ticks
      void update( Uint32 );

      // Render all the scene elements
      void render();

      // Resolve all the collision
      void resolveCollisions();


      // Return the camera for the scene
      virtual Camera* getCamera() { return _theCamera; }

      // Return the camera for the HUD
      virtual Camera* getHUD() { return _theHUD; }


      // Return a pointer to a specific dialog window
      Dialog* getDialog( unsigned int i ) { return _dialogWindows[i]; }


      //////////////////////////////////////////////////
      // Spawning functions to fill the environment/hud

      // Spawn an object with the supplied ID number at the supplied position in the scene
      void spawn( std::string, Vector );

      // Spawn an object with the supplied ID number at the supplied position in the scene
      void spawn( unsigned int, Vector );

      // Spawn an object with the supplied ID number at the supplied position in the HUD
      void spawnHUD( std::string, Vector );

      // Spawn an object with the supplied ID number at the supplied position in the HUD
      void spawnHUD( unsigned int, Vector );

      // Add an object to the caches that is owned by another class.
      void addSpawned( Drawable* );


      //////////////////////////////////////////////////
      // Code to enable controllable interface

      // Register game-wide events with the manager
      virtual void registerEvents( InputManager* );

      // Register context-wide actions with the handler
      virtual void registerActions( InputHandler* );


      // Interfaces for input
      // Handled and mapped actions
      virtual void booleanAction( const InputAction&, bool );
//      virtual void floatAction( const InputAction&, float ) {}
//      virtual void vectorAction( const InputAction&, const Vector& ) {}
//      virtual void mouseAction( const InputAction&, bool, const Vector& ) {}

      // Regolith events
      virtual void eventAction( const RegolithEvent&, const SDL_Event& );

  };

}


#endif // REGOLITH_SCENE_H_
