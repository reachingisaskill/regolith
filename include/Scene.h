
#ifndef __REGOLITH__SCENE_H__
#define __REGOLITH__SCENE_H__

#include "Texture.h"
#include "TextureBuilder.h"
#include "Window.h"
#include "Camera.h"

#include <SDL2/SDL.h>

#include <vector>
#include <map>


namespace Regolith
{

  typedef std::vector< Texture* > TextureList;
  typedef std::map< std::string, RawTexture > RawTextureMap;


  class Scene
  {
    private:
      // Scene owns the memory for the texture data
      RawTextureMap _rawTextures;

      Window* _theWindow;
      SDL_Renderer* _theRenderer;

      TextureBuilder _builder;
      TextureList _scene_elements;
      TextureList _hud_elements;
      Texture* _background;

      // Can be change to accelerator structures in the future
      // These do not own their memory. They are shortcut lists
      TextureList _collision_elements;
      TextureList _animated_elements;

      Camera _theCamera;
      Camera _theHUD;

    protected:
      void _addTextureFromFile( std::string, std::string );
      void _addTextureFromText( std::string, std::string, TTF_Font*, SDL_Color );

    public:
      // No initialisation
      Scene( Window*, SDL_Renderer* );

      // Load immediately from file
      Scene( Window*, SDL_Renderer*, std::string );

      // Build the whole scene from a json file description
      void buildFromJson( std::string );

      // Update the time-dependent scene elements with the No. ticks
      void update( Uint32 );

      // Render all the scene elements
      void render();

      // Return the camera for the scene
      Camera* getCamera() { return &_theCamera; }

      // Return the camera for the HUD
      Camera* getHUD() { return &_theHUD; }

  };

}


#endif // __REGOLITH__SCENE_H__

