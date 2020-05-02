
#ifndef REGOLITH_SCENE_H_
#define REGOLITH_SCENE_H_

#include "Definitions.h"
#include "NamedVector.h"
#include "Context.h"
#include "Controllable.h"
#include "Drawable.h"
#include "ObjectBuilder.h"
#include "Dialog.h"
#include "Window.h"
#include "Camera.h"

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
    private:
      // Scene state information
      std::string _sceneFile;
      bool _paused;
      int _defaultMusic;

      // Containers storing drawable objects - the scene and the hud elements
      // All memory is owned by Scene
      Drawable* _background;
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
      virtual void onQuit() {}


      // Function to parse the json document
      Json::Value _loadConfig();

      // Function to load the input configuration
      void _loadInput( Json::Value& );

      // Function to load all the sound files
      void _loadSounds( Json::Value& );

      // Function to build all the caches/acceleration structures
      void _loadCaches( Json::Value& );

      // Function to build the camera objects
      void _loadCameras( Json::Value& );

      // Function to build all the HUD structures
      void _loadHUD( Json::Value& );

      // Function to build the dialog boxes
      void _loadDialogs( Json::Value& );

      // Function to build the remaining configuration - should be implemeted by the derived classes
      virtual void _loadOptions( Json::Value& );


      // Override this function to allow the base class to build components specific to this scene type
      virtual void _loadSceneSpecificComponents( Json::Value& ) = 0;


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
      // Set the necessary parameters
      Scene( std::string );

      // Remove all the cached data and clean up
      virtual ~Scene();


      // Start the scene
      void start() { this->onStart(); }


      // Load the scene from the specified file
      void load();


      // Return a copy of the scene file
      std::string getSceneFile() { return _sceneFile; }


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
      virtual void floatAction( const InputAction&, float ) {}
      virtual void vectorAction( const InputAction&, const Vector& ) {}

      // Regolith events
      virtual void eventAction( const RegolithEvent&, const SDL_Event& );

  };

}


#endif // REGOLITH_SCENE_H_

