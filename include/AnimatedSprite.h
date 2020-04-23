
#ifndef __REGOLITH__ANIMATED_SPRITE_H__
#define __REGOLITH__ANIMATED_SPRITE_H__

#include "Definitions.h"

#include "Sprite.h"


namespace Regolith
{
  class Camera;

  class AnimatedSprite : public Sprite
  {
    private:
      Uint32 _updateRate;
      Uint32 _count;

    protected:

    public:
      AnimatedSprite();

      // Sprite sheet and update period
      AnimatedSprite( SpriteSheet, Uint32 );

      virtual ~AnimatedSprite();

      virtual int getProperties() const { return OBJECT_ANIMATED; }

      void setUpdateRate( Uint32 );

      // Update with time in ms
      virtual void update( Uint32 );


      // Return a clone of the current object. Transfers ownership of memory
      Drawable* clone() const;
  };

}

#endif // __REGOLITH__ANIMATED_SPRITE_H__

