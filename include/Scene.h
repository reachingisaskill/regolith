
#ifndef __REGOLITH__SCENE_H__
#define __REGOLITH__SCENE_H__

#include "Texture.h"
#include "TextureBuilder.h"
#include "Window.h"

#include <vector>


namespace Regolith
{

  typedef std::vector< Texture* > TextureList;


  class Scene
  {
    private:
      Window* _theWindow;
      TextureBuilder _builder;
      TextureList _scene_elements;
      TextureList _hud_elements;
      Texture* _background;

    public:
      // No initialisation
      explicit Scene( Window*, SDL_Renderer* );

      // Load immediately from file
      Scene( Window*, SDL_Renderer*, std::string );

      // Build the whole scene from a json file description
      void buildFromJson( std::string );

      // Update the time-dependent scene elements with the No. ticks
      void update( Uint32 );

      // Render all the scene elements
      void render();

  };

}


#endif // __REGOLITH__SCENE_H__

