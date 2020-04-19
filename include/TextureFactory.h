
#ifndef __REGOLITH__TEXTURE_FACTORY_H__
#define __REGOLITH__TEXTURE_FACTORY_H__

#include "Texture.h"

#include <json/json.h>


namespace Regolith
{

  // Forward declarations
  class TextureBuilder;

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Base class definition
  class TextureFactory_base
  {
    friend class TextureBuilder;

    private:
      TextureBuilder* _parentBuilder;

    protected :
      RawTexture findRawTexture( std::string ) const;
      SDL_Renderer* getRenderer() const;

    public:
      TextureFactory_base();

      virtual ~TextureFactory_base();

      virtual const char* getTextureName() const = 0;

      virtual Drawable* build( Json::Value& ) const = 0;
  };

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Provided classes

  // Standard Texture
  class SimpleFactory : public TextureFactory_base
  {
    private:

    public:
      SimpleFactory();

      virtual ~SimpleFactory();

      virtual const char* getTextureName() const { return "simple_texture"; }

      virtual Drawable* build( Json::Value& ) const;
  };


  // SpriteSheets
  class SpriteSheetFactory : public TextureFactory_base
  {
    private:

    public:
      SpriteSheetFactory();

      virtual ~SpriteSheetFactory();

      virtual const char* getTextureName() const { return "spritesheet"; }

      virtual Drawable* build( Json::Value& ) const;
  };


  // Animated SpriteSheets
  class AnimatedFactory : public TextureFactory_base
  {
    private:

    public:
      AnimatedFactory();

      virtual ~AnimatedFactory();

      virtual const char* getTextureName() const { return "animated"; }

      virtual Drawable* build( Json::Value& ) const;
  };


  // Updating FPS string
  class FPSStringFactory : public TextureFactory_base
  {
    private:

    public:
      FPSStringFactory();

      virtual ~FPSStringFactory();

      virtual const char* getTextureName() const { return "fps_string"; }

      virtual Drawable* build( Json::Value& ) const;
  };

}


#endif // __REGOLITH__TEXTURE_FACTORY_H__

