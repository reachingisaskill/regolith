
#ifndef REGOLITH_COLLISION_H_
#define REGOLITH_COLLISION_H_

#include "Regolith/Global/Global.h"

#include <vector>


namespace Regolith
{
  /*
   * Collision is implemented only as axis aligned bounding boxes for the current version of Regolith.
   * 
   * Some assumptions are made. E.g. the side with the shortest overlap is the side at which the overlap
   * happened. This is reasonably good assumption for when one box is much larger than the other, or
   * when the overlap distance is much smaller than the dimensions of the box.
   * I will (eventually) switch to a model which uses incoming delta-velocity to determine the side of 
   * the impact.
   *
   * Each collision object contains multiple sets of hitboxes. At a given time only one set may be active.
   * The collision object inherits its collision team from the owning physical object. Each hit box may be of a
   * different type, however.
   * This is a necessary behaviour to optimise collision detection. The design is similar to that of the texture
   * object. It is designed such that each frame of an animated texture has a specific set of hit boxes that are 
   * active.
   */

  // Forward declarations

  /*
  // Function that produces the contact class
  void collides( Collidable*, Collidable* );

  // Function that returns true if the first object contains the second object
//  bool contains( Collidable*, Collidable* );
  void contains( Collidable*, Collidable* );

  // Function that returns true if the object contains point
  bool contains( Collidable*, const Vector& );
  */


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Structure for holding the description of a single bouding box

  struct HitBox
  {
    // Relative position
    Vector position;
    // Dimensions
    float width;
    float height;

    // The type of the collision
    CollisionType type;

    HitBox() : position(), width( 0.0 ), height( 0.0 ), type( 0 ) {}
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Base class for all collision models

  class Collision
  {
    public:
      typedef std::vector< HitBox > HitBoxVector;
      typedef HitBoxVector::const_iterator iterator;

    private:
      typedef std::vector< HitBoxVector > CollisionFrames;

    private:
      // Store a vector of vector of hit boxes. Only one set is active at time.
      CollisionFrames _collisionFrames;

      // Current active hitboxes
      size_t _currentCollision;

    public:
      Collision();
//      ~Collision();

      void configure( Json::Value& );

      // If this is an animated collision, update accordingly
      void setFrameNumber( unsigned int frame ) { _currentCollision = frame; }

      // Return the start of the active collision vector
      iterator begin() const { return _collisionFrames[_currentCollision].begin(); }

      // Return the end of the active collision vector
      iterator end() const { return _collisionFrames[_currentCollision].end(); }

  };

}

#endif // REGOLITH_COLLISION_H_

