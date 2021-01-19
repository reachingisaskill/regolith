
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

      validateJsonArray( json_data["hit_boxes"], 0, JsonType::ARRAY );

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
          HitBox hb;

          validateJson( hitbox_data, "type", JsonType::STRING );
          hb.collisionType = Manager::getInstance()->getCollisionType( hitbox_data["type"].asString() );

          if ( validateJson( hitbox_data, "shape", JsonType::STRING, false ) )
          {
            std::string shape = hitbox_data["shape"].asString();

            if ( shape == "circle" )
            {
              validateJson( hitbox_data, "radius", JsonType::FLOAT );
              validateJson( hitbox_data, "center", JsonType::ARRAY );
              validateJsonArray( hitbox_data["center"], 2, JsonType::FLOAT );

              hb.number = 0;
              hb.points.reserve( 1 );
              hb.shape = HitBoxType::Circle;

              hb.points.push_back( Vector( hitbox_data["center"][0].asFloat(), hitbox_data["center"][1].asFloat() ) );
              hb.points.push_back( Vector( hitbox_data["radius"].asFloat(), 0.0 ) );

              hb.normals.push_back( unitVector_x );
            }
            else if ( shape == "polygon" )
            {
              validateJson( hitbox_data, "points", JsonType::ARRAY );
              validateJsonArray( hitbox_data["points"], 2, JsonType::ARRAY );

              Json::Value& points = hitbox_data["points"];
              hb.number = points.size();
              hb.points.reserve( hb.number );
              hb.normals.reserve( hb.number );
              hb.shape = HitBoxType::Polygon;

              for ( Json::ArrayIndex i = 0; i < hb.number; ++i )
              {
                validateJsonArray( points[i], 2, JsonType::FLOAT );
                hb.points.push_back( Vector( points[i][0].asFloat(), points[i][1].asFloat() ) );
              }

              Vector current_point;
              Vector last_point = hb.points[hb.number-1];
              for ( Json::ArrayIndex i = 0; i < hb.number; ++i )
              {
                current_point = hb.points[i];

                // Calculate the left-side normal (We go in a clockwise direction!)
                Vector delta = current_point - last_point;
                Vector normal( -delta.y(), delta.x() );
                hb.normals.push_back( normal );

                last_point = current_point;
              }
            }
            else
            {
              Exception ex( "Collision::configure", "Unknown hit-box shape" );
              ex.addDetail( "Shape", shape );
              throw ex;
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
            hb.shape = HitBoxType::Polygon;

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

          DEBUG_LOG( "Collision::configure : Hitbox configured" );
          for ( unsigned int i = 0; i < hb.number; ++i )
          {
            DEBUG_STREAM << "Collision::configure :   Hitbox point : " << hb.points[i] << ", normal : " << hb.normals[i];
          }
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

