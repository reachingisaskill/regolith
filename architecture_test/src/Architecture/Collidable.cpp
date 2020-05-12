
#include "Regolith/Architecture/Collidable.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  Collidable::Collidable() :
    PhysicalObject(),
    _collision(),
    _collisionActive( false ),
    _collisionType( 0 ),
    _teamID( 0 )
  {
  }


  void Collidable::configure( Json::Value& json_data )
  {
    Utilities::validateJson( json_data, "collision", Utilities::JSON_TYPE_OBJECT );
    Utilities::validateJson( json_data, "collision_type", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "collision_team", Utilities::JSON_TYPE_STRING );

    Json::Value& collision_data = json_data["collision"];
    std::string collision_type = json_data["collision_type"].asString();
    std::string collision_team = json_data["collision_team"].asString();

    INFO_STREAM << "Configuring collidable object with type: " << collision_type << " and team: " << collision_team;

    Utilities::validateJson( collision_data, "position", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJson( collision_data, "width", Utilities::JSON_TYPE_FLOAT );
    Utilities::validateJson( collision_data, "height", Utilities::JSON_TYPE_FLOAT );

    float col_x = collision_data["position"][0].asFloat();
    float col_y = collision_data["position"][1].asFloat();

    _collision.configure( Vector( col_x, col_y ), collision_data["width"].asFloat(), collision_data["height"].asFloat() );

    if ( Utilities::validateJson( json_data, "collision_active", Utilities::JSON_TYPE_BOOLEAN, false ) )
    {
      _collisionActive = json_data["collision_active"].asBool();
    }
  }

}

