#define LOGTASTIC_DEBUG_OFF

#include "GameObjects/Button.h"
#include "Managers/Manager.h"
#include "Components/Camera.h"
#include "Signals/Signal.h"

#include "logtastic.h"


namespace Regolith
{
  
  Button::Button( Texture tex, Collision* col ) :
    Drawable(),
    _textures( { tex, tex, tex, tex } ),
    _collision( col ),
    _destination( { 0, 0, tex.getWidth(), tex.getHeight() } ),
    _state( STATE_NORMAL ),
    _actions()
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
    _state( but._state ),
    _actions( but._actions )
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
    if ( _textures[ STATE_DOWN ].isAnimated() )
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
      case STATE_DOWN :
      case STATE_INACTIVE :
      default :
        break;
    }
    DEBUG_LOG ( "BUTTON FOCUSSED" );
  }


  void Button::takeFocus()
  {
    switch ( _state )
    {
      case STATE_FOCUSSED :
      case STATE_DOWN :
        _state = STATE_NORMAL;
        break;

      case STATE_NORMAL :
      case STATE_INACTIVE :
      default :
        break;
    }
    DEBUG_LOG ( "BUTTON DEFOCUSSED" );
  }


  void Button::down()
  {
    switch ( _state )
    {
      case STATE_FOCUSSED :
      case STATE_NORMAL :
        _state = STATE_DOWN;
        break;

      case STATE_DOWN :
      case STATE_INACTIVE :
      default :
        break;
    }
    DEBUG_LOG ( "BUTTON DOWN" );
  }


  void Button::up()
  {
    switch ( _state )
    {
      case STATE_FOCUSSED :
      case STATE_DOWN :
        _state = STATE_NORMAL;
        this->click();
        break;

      case STATE_NORMAL :
      case STATE_INACTIVE :
      default :
        break;
    }
    DEBUG_LOG ( "BUTTON UP" );
  }


  void Button::activate()
  {
    switch ( _state )
    {
      case STATE_NORMAL :
      case STATE_FOCUSSED :
      case STATE_DOWN :
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
      case STATE_DOWN :
        _state = STATE_INACTIVE;
        break;

      case STATE_INACTIVE :
      default :
        break;
    }
  }


  void Button::click()
  {
    std::vector< Signal* >::iterator end = _actions.end();
    for ( std::vector< Signal* >::iterator it = _actions.begin(); it != end; ++it )
    {
      (*it)->trigger();
    }
  }

}

