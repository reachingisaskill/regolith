#define LOGTASTIC_DEBUG_OFF

#include "Regolith/GamePlay/Collision.h"
#include "Regolith/Architecture/Collidable.h"

#include "logtastic.h"


namespace Regolith
{

  void callback( Collidable* obj1, Collidable* obj2, const Vector& normal, const float overlap )
  {
    Vector overlap1 = (-overlap)*normal;
    Vector overlap2 =   overlap *normal;
    obj1->onCollision( overlap1, obj2 );
    obj2->onCollision( overlap2, obj1 );
  }

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Collides functions

  void collides( Collidable* object1, Collidable* object2 )
  {
    DEBUG_LOG( "Checking Collision" );

    const Collision& collision1 = object1->getCollision();
    const Collision& collision2 = object2->getCollision();

    Vector pos1 = object1->position() + collision1.position(); // Move into global coordinate system
    Vector pos2 = object2->position() + collision2.position(); // Move into global coordinate system

    DEBUG_STREAM << " Global Pos1 : " << pos1 << " W = " << collision1.width() << " H = " << collision1.height();
    DEBUG_STREAM << " Global Pos2 : " << pos2 << " W = " << collision2.width() << " H = " << collision2.height();


    // X Axis
    float diff_x = pos2.x() - pos1.x();

    ////////////////////////////////////////////////// 
    // IF OBJ2 IN FRONT OBJ1 - X
    if ( diff_x > 0.0 ) 
    {
      float overlap_x = collision1.width() - diff_x;
      DEBUG_STREAM << " Diff_x = " << diff_x << ", Overlap = " << overlap_x;

      if ( overlap_x > 0.0 )
      {
        // Y Axis
        float diff_y = pos2.y() - pos1.y();

        ////////////////////////////////////////////////// 
        // IF OBJ2 IN FRONT OBJ1 - Y
        if ( diff_y >= 0.0 )
        {
          float overlap_y = collision1.height() - diff_y;

          DEBUG_STREAM << " Diff_y = " << diff_y << ", Overlap = " << overlap_y;

          if ( overlap_y > 0.0 )
          {
            // Both X & Y overlap - set the reference contact object
            if ( overlap_x >= overlap_y )
            {
              callback( object1, object2, unitVector_y, overlap_y );
            }
            else
            {
              callback( object1, object2, unitVector_x, overlap_x );
            }
          }
        }
        ////////////////////////////////////////////////// 
        // IF OBJ2 BEHIND OBJ1 - Y
        else
        {
          float overlap_y = collision2.height() + diff_y;

          DEBUG_STREAM << " Diff_y = " << diff_y << ", Overlap = " << overlap_y;
          if ( overlap_y > 0.0 )
          {
            // Both X & Y overlap - set the reference contact object
            if ( overlap_x >= overlap_y )
            {
              callback( object1, object2, -unitVector_y, overlap_y );
            }
            else
            {
              callback( object1, object2, unitVector_x, overlap_x );
            }
          }
        }
      }
    }
    ////////////////////////////////////////////////// 
    // IF OBJ2 BEHIND OBJ1 - X
    else
    {
      float overlap_x = collision2.width() + diff_x;
      DEBUG_STREAM << " Diff_x = " << diff_x << ", Overlap = " << overlap_x;

      if ( overlap_x > 0.0 )
      {
        // Y Axis
        float diff_y = pos2.y() - pos1.y();
        ////////////////////////////////////////////////// 
        // IF OBJ2 IN FRONT OBJ1 - Y
        if ( diff_y >= 0.0 )
        {
          float overlap_y = collision1.height() - diff_y;

          DEBUG_STREAM << "   Diff_y = " << diff_y << ", Overlap = " << overlap_y;
          if ( overlap_y > 0.0 )
          {
            // Both X & Y overlap - set the reference contact object
            if ( overlap_x >= overlap_y )
            {
              callback( object1, object2, unitVector_y, overlap_y );
            }
            else
            {
              callback( object1, object2, -unitVector_x, overlap_x );
            }
          }
        }
        ////////////////////////////////////////////////// 
        // IF OBJ2 BEHIND OBJ1 - Y
        else
        {
          float overlap_y = collision2.height() + diff_y;

          DEBUG_STREAM << " Diff_y = " << diff_y << ", Overlap = " << overlap_y;
          if ( overlap_y > 0.0 )
          {
            // Both X & Y overlap - set the reference contact object
            if ( overlap_x >= overlap_y )
            {
              callback( object1, object2, -unitVector_y, overlap_y );
            }
            else
            {
              callback( object1, object2, -unitVector_x, overlap_x );
            }
          }
        }
      }
    }
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Contains function

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


  bool contains( Collidable* object, const Vector& point )
  {
    DEBUG_LOG( "Checking Point Containment" );

    const Collision& collision = object->getCollision();
    Vector pos = object->position() + collision.position(); // Move into global coordinate system
    DEBUG_STREAM << " Global Pos1 : " << pos << " W = " << collision.width() << " H = " << collision.height();

    // X Axis
    float diff_x = point.x() - pos.x();
    if ( ( diff_x > 0.0 ) && ( diff_x < collision.width() ) )
    {
      float diff_y = point.y() - pos.y();
      if ( ( diff_y > 0.0 ) && ( diff_y < collision.height() ) ) 
      {
        return true;
      }
    }

    return false;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Collision class member functions

  Collision::Collision() :
    _position(),
    _width(),
    _height()
  {
  }


  Collision::Collision( Vector pos, float width, float height ) :
    _position( pos ),
    _width( width ),
    _height( height )
  {
  }


  Collision::~Collision()
  {
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Contact class member functions

  Contact::Contact() :
    _overlap1(),
    _overlap2(),
    _object1( nullptr ),
    _object2( nullptr )
  {
  }


  void Contact::set( Collidable* obj1, Collidable* obj2, Vector norm, float overlap )
  {
    _object1 = obj1;
    _object2 = obj2;
    _overlap1 = (-overlap) * norm;
    _overlap2 =   overlap  * norm;
  }


  void Contact::applyContact()
  {
    _object1->onCollision( _overlap1, _object2 );
    _object2->onCollision( _overlap2, _object1 );
  }

  /*
  void Contact::applyContact()
  {
    static Manager* man = Manager::getInstance();
    float invM1 = _object1->getInverseMass();
    float invM2 = _object2->getInverseMass();
    float totalInvM = invM1 + invM2;

    if ( totalInvM < epsilon ) return; // Both immovable objects!

    // Move objects out of collision weighted by their inverse mass
    Vector overlapVector = _normal*_overlap;
    _object1->move( -overlapVector * (invM1 / totalInvM) );
    _object2->move( overlapVector * (invM2 / totalInvM) );

    // Nullify relevant forces
    _object1->applyForce( man->getGravity()%_normal );
    _object2->applyForce( -man->getGravity()%_normal );

    // Remove velocity component
    float perp_velocity = -_object1->getVelocity() * _normal;
    _object1->addVelocity( perp_velocity * _normal );

    perp_velocity = _object2->getVelocity() * _normal;
    _object2->addVelocity( perp_velocity * _normal );

    DEBUG_STREAM << "Ov:" << overlapVector << ", TotIM: " << totalInvM << ", kick:" << overlapVector * (invM1 / totalInvM) << ", Force: " << man->getGravity()%_normal << ", newVel: " << _object1->getVelocity();

    _object1->onCollision( _object2->getTeam(), -overlapVector );
    _object2->onCollision( _object1->getTeam(), overlapVector );
  }
  */

}

