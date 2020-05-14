
#ifndef REGOLITH_GAMEOBJECTS_ANIMATED_SPRITE_H_
#define REGOLITH_GAMEOBJECTS_ANIMATED_SPRITE_H_

#include "Regolith/Global/Global.h"
#include "Regolith/GamePlay/Texture.h"
#include "Regolith/Architecture/Drawable.h"
#include "Regolith/Architecture/Animated.h"


namespace Regolith
{
  class Camera;

  class AnimatedSprite : public Drawable, public Animated
  {
    private:
      Texture _texture;
      SDL_Rect _destination;

    protected:

    public:
      AnimatedSprite();

      virtual ~AnimatedSprite();


      // Configure the object
      virtual void configure( Json::Value& ) override;


      // Render with the current renderer object
      virtual void render( const Camera& ) override;


      // Render with the current renderer object
      virtual void update( float ) override;


      // Return a new clone of the current object. Transfers ownership of memory
      virtual PhysicalObject* clone( const Vector& ) const override;


      // AnimatedSprite details
      Texture& texture() { return _texture; }
  };

}

#endif // REGOLITH_GAMEOBJECTS_ANIMATED_SPRITE_H_

