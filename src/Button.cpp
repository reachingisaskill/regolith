
#include "Button.h"

#include "Camera.h"

#include "logtastic.h"


namespace Regolith
{
  
  Button::Button( Texture tex, Collision* col ) :
    Drawable(),
    _textures( { tex, tex, tex, tex } ),
    _collision( col ),
    _destination( { 0, 0, tex.getWidth(), tex.getHeight() } ),
    _state( STATE_NORMAL )
  {
    if ( _collision == nullptr )
    {
      WARN_LOG( "Attempting to create a button with no collision. This button can never be pressed!" );
    }
  }


  Button::Button( const Button& but ) :
    Drawable( but ),
    _textures( but._textures ),
    _collision( (but._collision == nullptr) ? nullptr : but._collision->clone() ),
    _destination( but._destination ),
    _state( but._state )
  {
    if ( _collision == nullptr )
    {
      WARN_LOG( "Attempting to copy a button with no collision. This button can never be pressed!" );
    }
  }


  Button::~Button()
  {
    if ( _collision != nullptr )
    {
      delete _collision;
      _collision = nullptr;
    }
  }


  Drawable* Button::clone() const
  {
    return new Button( *this );
  }


  bool Button::hasAnimation() const
  {
    if ( _textures[ STATE_NORMAL ].isAnimated() )
      return true;
    if ( _textures[ STATE_FOCUSSED ].isAnimated() )
      return true;
    if ( _textures[ STATE_ACTIVATED ].isAnimated() )
      return true;
    if ( _textures[ STATE_INACTIVE ].isAnimated() )
      return true;

    return false;
  }


  void Button::setTexture( Texture tex, State state )
  {
    _textures[ state ] = tex;
  }


  void Button::update( Uint32 time )
  {
    if ( _textures[ _state ].isAnimated() )
      _textures[ _state ].update( time );

    this->step( time );
  }


  void Button::render( Camera* camera )
  {
    // Set the current sprite position
    _destination.x = position().x();
    _destination.y = position().y();
    
    // Move into the camera reference frame
    SDL_Rect destination = camera->place( _destination );
    DEBUG_STREAM << "BUTTON RENDER: Original: " << _destination.w << ", " << _destination.h << ", NEW: " << destination.w << ", " << destination.h;

    _textures[_state].draw( &destination );
  }


  unsigned int Button::getCollision( Collision*& collision )
  {
    collision = _collision;
    return 1;
  }


  void Button::giveFocus()
  {
    switch ( _state )
    {
      case STATE_NORMAL :
        _state = STATE_FOCUSSED;
        break;

      case STATE_FOCUSSED :
      case STATE_ACTIVATED :
      case STATE_INACTIVE :
      default :
        break;
    }
  }


  void Button::takeFocus()
  {
    switch ( _state )
    {
      case STATE_FOCUSSED :
      case STATE_ACTIVATED :
        _state = STATE_NORMAL;
        break;

      case STATE_NORMAL :
      case STATE_INACTIVE :
      default :
        break;
    }
  }


  bool Button::tryClick() const
  {
    switch ( _state )
    {
      case STATE_NORMAL :
      case STATE_FOCUSSED :
      case STATE_ACTIVATED :
        return true;
        break;

      case STATE_INACTIVE :
      default :
        return false;
        break;
    }
  }


  void Button::activate()
  {
    switch ( _state )
    {
      case STATE_NORMAL :
      case STATE_FOCUSSED :
      case STATE_ACTIVATED :
        break;

      case STATE_INACTIVE :
        _state = STATE_NORMAL;
        break;

      default :
        break;
    }
  }


  void Button::deactivate()
  {
    switch ( _state )
    {
      case STATE_NORMAL :
      case STATE_FOCUSSED :
      case STATE_ACTIVATED :
        _state = STATE_INACTIVE;
        break;

      case STATE_INACTIVE :
      default :
        break;
    }
  }

}

