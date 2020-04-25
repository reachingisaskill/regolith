
#include "TestDrawable.h"

#include "InputHandler.h"

#include "logtastic.h"


namespace Regolith
{

  TestDrawable::TestDrawable() :
    _collisionCount( 2 ),
    _collision( { Collision(), Collision() } ),
    _moveDistance( 1.0 ),
    _jump( false ),
    _attack( false )
  {
    _collision[0] = Collision( Vector( 0.0, 0.0 ), 1.0, 1.0 );
    _collision[1] = Collision( Vector( 2.0, 1.0 ), 0.5, 2.0 );
  }


  unsigned int TestDrawable::getCollision( Collision*& c )
  {
    c = _collision;
    return _collisionCount;
  }


  Drawable* TestDrawable::clone() const
  {
    return (Drawable*) new TestDrawable( *this );
  }


  void TestDrawable::registerEvents( InputHandler* handler )
  {
    handler->registerInputRequest( this, INPUT_ACTION_JUMP );
    handler->registerInputRequest( this, INPUT_ACTION_MOVE );
    handler->registerInputRequest( this, INPUT_ACTION_MOVE_UP );
    handler->registerInputRequest( this, INPUT_ACTION_MOVE_DOWN );
    handler->registerInputRequest( this, INPUT_ACTION_MOVE_LEFT );
    handler->registerInputRequest( this, INPUT_ACTION_MOVE_RIGHT );
    handler->registerInputRequest( this, INPUT_ACTION_MOVE_HORIZONTAL );
    handler->registerInputRequest( this, INPUT_ACTION_MOVE_VERTICAL );
    handler->registerInputRequest( this, INPUT_ACTION_ATTACK );
    handler->registerInputRequest( this, INPUT_ACTION_DEFEND );
  }


  void TestDrawable::booleanAction( const InputAction& action, bool value )
  {
    DEBUG_STREAM << "TestDrawable: Found action: " << action << ", value = " << value;
    switch ( action )
    {
      case INPUT_ACTION_JUMP :
        DEBUG_STREAM << "HERE " << value;
        _jump = value;
        break;
      case INPUT_ACTION_ATTACK :
        _jump = value;
        break;
      case INPUT_ACTION_MOVE_UP :
        this->velocity().y() = (value ? -_moveDistance : 0.0 );
        break;
      case INPUT_ACTION_MOVE_DOWN :
        this->velocity().y() = (value ? _moveDistance : 0.0 );
        break;
      case INPUT_ACTION_MOVE_LEFT :
        this->velocity().x() = (value ? -_moveDistance : 0.0 );
        break;
      case INPUT_ACTION_MOVE_RIGHT :
        this->velocity().y() = (value ? _moveDistance : 0.0 );
        break;
      default:
        break;
    }
  }


  void TestDrawable::floatAction( const InputAction& action, float value )
  {
    switch ( action )
    {
      case INPUT_ACTION_MOVE_HORIZONTAL :
        this->velocity().x() = value * _moveDistance;
        break;
      case INPUT_ACTION_MOVE_VERTICAL :
        this->velocity().y() = value * _moveDistance;
        break;
      default :
        break;
    }
  }


  void TestDrawable::vectorAction( const InputAction& action, const Vector& vector )
  {
    switch ( action )
    {
      case INPUT_ACTION_MOVE :
        this->move( vector );
        break;
      default:
        break;
    }
  }


}

