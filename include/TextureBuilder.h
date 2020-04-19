
#ifndef __REGOLITH__TEXTURE_BUILDER_H__
#define __REGOLITH__TEXTURE_BUILDER_H__

#include "Texture.h"
#include "TextureFactory.h"

#include <SDL2/SDL.h>

#include <string>
#include <json/json.h>


namespace Regolith
{

  // Forward declarations
  class Scene;


  // Typedefs
  typedef std::map< std::string, TextureFactory_base* > FactoryMap;



  class TextureBuilder
  {
    private:
      FactoryMap _factories;
      SDL_Renderer* _theRenderer;
      const Scene* _theScene;

    protected:

    public:

      TextureBuilder();

      explicit TextureBuilder( SDL_Renderer* );

      virtual ~TextureBuilder();

      // Configure for the current scene being loaded
      void setRenderer( SDL_Renderer* );
      void setScene( const Scene* );

      SDL_Renderer* getRenderer() const { return this->_theRenderer; }

      const Scene* getScene() const { return _theScene; }

      // Add a new factory to the build system - takes ownership of data
      void addFactory( TextureFactory_base* );

      // Only needs a json object containing all the relevant info
      Drawable* build( Json::Value );
  };

}

#endif // __REGOLITH__TEXTURE_BUILDER_H__

