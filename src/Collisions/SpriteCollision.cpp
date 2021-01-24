
#include "Regolith/Collisions/SpriteCollision.h"

#include "Regolith/Managers/Manager.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{


////////////////////////////////////////////////////////////////////////////////////////////////////
  // SpriteCollision class member functions

  SpriteCollision::SpriteCollision() :
    Collision(),
    _collisionFrames(),
    _currentCollision( 0 )
  {
  }


  void SpriteCollision::configure( Json::Value& json_data )
  {
    if ( validateJson( json_data, "hit_boxes", JsonType::ARRAY, false ) )
    {
      DEBUG_LOG( "SpriteCollision::configure : Loading hitboxes from array of frames" );

      validateJsonArray( json_data["hit_boxes"], 0, JsonType::ARRAY );

      // Reserve space for the number of frames in the animation
      _collisionFrames.reserve( (size_t)json_data["hit_boxes"].size() );

      // For each frame
      for ( Json::ArrayIndex frame_num = 0; frame_num < json_data["hit_boxes"].size(); ++frame_num )
      {
        DEBUG_LOG( "SpriteCollision::configure : Loading frame" );

        _collisionFrames.push_back( HitBoxVector() );

        Json::Value& frame_hitboxes = json_data["hit_boxes"][frame_num];
        validateJsonArray( frame_hitboxes, 0, JsonType::OBJECT );

        // Allocate space for the number of hit boxes in this frame
        _collisionFrames[frame_num].reserve( (size_t)frame_hitboxes.size() );

        for ( Json::ArrayIndex hitbox_num = 0; hitbox_num < frame_hitboxes.size(); ++hitbox_num )
        {
          DEBUG_LOG( "SpriteCollision::configure : Loading hitbox" );

          Json::Value& hitbox_data = frame_hitboxes[hitbox_num];
          HitBox hb;

          validateJson( hitbox_data, "type", JsonType::STRING );
          hb.collisionType = Manager::getInstance()->getCollisionType( hitbox_data["type"].asString() );

          if ( validateJson( hitbox_data, "points", JsonType::ARRAY, false ) )
          {
            validateJsonArray( hitbox_data["points"], 2, JsonType::ARRAY );

            Json::Value& points = hitbox_data["points"];
            hb.number = points.size();
            hb.points.reserve( hb.number );
            hb.normals.reserve( hb.number );

            // Push the polygon vertecies
            for ( Json::ArrayIndex i = 0; i < hb.number; ++i )
            {
              validateJsonArray( points[i], 2, JsonType::FLOAT );
              hb.points.push_back( Vector( points[i][0].asFloat(), points[i][1].asFloat() ) );
            }

            // Calculate the normals for each side of the polygon
            std::vector< Vector > deltas;
            for ( Json::ArrayIndex i = 0; i < hb.number; ++i )
            {
              Vector& current_point = hb.points[i];
              Vector& next_point = ( i+1 == hb.number ) ? hb.points[0] : hb.points[i+1];

              // Calculate the left-side normal (We go in a clockwise direction)
              Vector delta = next_point - current_point;
              Vector normal( delta.y(), -delta.x() );

              hb.normals.push_back( normal.norm() );
              deltas.push_back( delta );
            }

            // Check the final shape for convexity
            for ( size_t i = 0; i < hb.number; ++i )
            {
              Vector current_delta = deltas[i];
              Vector next_delta =  ( i+1 == hb.number ) ? deltas[0] : deltas[i+1];
              if ( ( current_delta ^ next_delta ) < 0.0 )
              {
                Exception ex( "SpriteCollision::configure()", "Polygon is not convex. Vertecies must be provided in a clockwise order for a convex shape only." );
                throw ex;
              }
            }
          }
          else // Default is an axis-align bounding box
          {
            validateJson( hitbox_data, "position", JsonType::ARRAY );
            validateJsonArray( hitbox_data["position"], 2, JsonType::FLOAT );
            validateJson( hitbox_data, "width", JsonType::FLOAT );
            validateJson( hitbox_data, "height", JsonType::FLOAT );

            // Its a box, reserve 4 points
            hb.number = 4;
            hb.points.reserve( 4 );
            hb.normals.reserve( 4 );

            float width = hitbox_data["width"].asFloat();
            float height = hitbox_data["height"].asFloat();

            hb.points.push_back( Vector( hitbox_data["position"][0].asFloat(), hitbox_data["position"][1].asFloat() ) );

            hb.points.push_back( hb.points[0] );
            hb.points[1].x() = hb.points[1].x() + width;

            hb.points.push_back( hb.points[1] );
            hb.points[2].y() = hb.points[2].y() + height;

            hb.points.push_back( hb.points[2] );
            hb.points[3].x() = hb.points[3].x() - width;

            hb.normals.push_back( -unitVector_y );
            hb.normals.push_back(  unitVector_x );
            hb.normals.push_back(  unitVector_y );
            hb.normals.push_back( -unitVector_x );
          }

          _collisionFrames[frame_num].push_back( hb );

          DEBUG_LOG( "SpriteCollision::configure : Hitbox configured" );
          for ( unsigned int i = 0; i < hb.number; ++i )
          {
            DEBUG_STREAM << "SpriteCollision::configure :   Hitbox point : " << hb.points[i] << ", normal : " << hb.normals[i];
          }
        }
      }

      DEBUG_LOG( "SpriteCollision::configure : Loaded all hitboxes" );
    }
    else if ( validateJson( json_data, "hit_box_file", JsonType::STRING, false ) )
    {
      Exception ex( "SpriteCollision::configure()", "Hit box files not yet supported" );
      throw ex;
    }

  }

}

