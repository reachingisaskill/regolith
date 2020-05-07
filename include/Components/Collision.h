
#ifndef REGOLITH_COLLISION_H_
#define REGOLITH_COLLISION_H_

#include "Components/Vector.h"
#include "Architecture/Drawable.h"


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

  // Function that produces the contact class
  bool collides( Drawable*, Drawable*, Contact& );

  // Function that returns true if the first object contains the second object
  bool contains( Drawable*, Drawable* );

  // Function that returns true if the object contains point
  bool contains( Drawable*, const Vector& );


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Base class for all collision models
  class Collision
  {
    friend bool collides( Drawable*, Drawable*, Contact& );
    friend bool contains( Drawable*, Drawable* );
    friend bool contains( Drawable*, const Vector& );
    private:
      // Position with respect to the owning drawable object
      Vector _position;
      float _width;
      float _height;

    public:
      Collision();

      Collision( Vector, float, float );

      virtual ~Collision();

      Vector& position() { return _position; }
      float& width() { return _width; }
      float& height() { return _height; }

      virtual Collision* clone() const;

  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Contact class
  class Contact
  {
    private:
      Vector _normal;
      float _overlap;

      Drawable* _object1;
      Drawable* _object2;

    public:
      Contact();

      void set( Drawable*, Drawable*, Vector, float );

      void applyContact();

      Vector& normal() { return _normal; }

      float& overlap() { return _overlap; }

  };


}

#endif // REGOLITH_COLLISION_H_

