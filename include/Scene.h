
#ifndef __REGOLITH__SCENE_H__
#define __REGOLITH__SCENE_H__

#include "Definitions.h"
#include "NamedVector.h"
#include "Context.h"
#include "Controllable.h"
#include "Drawable.h"
#include "ObjectBuilder.h"
#include "Window.h"
#include "Camera.h"

#include <vector>
#include <queue>
#include <map>


namespace Regolith
{

  typedef std::map< std::string, RawTexture > RawTextureMap;
  typedef std::vector< Drawable* > ResourceList;
  typedef std::map< std::string, unsigned int > ResourceNameMap;
  typedef std::map< std::string, unsigned int > TeamNameMap;

  typedef std::vector< Drawable* > ElementList;
  typedef std::vector< ElementList > TeamsList;


  class Scene : public Context, public Controllable
  {
    private:
      // Scene owns the memory for the texture data
      RawTextureMap _rawTextures;
      NamedVector<Drawable> _resources;
      TeamNameMap _teamNames;
      bool _paused;

      // Key components for building elements and rendering
      Window* _theWindow;
      SDL_Renderer* _theRenderer;
      ObjectBuilder* _theBuilder;
      std::string _sceneFile;

      // Containers storing drawable objects - the scene and the hud elements
      // All memory is owned by the resource list above
      Drawable* _background;
      ElementList _sceneElements;
      ElementList _hudElements;

      // Can be change to accelerator structures in the future
      // These do not own their memory. They are shortcut lists
      TeamsList _collisionElements;
      ElementList _animatedElements;
      ElementList _inputElements;

      // Cameras for rendering
      Camera* _theCamera;
      Camera* _theHUD;

    protected:

      // Function for derived classes to implement when the pause signal is received
      virtual void onPause() {}

      // Function for derived classes to implement when the pause is over
      virtual void onStart() {}

      // Function for derived classes to implement when the pause is over
      virtual void onResume() {}

      // Function for derived classes to implement when the quit signal is received (auto saving?)
      virtual void onQuit() {}


      // Helper functions to build and store the raw texture files
      void _addTextureFromFile( Json::Value& );
      void _addTextureFromText( Json::Value& );

      // Build the whole scene from a json file description
      void buildFromJson();

      // Function to parse the json document
      Json::Value _loadConfig();

      // Function to load the input configuration
      void _loadInput( Json::Value& );

      // Function to load all the raw texture files
      void _loadTextures( Json::Value& );

      // Function to build all the resources
      void _loadResources( Json::Value& );

      // Function to build all the caches/acceleration structures
      void _loadCaches( Json::Value& );

      // Function to build the camera objects
      void _loadCameras( Json::Value& );

      // Function to build the dialog boxes
      void _loadDialogs( Json::Value& );


      // Override this function to allow the base class to build components specific to this scene type
      virtual void _loadSceneSpecificComponents( Json::Value& ) = 0;


      // Derived class rendering function
      virtual void _render() = 0;

      // Update the time-dependent scene elements with the No. ticks
      virtual void _update( Uint32 ) = 0;

      // Derived class collisions function
      virtual void _resolveCollisions() = 0;


      // Return the pointer to the background object
      Drawable* getBackground() { return _background; }

    public:
      // Set the necessary parameters
      Scene( std::string );

      // Remove all the cached data and clean up
      virtual ~Scene();


      // Configure the info required to build the scene
      void configure( SDL_Renderer*, Window*, ObjectBuilder* );

      // Load the scene from the specified file
      void load();

      // Return a copy of the scene file
      std::string getSceneFile() { return _sceneFile; }

      // Return a pointer to the object builder
      ObjectBuilder* getBuilder() { return _theBuilder; }


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


      // Return a pointer to a raw texture object
      RawTexture findRawTexture( std::string ) const;

      // Look up the id number for a given resource
      unsigned int findResourceID( std::string name ) const { return _resources.getID( name ); }

      // Return a pointer to a specific resource
      Drawable* findResource( unsigned int n ) { return _resources[ n ]; }
      Drawable* findResource( std::string name ) { return _resources.getByName( name ); }


      // Spawn an object with the supplied ID number at the default position in the Scene
      void spawn( unsigned int );

      // Spawn an object with the supplied ID number at the supplied position in the scene
      void spawnAt( unsigned int, Vector );

      // Spawn an object with the supplied ID number at the supplied position in the HUD
      void spawnHUD( unsigned int, Vector );

      // Spawn an object with the supplied ID number at the supplied position, and return the pointer
      // THIS FUNCTION RETURNS OWNSHIP OF THIS MEMORY!
      Drawable* spawnReturn( unsigned int, Vector );


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


#endif // __REGOLITH__SCENE_H__

