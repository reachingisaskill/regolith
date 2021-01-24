
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

//  // Enumarate the types of hit box for optimizations
//  //  Polygon - Has a list of points and normals of the same number describing the number of vertices and edges in clockwise order
//  //  Coming soon: Circle - Store center point. Radius is the x-value of the second point
//  enum class HitBoxType { Circle, Polygon };


  // Struct holding the info required to perform collision detection using the separating axis theorem
  struct HitBox
  {
    // Vertex positions
    std::vector< Vector > points;

    // Edge normals
    std::vector< Vector > normals;

    // Number of points/edges
    unsigned int number;

    // The type of the collision
    CollisionType collisionType;

//    // Type of hitbox to allow calculation optimization - NOT USED CURRENTLY. EVERYTHING IS A POLYGON!
//    HitBoxType shape;
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Base class for all collision models

  class Collision
  {
    public:
      typedef std::vector< HitBox > HitBoxVector;
      typedef HitBoxVector::const_iterator iterator;

    protected:
      typedef std::vector< HitBoxVector > Frames;

    private:

    public:
      // Con/Destruction
      Collision() {}
      virtual ~Collision() {}


      // Configure the hitboxes
      virtual void configure( Json::Value& ) = 0;


      // Return the number of hitboxes in the current frame
      virtual size_t size() const = 0;


      // Return the start of the collision vector
      virtual iterator begin() const = 0;

      // Return the end of the collision vector
      virtual iterator end() const = 0;

  };

}

#endif // REGOLITH_COLLISIONS_COLLISION_H_

