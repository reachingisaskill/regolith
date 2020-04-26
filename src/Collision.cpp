#define __DEBUG_OFF__

#include "Collision.h"

#include "Manager.h"

#include "logtastic.h"


namespace Regolith
{

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Collides functions

  bool collides( Drawable* object1, Drawable* object2, Contact& cont )
  {
    DEBUG_LOG( "Checking Collision" );
    if ( ( ! object1->collisionActive()) || ( ! object2->collisionActive() ) ) return false;

    Collision* collisions1 = nullptr;
    Collision* collisions2 = nullptr;
    unsigned int number1 = object1->getCollision( collisions1 );
    unsigned int number2 = object2->getCollision( collisions2 );
    Vector pos1;
    Vector pos2;

    Collision* c_iter1 = collisions1;
    Collision* c_iter2;
    for ( unsigned int i = 0; i < number1; ++i, ++c_iter1 )
    {
      pos1 = object1->position() + c_iter1->position(); // Move into global coordinate system
      DEBUG_STREAM << " Global Pos1, " << i << " : " << pos1 << " W = " << c_iter1->width() << " H = " << c_iter1->height();

      c_iter2 = collisions2;
      for ( unsigned int j = 0; j < number2; ++j, ++c_iter2 )
      {
        pos2 = object2->position() + c_iter2->position(); // Move into global coordinate system
        DEBUG_STREAM << "   Global Pos2, " << j << " : " << pos2 << " W = " << c_iter2->width() << " H = " << c_iter2->height();

        // X Axis
        float diff_x = pos2.x() - pos1.x();
        ////////////////////////////////////////////////// 
        // IF OBJ2 IN FRONT OBJ1 - X
        if ( diff_x > 0.0 ) 
        {
          float overlap_x = c_iter1->_width - diff_x;
          DEBUG_STREAM << "   Diff_x = " << diff_x << ", Overlap = " << overlap_x;

          if ( overlap_x > 0.0 )
          {
            // Y Axis
            float diff_y = pos2.y() - pos1.y();
            ////////////////////////////////////////////////// 
            // IF OBJ2 IN FRONT OBJ1 - Y
            if ( diff_y >= 0.0 )
            {
              float overlap_y = c_iter1->_height - diff_y;

              DEBUG_STREAM << "   Diff_y = " << diff_y << ", Overlap = " << overlap_y;
              if ( overlap_y > 0.0 )
              {
                // Both X & Y overlap - set the reference contact object
                if ( overlap_x >= overlap_y )
                {
                  cont.set( object1, object2, unitVector_y, overlap_y );
                  return true;
                }
                else
                {
                  cont.set( object1, object2, unitVector_x, overlap_x );
                  return true;
                }
              }
            }
            ////////////////////////////////////////////////// 
            // IF OBJ2 BEHIND OBJ1 - Y
            else
            {
              float overlap_y = c_iter2->_height + diff_y;

              DEBUG_STREAM << "   Diff_y = " << diff_y << ", Overlap = " << overlap_y;
              if ( overlap_y > 0.0 )
              {
                // Both X & Y overlap - set the reference contact object
                if ( overlap_x >= overlap_y )
                {
                  cont.set( object1, object2, -unitVector_y, overlap_y );
                  return true;
                }
                else
                {
                  cont.set( object1, object2, unitVector_x, overlap_x );
                  return true;
                }
              }
            }
          }
        }
        ////////////////////////////////////////////////// 
        // IF OBJ2 BEHIND OBJ1 - X
        else
        {
          float overlap_x = c_iter2->_width + diff_x;
          DEBUG_STREAM << "   Diff_x = " << diff_x << ", Overlap = " << overlap_x;

          if ( overlap_x > 0.0 )
          {
            // Y Axis
            float diff_y = pos2.y() - pos1.y();
            ////////////////////////////////////////////////// 
            // IF OBJ2 IN FRONT OBJ1 - Y
            if ( diff_y >= 0.0 )
            {
              float overlap_y = c_iter1->_height - diff_y;

              DEBUG_STREAM << "   Diff_y = " << diff_y << ", Overlap = " << overlap_y;
              if ( overlap_y > 0.0 )
              {
                // Both X & Y overlap - set the reference contact object
                if ( overlap_x >= overlap_y )
                {
                  cont.set( object1, object2, unitVector_y, overlap_y );
                  return true;
                }
                else
                {
                  cont.set( object1, object2, -unitVector_x, overlap_x );
                  return true;
                }
              }
            }
            ////////////////////////////////////////////////// 
            // IF OBJ2 BEHIND OBJ1 - Y
            else
            {
              float overlap_y = c_iter2->_height + diff_y;

              DEBUG_STREAM << "   Diff_y = " << diff_y << ", Overlap = " << overlap_y;
              if ( overlap_y > 0.0 )
              {
                // Both X & Y overlap - set the reference contact object
                if ( overlap_x >= overlap_y )
                {
                  cont.set( object1, object2, -unitVector_y, overlap_y );
                  return true;
                }
                else
                {
                  cont.set( object1, object2, -unitVector_x, overlap_x );
                  return true;
                }
              }
            }
          }
        }
      }
    }

    return false;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Contains function

  bool contains( Drawable* parent, Drawable* child )
  {
    DEBUG_LOG( "Checking Containment" );
    if ( ( ! parent->collisionActive()) || ( ! child->collisionActive() ) ) return false;

    Collision* parent_coll = nullptr;
    Collision* child_coll = nullptr;
    unsigned int p_number = parent->getCollision( parent_coll );
    unsigned int c_number = child->getCollision( child_coll );
    Vector parent_pos;
    Vector child_pos;

    Collision* p_iter = parent_coll;
    Collision* c_iter;
    for ( unsigned int i = 0; i < p_number; ++i, ++p_iter )
    {
      parent_pos = parent->position() + p_iter->position(); // Move into global coordinate system
      DEBUG_STREAM << " Global Pos1, " << i << " : " << parent_pos << " W = " << p_iter->width() << " H = " << p_iter->height();

      c_iter = child_coll;
      for ( unsigned int j = 0; j < c_number; ++j, ++c_iter )
      {
        child_pos = child->position() + c_iter->position(); // Move into global coordinate system
        DEBUG_STREAM << "   Global Pos2, " << j << " : " << child_pos << " W = " << c_iter->width() << " H = " << c_iter->height();

        // X Axis
        float diff_x = child_pos.x() - parent_pos.x();
        if ( diff_x < 0.0 ) return false;

        if ( diff_x > (p_iter->_width - c_iter->_width) ) return false;

        // Y Axis
        float diff_y = child_pos.y() - parent_pos.y();
        if ( diff_y < 0.0 ) return false;

        if ( diff_y > (p_iter->_height - c_iter->_height) ) return false;

      }
    }

    return true;
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


  Collision* Collision::clone() const
  {
    return new Collision( *this );
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Contact class member functions

  Contact::Contact() :
    _normal(),
    _overlap(),
    _object1( nullptr ),
    _object2( nullptr )
  {
  }


  void Contact::set( Drawable* obj1, Drawable* obj2, Vector norm, float overlap )
  {
    _object1 = obj1;
    _object2 = obj2;
    _normal = norm;
    _overlap = overlap;
  }


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
  }

}

