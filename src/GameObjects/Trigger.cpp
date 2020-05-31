
#include "Regolith/GameObjects/Trigger.h"
#include "Regolith/Managers/DataHandler.h"


namespace Regolith
{

  Trigger::Trigger() 
  {
  }

  Trigger::~Trigger()
  {
  }


  void Trigger::configure( Json::Value& json_data, DataHandler& handler )
  {
    Collidable::configure( json_data, handler );
    Interactable::configure( json_data, handler );
  }


  PhysicalObject* Trigger::clone( const Vector& pos ) const
  {
    PhysicalObject* temp = new Trigger( *this );
    temp->setPosition( pos );
    return temp;
  }
  

}

