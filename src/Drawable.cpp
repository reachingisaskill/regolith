
#include "Drawable.h"

#include <utility>


namespace Regolith
{

  Drawable::Drawable() :
    _theRenderer( nullptr ),
    _position( 0.0 ),
    _rotation( 0.0 ),
    _team( 0 ),
    _width( 0 ),
    _height( 0 )
  {
  }


  Drawable::Drawable( int width, int height, float rot ) :
    _theRenderer( nullptr ),
    _position( 0.0 ),
    _rotation( rot ),
    _team( 0 ),
    _width( width ),
    _height( height )
  {
  }


  Drawable::~Drawable()
  {
  }


  Drawable* Drawable::cloneAt( Vector pos ) const
  {
    Drawable* temp = this->clone();
    temp->setPosition( pos );
    return temp;
  }

}

