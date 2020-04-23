
#ifndef __REGOLITH__TEXTURE_FACTORY_H__
#define __REGOLITH__TEXTURE_FACTORY_H__

#include "Texture.h"

#include <json/json.h>


namespace Regolith
{

  // Forward declarations
  class ObjectBuilder;

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Base class definition
  class ObjectFactory_base
  {
    friend class ObjectBuilder;

    private:
      ObjectBuilder* _parentBuilder;

    protected :
      RawTexture findRawTexture( std::string ) const;
      SDL_Renderer* getRenderer() const;

      // Helper function to build the Drawable base class
      void buildDrawable( Drawable*, Json::Value& ) const;

      Collision* buildCollision( Json::Value& ) const;

    public:
      ObjectFactory_base();

      virtual ~ObjectFactory_base();

      virtual const char* getObjectName() const = 0;

      virtual Drawable* build( Json::Value& ) const = 0;
  };

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Provided classes

//  // Standard Texture
//  class SimpleFactory : public ObjectFactory_base
//  {
//    private:
//
//    public:
//      SimpleFactory();
//
//      virtual ~SimpleFactory();
//
//      virtual const char* getObjectName() const { return "simple_texture"; }
//
//      virtual Drawable* build( Json::Value& ) const;
//  };


  // Sprites
  class SpriteFactory : public ObjectFactory_base
  {
    private:

    public:
      SpriteFactory();

      virtual ~SpriteFactory();

      virtual const char* getObjectName() const { return "sprite"; }

      virtual Drawable* build( Json::Value& ) const;
  };


  // Animated Sprites
  class AnimatedFactory : public ObjectFactory_base
  {
    private:

    public:
      AnimatedFactory();

      virtual ~AnimatedFactory();

      virtual const char* getObjectName() const { return "animated"; }

      virtual Drawable* build( Json::Value& ) const;
  };


  // Updating FPS string
  class FPSStringFactory : public ObjectFactory_base
  {
    private:

    public:
      FPSStringFactory();

      virtual ~FPSStringFactory();

      virtual const char* getObjectName() const { return "fps_string"; }

      virtual Drawable* build( Json::Value& ) const;
  };

}


#endif // __REGOLITH__TEXTURE_FACTORY_H__

