
#ifndef __REGOLITH__ANIMATED_SPRITE_H__
#define __REGOLITH__ANIMATED_SPRITE_H__

#include "Texture.h"

#include <SDL2/SDL.h>


namespace Regolith
{
  class Camera;

  class AnimatedSprite : public SpriteSheet
  {
    AnimatedSprite( const AnimatedSprite& ) = delete; // Non-copy-constructable
    AnimatedSprite& operator=( const AnimatedSprite& ) = delete; // Non-copy-assignable

    friend class TextureBuilder;

    private:
      Uint32 _updateRate;
      Uint32 _count;

    protected:
      AnimatedSprite( RawTexture );

    public:
      AnimatedSprite();

      // Move-construct
      AnimatedSprite( AnimatedSprite&& );

      // Move-assign
      AnimatedSprite& operator=( AnimatedSprite&& );

      virtual ~AnimatedSprite();

      virtual ObjectProperty getProperties() const { return OBJECT_ANIMATED; }

      void setUpdateRate( Uint32 );

      // Update with time in ms
      virtual void update( Uint32 );

      // Render with the current renderer object
      virtual void render( Camera* );
  };

}

#endif // __REGOLITH__ANIMATED_SPRITE_H__

