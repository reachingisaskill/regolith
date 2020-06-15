
#include "Regolith/GameObjects/Region.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  Region::Region()
  {
  }


  Region::~Region()
  {
  }


  void Region::configure( Json::Value& json_data, ContextGroup& cg, DataHandler& handler )
  {
    Collidable::configure( json_data, cg, handler );
  }


  PhysicalObject* Region::clone( const Vector& pos ) const
  {
    PhysicalObject* temp = new Region( *this );
    temp->setPosition( pos );
    return temp;
  }
}

