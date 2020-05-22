
#ifndef REGOLITH_GAMEOBJECTS_COLLIDABLE_SPRITE_H_
#define REGOLITH_GAMEOBJECTS_COLLIDABLE_SPRITE_H_

#include "Regolith/Global/Global.h"
#include "Regolith/GamePlay/Texture.h"
#include "Regolith/GamePlay/Collision.h"
#include "Regolith/Architecture/Drawable.h"
#include "Regolith/Architecture/Collidable.h"


namespace Regolith
{
  class Camera;

  class CollidableSprite : public Drawable, public Collidable
  {
    private:
      Texture _texture;
      SDL_Rect _destination;

    protected:

    public:
      CollidableSprite();

      virtual ~CollidableSprite();


      // Configure the object
      virtual void configure( Json::Value& ) override;


      // Render with the current renderer object
      virtual void render( const Camera& ) override;


      // Return a new clone of the current object. Transfers ownership of memory
      virtual PhysicalObject* clone( const Vector& ) const override;


      // Sprite details
      Texture& texture() { return _texture; }


      // Function called when a collision is found with another object
      // Overlap vector, other object pointer
      virtual void onCollision( const Vector&, float, const Collidable* ) override;
  };

}

#endif // REGOLITH_GAMEOBJECTS_COLLIDABLE_SPRITE_H_

