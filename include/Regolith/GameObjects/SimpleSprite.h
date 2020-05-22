
#ifndef REGOLITH_GAMEOBJECTS_SIMPLE_SPRITE_H_
#define REGOLITH_GAMEOBJECTS_SIMPLE_SPRITE_H_

#include "Regolith/Global/Global.h"
#include "Regolith/GamePlay/Texture.h"
#include "Regolith/GamePlay/Collision.h"
#include "Regolith/Architecture/Drawable.h"


namespace Regolith
{
  class Camera;

  class SimpleSprite : public Drawable
  {
    private:
      Texture _texture;
      SDL_Rect _destination;

    protected:

    public:
      SimpleSprite();

      virtual ~SimpleSprite();


      // Configure the object
      virtual void configure( Json::Value& ) override;


      // Render with the current renderer object
      virtual void render( const Camera& ) override;


      // Return a new clone of the current object. Transfers ownership of memory
      virtual PhysicalObject* clone( const Vector& ) const override;


      // SimpleSprite details
      Texture& texture() { return _texture; }
  };

}

#endif // REGOLITH_SPRITE_H_

