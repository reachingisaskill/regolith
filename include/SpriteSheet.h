
#ifndef __REGOLITH__SPRITE_SHEET_H__
#define __REGOLITH__SPRITE_SHEET_H__

#include "Texture.h"


namespace Regolith
{

  class SpriteSheet : public Texture
  {
    SpriteSheet( const SpriteSheet& ) = delete; // Non-copy-constructable
    SpriteSheet& operator=( const SpriteSheet& ) = delete; // Non-copy-assignable

    private:
      int _currentSprite;
      int _rows;
      int _columns;
      int _numSprites;
      int _spriteWidth;
      int _spriteHeight;
      SDL_Rect _spriteRect;

    protected:
      virtual void _free();

    public:
      SpriteSheet();

      // Move-construct
      SpriteSheet( SpriteSheet&& );

      // Move-assign
      SpriteSheet& operator=( SpriteSheet&& );

      virtual ~SpriteSheet();

      // Set the texture wrapper pointer, No. rows, No. Columns, and No. of used cells
      void configure( int, int, int number = 0 );

      // Render with the current renderer object
      virtual void render();

      // Sprite details
      int getNumberSprites() const;
      void setSpriteNumber( int s ) { _currentSprite = s; }

      int getWidth() const { return _spriteWidth; }
      int getHeight() const { return _spriteHeight; }
  };

}

#endif // __REGOLITH__SPRITE_SHEET_H__

