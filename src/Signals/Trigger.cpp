
#include "Signals/Trigger.h"
#include "Components/Exception.h"
#include "Components/Collision.h"

#include "logtastic.h"


namespace Regolith
{

  Trigger::Trigger( unsigned int size, ContextEvent event ) :
    Drawable(),
    _collision( new Collision*[size] ),
    _eventType( event ),
    _size( size ),
    _current( 0 )
  {
  }


  Trigger::Trigger( const Trigger& trigger ) :
    Drawable( trigger ),
    _collision( new Collision*[trigger._size] ),
    _eventType( trigger._eventType ),
    _size( trigger._size ),
    _current( trigger._current )
  {
    if ( _current != _size )
    {
      FAILURE_STREAM << "All collision objects have not been specified. Expected: " << _size << " received: " << _current;
      Exception ex( "Trigger::Trigger()", "All collision objects have not been specified." );
      ex.addDetail( "Current No.", _current );
      ex.addDetail( "Size", _size );
      throw ex;
    }
    for ( unsigned int i = 0; i < _size; ++i )
    {
      _collision[i] = trigger._collision[i]->clone();
    }
  }


  Trigger::~Trigger()
  {
    // Delete the individual objects
    for ( unsigned int i = 0; i < _size; ++i )
    {
      delete _collision[i];
    }
    // Delete the array pointers
    delete[] _collision;
  }


  unsigned int Trigger::getCollision( Collision*& col )
  {
    col = _collision[0];
    return _size;
  }


  void Trigger::addCollision( Collision* col )
  {
    if ( _current >= _size )
    {
      ERROR_STREAM << "Adding too many collision objects to a trigger: " << _current << " >= " << _size;
      return;
    }
    _collision[_current++] = col;
  }


  Drawable* Trigger::clone() const
  {
    return new Trigger( *this );
  }


}

