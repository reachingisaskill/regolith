
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















////////////////////////////////////////////////////////////////////////////////////////////////////
  // Something?


//  void callback( Collidable* obj1, Collidable* obj2, const Vector& normal, const float overlap )
//  {
//    float invM1 = obj1->getInverseMass();
//    float invM2 = obj2->getInverseMass();
//    float totalInvM = invM1 + invM2;
//
//    if ( totalInvM < epsilon )
//    {
//      obj1->onCollision( -normal, 0.0, obj2 );
//      obj2->onCollision(  normal, 0.0, obj1 );
//    }
//    else 
//    {
//      obj1->onCollision( -normal, (invM1*overlap/totalInvM), obj2 );
//      obj2->onCollision(  normal, (invM2*overlap/totalInvM), obj1 );
//    }
//  }

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Collides functions

//  void collides( Collidable* object1, Collidable* object2 )
//  {
//    DEBUG_LOG( "Checking Collision" );
//
//    const Collision& collision1 = object1->getCollision();
//    const Collision& collision2 = object2->getCollision();
//
//    Vector pos1 = object1->position() + collision1.position(); // Move into global coordinate system
//    Vector pos2 = object2->position() + collision2.position(); // Move into global coordinate system
//
//    DEBUG_STREAM << " Global Pos1 : " << pos1 << " W = " << collision1.width() << " H = " << collision1.height();
//    DEBUG_STREAM << " Global Pos2 : " << pos2 << " W = " << collision2.width() << " H = " << collision2.height();
//
//
//    // X Axis
//    float diff_x = pos2.x() - pos1.x();
//
//    ////////////////////////////////////////////////// 
//    // IF OBJ2 IN FRONT OBJ1 - X
//    if ( diff_x > 0.0 ) 
//    {
//      float overlap_x = collision1.width() - diff_x;
//      DEBUG_STREAM << " Diff_x = " << diff_x << ", Overlap = " << overlap_x;
//
//      if ( overlap_x > 0.0 )
//      {
//        // Y Axis
//        float diff_y = pos2.y() - pos1.y();
//
//        ////////////////////////////////////////////////// 
//        // IF OBJ2 IN FRONT OBJ1 - Y
//        if ( diff_y >= 0.0 )
//        {
//          float overlap_y = collision1.height() - diff_y;
//
//          DEBUG_STREAM << " Diff_y = " << diff_y << ", Overlap = " << overlap_y;
//
//          if ( overlap_y > 0.0 )
//          {
//            // Both X & Y overlap - set the reference contact object
//            if ( overlap_x >= overlap_y )
//            {
//              callback( object1, object2, unitVector_y, overlap_y );
//            }
//            else
//            {
//              callback( object1, object2, unitVector_x, overlap_x );
//            }
//          }
//        }
//        ////////////////////////////////////////////////// 
//        // IF OBJ2 BEHIND OBJ1 - Y
//        else
//        {
//          float overlap_y = collision2.height() + diff_y;
//
//          DEBUG_STREAM << " Diff_y = " << diff_y << ", Overlap = " << overlap_y;
//          if ( overlap_y > 0.0 )
//          {
//            // Both X & Y overlap - set the reference contact object
//            if ( overlap_x >= overlap_y )
//            {
//              callback( object1, object2, -unitVector_y, overlap_y );
//            }
//            else
//            {
//              callback( object1, object2, unitVector_x, overlap_x );
//            }
//          }
//        }
//      }
//    }
//    ////////////////////////////////////////////////// 
//    // IF OBJ2 BEHIND OBJ1 - X
//    else
//    {
//      float overlap_x = collision2.width() + diff_x;
//      DEBUG_STREAM << " Diff_x = " << diff_x << ", Overlap = " << overlap_x;
//
//      if ( overlap_x > 0.0 )
//      {
//        // Y Axis
//        float diff_y = pos2.y() - pos1.y();
//        ////////////////////////////////////////////////// 
//        // IF OBJ2 IN FRONT OBJ1 - Y
//        if ( diff_y >= 0.0 )
//        {
//          float overlap_y = collision1.height() - diff_y;
//
//          DEBUG_STREAM << "   Diff_y = " << diff_y << ", Overlap = " << overlap_y;
//          if ( overlap_y > 0.0 )
//          {
//            // Both X & Y overlap - set the reference contact object
//            if ( overlap_x >= overlap_y )
//            {
//              callback( object1, object2, unitVector_y, overlap_y );
//            }
//            else
//            {
//              callback( object1, object2, -unitVector_x, overlap_x );
//            }
//          }
//        }
//        ////////////////////////////////////////////////// 
//        // IF OBJ2 BEHIND OBJ1 - Y
//        else
//        {
//          float overlap_y = collision2.height() + diff_y;
//
//          DEBUG_STREAM << " Diff_y = " << diff_y << ", Overlap = " << overlap_y;
//          if ( overlap_y > 0.0 )
//          {
//            // Both X & Y overlap - set the reference contact object
//            if ( overlap_x >= overlap_y )
//            {
//              callback( object1, object2, -unitVector_y, overlap_y );
//            }
//            else
//            {
//              callback( object1, object2, -unitVector_x, overlap_x );
//            }
//          }
//        }
//      }
//    }
//  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Contains function


  /*
  bool contains( Collidable* parent, Collidable* child )
  {
    DEBUG_LOG( "Checking Containment" );

    const Collision& parent_coll = parent->getCollision();
    const Collision& child_coll = child->getCollision();

    Vector parent_pos;
    Vector child_pos;

    parent_pos = parent->position() + parent_coll.position(); // Move into global coordinate system
    DEBUG_STREAM << " Global Pos1 : " << parent_pos << " W = " << parent_coll.width() << " H = " << parent_coll.height();

    child_pos = child->position() + child_coll.position(); // Move into global coordinate system
    DEBUG_STREAM << "   Global Pos2 : " << child_pos << " W = " << child_coll.width() << " H = " << child_coll.height();


    // X Axis
    float diff_x = child_pos.x() - parent_pos.x();
    if ( ( diff_x > -child_coll.width() ) && ( diff_x < parent_coll.width() ) ) 
    {
      // Y Axis
      float diff_y = child_pos.y() - parent_pos.y();
      if ( ( diff_y > child_coll.height() ) && ( diff_y < parent_coll.height() ) )
      {
        return true;
      }
    }

    return false;
  }
  */



//  void contains( Collidable* parent, Collidable* child )
//  {
//    DEBUG_LOG( "Checking Containment" );
//
//    const Collision& parent_coll = parent->getCollision();
//    const Collision& child_coll = child->getCollision();
//
//    Vector parent_pos;
//    Vector child_pos;
//
//    parent_pos = parent->position() + parent_coll.position(); // Move into global coordinate system
//    DEBUG_STREAM << " Global Pos1 : " << parent_pos << " W = " << parent_coll.width() << " H = " << parent_coll.height();
//
//    child_pos = child->position() + child_coll.position(); // Move into global coordinate system
//    DEBUG_STREAM << " Global Pos2 : " << child_pos << " W = " << child_coll.width() << " H = " << child_coll.height();
//
//
//    // X Axis
//    float diff_x = child_pos.x() - parent_pos.x();
//    float diff_y = child_pos.y() - parent_pos.y();
//
//    if ( ( diff_x >= 0.0 ) && ( ( diff_x + child_coll.width()) <= parent_coll.width() ) )
//      if ( ( diff_y >= 0.0 ) && ( ( diff_y + child_coll.height()) <= parent_coll.height() ) )
//        return; // Contained
//
//    if ( diff_x > 0.0 )
//    {
//      diff_x += child_coll.width() - parent_coll.width();
//      if ( diff_x < 0.0 ) diff_x = 0.0;
//    }
//
//    if ( diff_y > 0.0 )
//    {
//      diff_y += child_coll.height() - parent_coll.height();
//      if ( diff_y < 0.0 ) diff_y = 0.0;
//    }
//
//    Vector normal( diff_x, diff_y );
//    float length = normal.mod();
//
//    DEBUG_STREAM << "DIFF_X = " << diff_x << " ,  DIFF_Y = " << diff_y << " L = " << length;
//
//
//    callback( child, parent, (normal /= length), length );
//  }


//  bool contains( ContextLayer& layer, Collidable* object )
//  {
//    DEBUG_LOG( "Checking Layer Containement" );
//
//    const Collision& object_coll = object->getCollision();
//
//    const Vector& layer_pos = layer.getPosition();
//    Vector object_pos;
//
//    object_pos = object->position() + object_coll.position(); // Move into global coordinate system
//    DEBUG_STREAM << "   Global Pos2 : " << object_pos << " W = " << object_coll.width() << " H = " << object_coll.height();
//
//
//    // X Axis
//    float diff_x = object_pos.x() - layer_pos.x();
//    if ( ( diff_x > -object_coll.width() ) && ( diff_x < layer.getWidth() ) ) 
//    {
//      // Y Axis
//      float diff_y = object_pos.y() - layer_pos.y();
//      if ( ( diff_y > object_coll.height() ) && ( diff_y < layer.getHeight() ) )
//      {
//        return true;
//      }
//    }
//
//    return false;
//  }


//  bool contains( Collidable* object, const Vector& point )
//  {
//    DEBUG_LOG( "Checking Point Containment" );
//
//    const Collision& collision = object->getCollision();
//    Vector pos = object->position() + collision.position(); // Move into global coordinate system
//    DEBUG_STREAM << " Global Pos1 : " << pos << " W = " << collision.width() << " H = " << collision.height();
//
//    // X Axis
//    float diff_x = point.x() - pos.x();
//    if ( ( diff_x > 0.0 ) && ( diff_x < collision.width() ) )
//    {
//      float diff_y = point.y() - pos.y();
//      if ( ( diff_y > 0.0 ) && ( diff_y < collision.height() ) ) 
//      {
//        return true;
//      }
//    }
//
//    return false;
//  }

}

