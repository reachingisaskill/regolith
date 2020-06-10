
#include "Regolith/GameObjects/Region.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  Region::Region() :
    _width( 0.0 ),
    _height( 0.0 )
  {
  }


  Region::~Region()
  {
  }


  void Region::configure( Json::Value& json_data, DataHandler& handler )
  {
    Collidable::configure( json_data, handler );

    Utilities::validateJson( json_data, "width", Utilities::JSON_TYPE_FLOAT );
    Utilities::validateJson( json_data, "height", Utilities::JSON_TYPE_FLOAT );

    _width = json_data["width"].asFloat();
    _height = json_data["height"].asFloat();
  }


  PhysicalObject* Region::clone( const Vector& pos ) const
  {
    PhysicalObject* temp = new Region( *this );
    temp->setPosition( pos );
    return temp;
  }
}

