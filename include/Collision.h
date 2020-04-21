
#ifndef __REGOLITH__COLLISION_H__
#define __REGOLITH__COLLISION_H__

#include "Vector.h"
#include "Drawable.h"


namespace Regolith
{
  /*
   * Collision is implemented only as axis aligned bounding boxes for the current version of Regolith.
   */

  // Forward declarations
  class Contact;
  class Collision;

  // Function that produces the interaction class
  bool collides( Drawable*, Drawable*, Contact& );


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Base class for all collision models
  class Collision
  {
    friend bool collides( Drawable*, Drawable*, Contact& );
    private:
      Vector _position;
      float _width;
      float _height;

    public:
      Collision( Vector, float, float );

      virtual ~Collision();

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

  };


}

#endif // __REGOLITH__COLLISION_H__

