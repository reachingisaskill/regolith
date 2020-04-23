
#include "Collision.h"

#include "logtastic.h"


namespace Regolith
{

  bool collides( Drawable* object1, Drawable* object2, Contact& cont )
  {
    DEBUG_LOG( "Checking Collision" );
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
  }

}

