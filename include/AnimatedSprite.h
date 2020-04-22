
#ifndef __REGOLITH__ANIMATED_SPRITE_H__
#define __REGOLITH__ANIMATED_SPRITE_H__

#include "Definitions.h"

#include "SpriteSheet.h"


namespace Regolith
{
  class Camera;

  class AnimatedSprite : public SpriteSheet
  {
    private:
      Uint32 _updateRate;
      Uint32 _count;

    protected:

    public:
      AnimatedSprite();

      AnimatedSprite( RawTexture );

//      // Move-construct
//      AnimatedSprite( AnimatedSprite&& );
//
//      // Move-assign
//      AnimatedSprite& operator=( AnimatedSprite&& );

      virtual ~AnimatedSprite();

      virtual int getProperties() const { return OBJECT_ANIMATED; }

      void setUpdateRate( Uint32 );

      // Update with time in ms
      virtual void update( Uint32 );

      // Render with the current renderer object
      // Use the SpriteSheet variant
//      virtual void render( Camera* );

      // Return a clone of the current object. Transfers ownership of memory
      Drawable* clone() const;
  };

}

#endif // __REGOLITH__ANIMATED_SPRITE_H__

