#define __DEBUG_OFF__

#include "AnimatedSprite.h"

#include "logtastic.h"

namespace Regolith
{

  AnimatedSprite::AnimatedSprite() :
    Sprite(),
    _updateRate( 0 ),
    _count( 0 )
  {
  }


  AnimatedSprite::AnimatedSprite( SpriteSheet sheet, Uint32 rate ) :
    Sprite( sheet ),
    _updateRate( rate ),
    _count( 0 )
  {
  }


  AnimatedSprite::~AnimatedSprite()
  {
  }


  void AnimatedSprite::setUpdateRate( Uint32 rate )
  {
    _updateRate = rate;
  }


  void AnimatedSprite::update( Uint32 timestep )
  {
    _count += timestep;
    int frame_number = this->spriteSheet().currentSpriteNumber();
    frame_number += _count / _updateRate;
    frame_number = frame_number % this->spriteSheet().getNumberSprites();
    this->spriteSheet().setSpriteNumber( frame_number );
    _count = _count % _updateRate;

    DEBUG_STREAM << "AnimatedSprite : _count : " << _count << ", frame No. : " << frame_number << ", update rate : " << _updateRate;
  }


  Drawable* AnimatedSprite::clone() const
  {
    return (Drawable*) new AnimatedSprite( *this );
  }

}

