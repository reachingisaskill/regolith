
#include "Regolith/Collisions/Collision.h"

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
    if ( validateJson( json_data, "hit_boxes", JsonType::ARRAY, false ) )
    {
      DEBUG_LOG( "Collision::configure : Loading hitboxes from array of frames" );

      validateJsonArray( json_data["hit_boxes"], 1, JsonType::ARRAY );

      // Reserve space for the number of frames in the animation
      _collisionFrames.reserve( (size_t)json_data["hit_boxes"].size() );

      // For each frame
      for ( Json::ArrayIndex frame_num = 0; frame_num < json_data["hit_boxes"].size(); ++frame_num )
      {
        DEBUG_LOG( "Collision::configure : Loading frame" );

        _collisionFrames.push_back( HitBoxVector() );

        Json::Value& frame_hitboxes = json_data["hit_boxes"][frame_num];
        validateJsonArray( frame_hitboxes, 0, JsonType::OBJECT );

        // Allocate space for the number of hit boxes in this frame
        _collisionFrames[frame_num].reserve( (size_t)frame_hitboxes.size() );

        for ( Json::ArrayIndex hitbox_num = 0; hitbox_num < frame_hitboxes.size(); ++hitbox_num )
        {
          DEBUG_LOG( "Collision::configure : Loading hitbox" );

          Json::Value& hitbox_data = frame_hitboxes[hitbox_num];

          validateJson( hitbox_data, "position", JsonType::ARRAY );
          validateJsonArray( hitbox_data["position"], 2, JsonType::INTEGER );
          validateJson( hitbox_data, "width", JsonType::INTEGER );
          validateJson( hitbox_data, "height", JsonType::INTEGER );
          validateJson( hitbox_data, "type", JsonType::STRING );

          HitBox hb;
          hb.position.x() = hitbox_data["position"][0].asInt();
          hb.position.y() = hitbox_data["position"][1].asInt();
          hb.width = hitbox_data["width"].asInt();
          hb.height = hitbox_data["height"].asInt();
          hb.type = Manager::getInstance()->getCollisionType( hitbox_data["type"].asString() );

//          hb.halfWidth = 0.5*hb.width;
//          hb.halfHeight = 0.5*hb.height;
//          hb.center.x() = hb.position().x() + hb.halfWidth;
//          hb.center.y() = hb.position().y() + hb.halfHeight;

          _collisionFrames[frame_num].push_back( hb );
        }
      }

      DEBUG_LOG( "Collision::configure : Loaded all hitboxes" );
    }
    else if ( validateJson( json_data, "hit_box_file", JsonType::STRING, false ) )
    {
      Exception ex( "Collision::configure()", "Hit box files not yet supported" );
      throw ex;
    }

  }

}

