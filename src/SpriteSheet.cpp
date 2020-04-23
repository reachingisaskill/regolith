
#include "SpriteSheet.h"

namespace Regolith
{

  SpriteSheet::SpriteSheet() :
    Texture(),
    _currentSprite( 0 ),
    _rows( 0 ),
    _columns( 0 ),
    _numSprites( 0 ),
    _spriteWidth( 0 ),
    _spriteHeight( 0 )
  {
  }


  SpriteSheet::SpriteSheet( RawTexture tex ) :
    Texture( tex ),
    _currentSprite( 0 ),
    _rows( 0 ),
    _columns( 0 ),
    _numSprites( 0 ),
    _spriteWidth( 0 ),
    _spriteHeight( 0 )
  {
  }


  SpriteSheet::~SpriteSheet()
  {
  }


  int SpriteSheet::getNumberSprites() const
  {
    return _numSprites;
  }


  void SpriteSheet::setSpriteNumber( int num )
  {
    _currentSprite = num;
    int sprite_x = (_currentSprite % _columns) * _spriteWidth;
    int sprite_y = (_currentSprite / _columns) * _spriteHeight;

    SDL_Rect clip;
    clip.x = sprite_x;
    clip.y = sprite_y;
    clip.w = _spriteWidth;
    clip.h = _spriteHeight;

    this->setClip( clip );
  }


  void SpriteSheet::configure( int rows, int cols, int number )
  {
    _rows = rows;
    _columns = cols;

    if ( number == 0 )
    {
      _numSprites = _rows * _columns;
    }
    else
    {
      _numSprites = number;
    }

    _spriteWidth = Texture::getWidth() / _columns;
    _spriteHeight = Texture::getHeight() / _rows;

    this->setSpriteNumber( 0 );
  }

}

