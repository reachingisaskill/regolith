
#include "Collision.h"


namespace Regolith
{

  bool collides( Drawable* object1, Drawable* object2, Contact& cont )
  {
    Collision* collisions1 = nullptr;
    Collision* collisions2 = nullptr;
    unsigned int number1 = object1->getCollision( collisions1 );
    unsigned int number2 = object2->getCollision( collisions2 );

    for ( unsigned int i = 0; i < number1; ++i, ++collisions1 )
    {
      for ( unsigned int j = 0; j < number2; ++j, ++collisions2 )
      {
        // X Axis
        float diff_x = collisions2->_position.x() - collisions1->_position.x();
        float overlap_x = ( (diff_x < 0.0) ? (diff_x - collisions1->_width) : (-diff_x - collisions2->_width) );
        if ( overlap_x > 0.0 )
        {
          float diff_y = collisions2->_position.y() - collisions1->_position.y();
          float overlap_y = ( (diff_y < 0.0) ? (diff_y - collisions1->_height) : (-diff_y - collisions2->_height) );
          if ( overlap_y > 0.0 )
          {
            if ( overlap_x > overlap_y )
              cont.set( object1, object2, unitVector_x, overlap_x );
            else
              cont.set( object1, object2, unitVector_y, overlap_y );
          }
        }
      }
    }

    return false;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Collision class member functions

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

