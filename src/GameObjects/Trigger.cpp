
#include "Regolith/GameObjects/Trigger.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  Trigger::Trigger() :
    _width( 0.0 ),
    _height( 0.0 )
  {
  }

  Trigger::~Trigger()
  {
  }


  void Trigger::configure( Json::Value& json_data, ContextGroup& cg, DataHandler& handler )
  {
    Collidable::configure( json_data, cg, handler );
    Interactable::configure( json_data, cg, handler );


    Utilities::validateJson( json_data, "width", Utilities::JSON_TYPE_FLOAT );
    Utilities::validateJson( json_data, "height", Utilities::JSON_TYPE_FLOAT );

    _width = json_data["width"].asFloat();
    _height = json_data["height"].asFloat();
  }


  PhysicalObject* Trigger::clone( const Vector& pos ) const
  {
    PhysicalObject* temp = new Trigger( *this );
    temp->setPosition( pos );
    return temp;
  }
  

}

