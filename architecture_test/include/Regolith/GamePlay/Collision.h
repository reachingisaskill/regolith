
#ifndef REGOLITH_COLLISION_H_
#define REGOLITH_COLLISION_H_

#include "Regolith/Global/Global.h"


namespace Regolith
{
  /*
   * Collision is implemented only as axis aligned bounding boxes for the current version of Regolith.
   * 
   * Some assumptions are made. E.g. the side with the shortest overlap is the side at which the overlap
   * happened. This is reasonably good assumption for when onve box is much larger than the other, or
   * when the overlap distance is much smaller than the dimensions of the box.
   * I will (eventually) switch to a model which uses incoming delta-velocity to determine the side of 
   * the impact.
   */

  // Forward declarations
  class Contact;
  class Collision;
  class Collidable;

  // Function that produces the contact class
  bool collides( Collidable*, Collidable*, Contact& );

  // Function that returns true if the first object contains the second object
  bool contains( Collidable*, Collidable* );

  // Function that returns true if the object contains point
  bool contains( Collidable*, const Vector& );


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Base class for all collision models
  class Collision
  {
    friend bool collides( Collidable*, Collidable*, Contact& );
    friend bool contains( Collidable*, Collidable* );
    friend bool contains( Collidable*, const Vector& );

    private:
      // Position with respect to the owning drawable object
      Vector _position;
      float _width;
      float _height;

      // Add daughter collision objects

    public:
      Collision();

      Collision( Vector, float, float );

      virtual ~Collision();

      Vector& position() { return _position; }
      float& width() { return _width; }
      float& height() { return _height; }
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Contact class
  class Contact
  {
    private:
      Vector _overlap1;
      Vector _overlap2;

      Collidable* _object1;
      Collidable* _object2;

    public:
      Contact();

      void set( Collidable*, Collidable*, Vector, float );

      void applyContact();

      const Vector& overlap1() { return _overlap1; }
      const Vector& overlap2() { return _overlap2; }

      const Collidable* object1() { return _object1; }
      const Collidable* object2() { return _object2; }

  };


}

#endif // REGOLITH_COLLISION_H_

