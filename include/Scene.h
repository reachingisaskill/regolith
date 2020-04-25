
#ifndef __REGOLITH__SCENE_H__
#define __REGOLITH__SCENE_H__

#include "Drawable.h"
#include "ObjectBuilder.h"
#include "Window.h"
#include "Camera.h"

#include <SDL2/SDL.h>

#include <vector>
#include <map>


namespace Regolith
{

  typedef std::map< std::string, RawTexture > RawTextureMap;
  typedef std::vector< Drawable* > ResourceList;
  typedef std::map< std::string, unsigned int > ResourceNameMap;
  typedef std::map< std::string, unsigned int > TeamNameMap;

  typedef std::vector< Drawable* > ElementList;
  typedef std::vector< ElementList > TeamsList;


  class Scene
  {
    private:
      // Scene owns the memory for the texture data
      RawTextureMap _rawTextures;
      ResourceList _resources;
      ResourceNameMap _resourceNames;
      TeamNameMap _teamNames;

      // Key components for building elements and rendering
      Window* _theWindow;
      SDL_Renderer* _theRenderer;
      ObjectBuilder* _theBuilder;
      std::string _sceneFile;
      InputHandler* _theInput;

      // Containers storing drawable objects - the scene and the hud elements
      // All memory is owned by the resource list above
      Drawable* _background;
      Drawable* _player;
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

    public:
      // Set the necessary parameters
      Scene( Window*, SDL_Renderer*, ObjectBuilder*, std::string );

      // Remove all the cached data and clean up
      ~Scene();

      // Load the scene from the specified fiel
      void load();

      // Update the time-dependent scene elements with the No. ticks
      void update( Uint32 );

      // Render all the scene elements
      void render();

      // Handle events
      void processEvents();

      // Resolve all the collision
      void resolveCollisions();

      // Return the camera for the scene
      Camera* getCamera() { return _theCamera; }

      // Return the camera for the HUD
      Camera* getHUD() { return _theHUD; }

      // Return a pointer to a raw texture object
      RawTexture findRawTexture( std::string ) const;

      // Spawn an object with the supplied ID number at the default position in the Scene
      void spawn( unsigned int );

      // Spawn an object with the supplied ID number at the supplied position in the scene
      void spawnAt( unsigned int, Vector );

      // Spawn an object with the supplied ID number at the supplied position in the HUD
      void spawnHUD( unsigned int, Vector );

  };

}


#endif // __REGOLITH__SCENE_H__

