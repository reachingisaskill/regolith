#define __DEBUG_OFF__

#include "AnimatedSprite.h"

#include "logtastic.h"

namespace Regolith
{

  AnimatedSprite::AnimatedSprite() :
    SpriteSheet(),
    _updateRate( 0 ),
    _count( 0 )
  {
  }


  AnimatedSprite::AnimatedSprite( RawTexture tex ) :
    SpriteSheet( tex ),
    _updateRate( 0 ),
    _count( 0 )
  {
  }


//  // Move Constructor
//  AnimatedSprite::AnimatedSprite( AnimatedSprite&& ap ) :
//    SpriteSheet( std::move( ap ) ),
//    _updateRate( std::move( ap._updateRate ) ),
//    _count( std::move( ap._count ) )
//  {
//  }
//
//
//  // Move Assignement
//  AnimatedSprite& AnimatedSprite::operator=( AnimatedSprite&& ap )
//  {
//    SpriteSheet::operator=( std::move( ap ) );
//
//    _updateRate = std::move( ap._updateRate );
//    _count = std::move( ap._count );
//
//    return *this;
//  }


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
    int frame_number = this->currentSpriteNumber();
    frame_number += _count / _updateRate;
    frame_number = frame_number % this->getNumberSprites();
    this->setSpriteNumber( frame_number );
    _count = _count % _updateRate;

    DEBUG_STREAM << "AnimatedSprite : _count : " << _count << ", frame No. : " << frame_number << ", update rate : " << _updateRate;
  }


  Drawable* AnimatedSprite::clone() const
  {
    return (Drawable*) new AnimatedSprite( *this );
  }

}

