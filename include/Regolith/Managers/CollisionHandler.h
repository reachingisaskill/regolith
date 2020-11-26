
#ifndef REGOLITH_COLLISION_HANDLER_H_
#define REGOLITH_COLLISION_HANDLER_H_

#include "Regolith/Global/Global.h"

#include <vector>
#include <utility>

namespace Regolith
{

  /*
   * Collision is implemented only as axis aligned bounding boxes for the current version of Regolith.
   *
   * Stores a lookup to determine which teams are allowed to collide with which other teams.
   * Caching the data like this means that we need to do few comparisons at run time, and each context is highly configurable.
   * Each context will store a unique instance of this class, and therefore does not need to care about all the teams.

   * Also provides the functions for comparing the collision of physical objects and triggering the callback events
   * 
   * Some assumptions are made. E.g. the side with the shortest overlap is the side at which the overlap
   * happened. This is reasonably good assumption for when one box is much larger than the other, or
   * when the overlap distance is much smaller than the dimensions of the box.
   * I will (eventually) switch to a model which uses incoming delta-velocity to determine the side of 
   * the impact.
   */

  // Forward declarations
  class PhysicalObject;
  class ContextLayer;


  // Collision handler definition
  class CollisionHandler
  {
    typedef std::vector< std::pair< CollisionTeam, CollisionTeam > > CollisionPairList;

    private:
      CollisionPairList _pairings;
      CollisionPairList _containers;

      // These variables are stored as class memebers to reduce the size of the stack frame.
      // Should (slightly!) speed up the collision detection process
      Vector _object_pos1;
      Vector _object_pos2;
      Vector _pos1;
      Vector _pos2;
      float _overlap_x;
      float _overlap_y;
      float _diff_x;
      float _diff_y;

      Vector _contact_vector;
      CollisionType _type1;
      CollisionType _type2;
      float _invM1;
      float _invM2;
      float _total_invM;

    protected:
      // Used to handle the function calls the two objects that have collided.
      inline void callback( PhysicalObject*, PhysicalObject* );

    public:
      // No changing!
      typedef CollisionPairList::const_iterator iterator;

      // Con/De-structors
      CollisionHandler();
      virtual ~CollisionHandler();

      // Configure the pairings itself from the json data
      void configure( Json::Value& );

      // Manually add a pairing
      void addCollisionPair( CollisionTeam, CollisionTeam );

      // Manually add a containment pairing
      void addContainerPair( CollisionTeam, CollisionTeam );

      // Configures an empty layer
      void setupEmptyLayer( ContextLayer& ) const;

      // Basic iterator interface
      iterator collisionBegin() const { return _pairings.begin(); }
      iterator collisionEnd() const { return _pairings.end(); }

      // Basic iterator interface
      iterator containerBegin() const { return _containers.begin(); }
      iterator containerEnd() const { return _containers.end(); }

      // Do two object collide
      void collides( PhysicalObject*, PhysicalObject* );
      // Does the first object contain the second
      void contains( PhysicalObject*, PhysicalObject* ) {}

  };


}

#endif // REGOLITH_COLLISION_HANDLER_H_

