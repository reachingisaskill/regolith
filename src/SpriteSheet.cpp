
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
    _spriteHeight( 0 ),
    _spriteRect()
  {
  }


  SpriteSheet::SpriteSheet( RawTexture tex ) :
    Texture( tex ),
    _currentSprite( 0 ),
    _rows( 0 ),
    _columns( 0 ),
    _numSprites( 0 ),
    _spriteWidth( 0 ),
    _spriteHeight( 0 ),
    _spriteRect()
  {
  }


  // Move Constructor
  SpriteSheet::SpriteSheet( SpriteSheet&& sp ) :
    Texture( std::move( sp ) ),
    _currentSprite( std::move( sp._currentSprite ) ),
    _rows( std::move( sp._rows ) ),
    _columns( std::move( sp._columns ) ),
    _numSprites( std::move( sp._numSprites ) ),
    _spriteWidth( std::move( sp._spriteWidth ) ),
    _spriteHeight( std::move( sp._spriteHeight ) ),
    _spriteRect( std::move( sp._spriteRect ) )
  {
  }


  // Move Assignement
  SpriteSheet& SpriteSheet::operator=( SpriteSheet&& sp )
  {
    Texture::operator=( std::move( sp ) );

    _currentSprite = std::move( sp._currentSprite );
    _rows = std::move( sp._rows );
    _columns = std::move( sp._columns );
    _numSprites = std::move( sp._numSprites );
    _spriteWidth = std::move( sp._spriteWidth );
    _spriteHeight = std::move( sp._spriteHeight );
    _spriteRect = std::move( sp._spriteRect );

    return *this;
  }


  SpriteSheet::~SpriteSheet()
  {
  }


  void SpriteSheet::render( Camera* camera )
  {
    int sprite_x = (_currentSprite % _columns) * _spriteWidth;
    int sprite_y = (_currentSprite / _columns) * _spriteHeight;

    _spriteRect.x = sprite_x;
    _spriteRect.y = sprite_y;
//    _spriteRect.w = _spriteWidth;
//    _spriteRect.h = _spriteHeight;

    this->setClip( _spriteRect );
    Texture::render( camera );
  }


  int SpriteSheet::getNumberSprites() const
  {
    return _numSprites;
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

    _spriteRect.w = _spriteWidth;
    _spriteRect.h = _spriteHeight;
  }

}

