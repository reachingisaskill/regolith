
#ifndef REGOLITH_TEXTURE_FACTORY_H_
#define REGOLITH_TEXTURE_FACTORY_H_

#include "Components/Texture.h"

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

      // Build a new texture object from the json data
      Texture buildTexture( Json::Value& ) const;

      Collision* buildCollision( Json::Value& ) const;

    public:
      ObjectFactory_base();

      virtual ~ObjectFactory_base();

      virtual const char* getObjectName() const = 0;

      virtual Drawable* build( Json::Value& ) const = 0;
  };

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Provided classes

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


  // Buttons
  class ButtonFactory : public ObjectFactory_base
  {
    private:

    public:
      ButtonFactory();

      virtual ~ButtonFactory();

      virtual const char* getObjectName() const { return "button"; }

      virtual Drawable* build( Json::Value& ) const;
  };

}


#endif // REGOLITH_TEXTURE_FACTORY_H_
