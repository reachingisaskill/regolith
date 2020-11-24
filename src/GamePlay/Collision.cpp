
#include "Regolith/GamePlay/Collision.h"

#include "Regolith/Managers/Manager.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Collision class member functions

  Collision::Collision() :
    _collisionFrames(),
    _currentCollision( 0 )
  {
  }


  void Collision::configure( Json::Value& json_data )
  {
    // Configure me senpai...

    if ( Utilities::validateJson( json_data, "hit_boxes", Utilities::JSON_TYPE_ARRAY, false ) )
    {
      DEBUG_LOG( "Collision::configure : Loading hitboxes from array of frames" );

      Utilities::validateJsonArray( json_data["hit_boxes"], 1, Utilities::JSON_TYPE_ARRAY );

      // Reserve space for the number of frames in the animation
      _collisionFrames.reserve( (size_t)json_data["hit_boxes"].size() );

      // For each frame
      for ( Json::ArrayIndex frame_num = 0; frame_num < json_data["hit_boxes"].size(); ++frame_num )
      {
        DEBUG_LOG( "Collision::configure : Loading frame" );

        _collisionFrames.push_back( HitBoxVector() );

        Json::Value& frame_hitboxes = json_data["hit_boxes"][frame_num];
        Utilities::validateJsonArray( frame_hitboxes, 0, Utilities::JSON_TYPE_OBJECT );

        // Allocate space for the number of hit boxes in this frame
        _collisionFrames[frame_num].reserve( (size_t)frame_hitboxes.size() );

        for ( Json::ArrayIndex hitbox_num = 0; hitbox_num < frame_hitboxes.size(); ++hitbox_num )
        {
          DEBUG_LOG( "Collision::configure : Loading hitbox" );

          Json::Value& hitbox_data = frame_hitboxes[hitbox_num];

//          DEBUG_STREAM << "Collision::configure : " << hitbox_data.asString();
          DEBUG_STREAM << "Collision::configure : " << hitbox_data.isMember( "position" );

          Utilities::validateJson( hitbox_data, "position", Utilities::JSON_TYPE_ARRAY );
          Utilities::validateJsonArray( hitbox_data["position"], 2, Utilities::JSON_TYPE_INTEGER );
          Utilities::validateJson( hitbox_data, "width", Utilities::JSON_TYPE_INTEGER );
          Utilities::validateJson( hitbox_data, "height", Utilities::JSON_TYPE_INTEGER );
          Utilities::validateJson( hitbox_data, "type", Utilities::JSON_TYPE_STRING );

          HitBox hb;
          hb.position.x() = hitbox_data["position"][0].asInt();
          hb.position.y() = hitbox_data["position"][1].asInt();
          hb.width = hitbox_data["width"].asInt();
          hb.height = hitbox_data["height"].asInt();
          hb.type = Manager::getInstance()->getCollisionType( hitbox_data["type"].asString() );

          _collisionFrames[frame_num].push_back( hb );
        }
      }
    }
    else if ( Utilities::validateJson( json_data, "hit_box_file", Utilities::JSON_TYPE_STRING, false ) )
    {
      Exception ex( "Collision::configure()", "Hit box files not yet supported" );
      throw ex;
    }

  }

}

