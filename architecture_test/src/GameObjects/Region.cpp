
#include "Regolith/GameObjects/Region.h"

namespace Regolith
{

  Region::Region()
  {
  }


  Region::~Region()
  {
  }


  void Region::configure( Json::Value& json_data )
  {
    Collidable::configure( json_data );
  }


  PhysicalObject* Region::clone( const Vector& pos ) const
  {
    PhysicalObject* temp = new Region( *this );
    temp->setPosition( pos );
    return temp;
  }
}

