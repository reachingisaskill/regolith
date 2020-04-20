
#ifndef __REGOLITH__SCENE_H__
#define __REGOLITH__SCENE_H__

#include "Drawable.h"
#include "TextureBuilder.h"
#include "Window.h"
#include "Camera.h"

#include <SDL2/SDL.h>

#include <vector>
#include <map>


namespace Regolith
{

  typedef std::vector< Drawable* > TextureList;
  typedef std::map< std::string, RawTexture > RawTextureMap;


  class Scene
  {
    private:
      // Scene owns the memory for the texture data
      RawTextureMap _rawTextures;

      Window* _theWindow;
      SDL_Renderer* _theRenderer;
      TextureBuilder* _theBuilder;
      std::string _sceneFile;

      TextureList _scene_elements;
      TextureList _hud_elements;
      Drawable* _background;

      // Can be change to accelerator structures in the future
      // These do not own their memory. They are shortcut lists
      TextureList _collision_elements;
      TextureList _animated_elements;

      Camera _theCamera;
      Camera _theHUD;

    protected:
      void _addTextureFromFile( Json::Value& );
      void _addTextureFromText( Json::Value& );

      // Build the whole scene from a json file description
      void buildFromJson();

    public:
//      // No initialisation
//      Scene( Window*, SDL_Renderer* );

      // Set the necessary parameters
      Scene( Window*, SDL_Renderer*, TextureBuilder*, std::string );

      // Load the scene from the specified fiel
      void load();

      // Update the time-dependent scene elements with the No. ticks
      void update( Uint32 );

      // Render all the scene elements
      void render();

      // Handle events
      void handleEvent( SDL_Event* );

      // Return the camera for the scene
      Camera* getCamera() { return &_theCamera; }

      // Return the camera for the HUD
      Camera* getHUD() { return &_theHUD; }

      // Return a pointer to a raw texture object
      RawTexture findRawTexture( std::string ) const;
  };

}


#endif // __REGOLITH__SCENE_H__

