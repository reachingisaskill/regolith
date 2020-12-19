
#ifndef REGOLITH_COLLISIONS_COLLISION_H_
#define REGOLITH_COLLISIONS_COLLISION_H_

#include "Regolith/Global/Global.h"

#include <vector>


namespace Regolith
{
  /*
   * Storage classes for the bounding boxes that define collision interactions
   *
   * Each collision object contains multiple sets of hitboxes. At a given time only one set may be active.
   * The collision object inherits its collision team from the owning physical object. Each hit box may be of a
   * different type, however.
   * This is a necessary behaviour to optimise collision detection. The design is similar to that of the texture
   * object. It is designed such that each frame of an animated texture has a specific set of hit boxes that are 
   * active.
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

      // Configure the hitboxes for each frame
      void configure( Json::Value& );


      // Return the total number of frames loaded
      unsigned int getNumberFrames() const { return _collisionFrames.size(); }

      // Return the number of hitboxes in the current frame
      unsigned int getNumberHitBoxes() const { return _collisionFrames[_currentCollision].size(); }


      // If this is an animated collision, update accordingly
      void setFrameNumber( unsigned int frame ) { _currentCollision = frame; }

      // Return the start of the active collision vector
      iterator begin() const { return _collisionFrames[_currentCollision].begin(); }

      // Return the end of the active collision vector
      iterator end() const { return _collisionFrames[_currentCollision].end(); }

  };

}

#endif // REGOLITH_COLLISIONS_COLLISION_H_
