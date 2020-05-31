
#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  PhysicalObject::PhysicalObject() :
    _destroyMe( false ),
    _position(),
    _mass( 0.0 ),
    _inverseMass( 0.0 ),
    _width(),
    _height(),
    _rotation()
  {
  }


  void PhysicalObject::setMass( float m )
  {
    _mass = m;
    if ( _mass < epsilon )
      _inverseMass = 0.0;
    else
      _inverseMass = 1.0/_mass;
  }


  void PhysicalObject::configure( Json::Value& json_data, DataHandler& )
  {
    if ( Utilities::validateJson( json_data, "mass", Utilities::JSON_TYPE_FLOAT, false ) )
    {
      setMass( json_data["mass"].asFloat() );
    }

    if ( Utilities::validateJson( json_data, "position", Utilities::JSON_TYPE_FLOAT, false ) )
    {
      Utilities::validateJsonArray( json_data["position"], 2, Utilities::JSON_TYPE_FLOAT );

      float x =  json_data["position"][0].asFloat();
      float y =  json_data["position"][1].asFloat();
      _position = Vector( x, y );
    }

    if ( Utilities::validateJson( json_data, "width", Utilities::JSON_TYPE_FLOAT, false ) )
    {
      _width = json_data["width"].asFloat();
    }

    if ( Utilities::validateJson( json_data, "height", Utilities::JSON_TYPE_FLOAT, false ) )
    {
      _height = json_data["height"].asFloat();
    }
  }

}

