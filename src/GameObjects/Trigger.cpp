
#include "Regolith/GameObjects/Trigger.h"


namespace Regolith
{

  Trigger::Trigger() 
  {
  }

  Trigger::~Trigger()
  {
  }


  void Trigger::configure( Json::Value& json_data )
  {
    Collidable::configure( json_data );
    Interactable::configure( json_data );
  }


  PhysicalObject* Trigger::clone( const Vector& pos ) const
  {
    PhysicalObject* temp = new Trigger( *this );
    temp->setPosition( pos );
    return temp;
  }
  

}

