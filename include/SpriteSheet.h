
#ifndef __REGOLITH__SPRITE_SHEET_H__
#define __REGOLITH__SPRITE_SHEET_H__

#include "Texture.h"

#include <SDL2/SDL.h>


namespace Regolith
{
  class Camera;

  class SpriteSheet : public Texture
  {
    private:
      int _currentSprite;
      int _rows;
      int _columns;
      int _numSprites;
      int _spriteWidth;
      int _spriteHeight;

    protected:

    public:
      SpriteSheet();

      explicit SpriteSheet( RawTexture );

      virtual ~SpriteSheet();

      // Set the texture wrapper pointer, No. rows, No. Columns, and No. of used cells
      void configure( int, int, int number = 0 );


      // Sprite details
      int getNumberSprites() const;
      void setSpriteNumber( int );
      const int& currentSpriteNumber() { return _currentSprite; }


      int getWidth() const { return _spriteWidth; }
      int getHeight() const { return _spriteHeight; }
  };

}

#endif // __REGOLITH__SPRITE_SHEET_H__

