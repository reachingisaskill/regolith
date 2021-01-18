
#ifndef REGOLITH_COLLISION_HANDLER_H_
#define REGOLITH_COLLISION_HANDLER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/ObjectInterfaces/CollidableObject.h"

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


////////////////////////////////////////////////////////////////////////////////
  // Forward declarations
  class PhysicalObject;
  class ContextLayer;


////////////////////////////////////////////////////////////////////////////////
  // Collision handler definition
  class CollisionHandler
  {
    typedef std::vector< std::pair< CollisionTeam, CollisionTeam > > CollisionPairList;
    typedef std::set< CollisionTeam > CollisionSet;

    private:
      CollisionSet _teamCollision;
      CollisionPairList _pairings;
      CollisionPairList _containers;

      // These variables are stored as class memebers to reduce the size of the stack frame.
      // Should (slightly!) speed up the collision detection process
      Vector _object_pos1;
      Vector _object_pos2;

      Vector _point1;
      Vector _point2;

      Vector _normal1;
      Vector _normal2;

      float _projection1;
      float _projection2;

      Vector _overlapping_point;

      float _largest_overlap;
      float _diff;

      float _total_M;
      float _total_L;
      float _coef_restitution;

      Contact _contact1;
      Contact _contact2;

    protected:
      // Used to handle the function calls the two objects that have collided.
      inline void callback( CollidableObject*, CollidableObject* );

    public:
      // Const iterators - no changing!
      typedef CollisionSet::const_iterator SetIterator;
      typedef CollisionPairList::const_iterator PairIterator;

      // Con/De-structors
      CollisionHandler();
      virtual ~CollisionHandler();

      // Delete the copy/move constructors. Can't be arsed to implement them and we don't need them.
      CollisionHandler( const CollisionHandler& ) = delete;
      CollisionHandler( CollisionHandler&& ) = delete;
      CollisionHandler& operator=( const CollisionHandler& ) = delete;
      CollisionHandler& operator=( CollisionHandler&& ) = delete;


      // Configure the pairings itself from the json data
      void configure( Json::Value& );

      // Manually add a team collision
      void addTeamCollision( CollisionTeam );

      // Manually add a pairing
      void addCollisionPair( CollisionTeam, CollisionTeam );

      // Manually add a containment pairing
      void addContainerPair( CollisionTeam, CollisionTeam );

      // Configures an empty layer
      void setupEmptyLayer( ContextLayer& ) const;

      // Basic iterator interface
      SetIterator teamCollisionBegin() const { return _teamCollision.begin(); }
      SetIterator teamCollisionEnd() const { return _teamCollision.end(); }

      // Basic iterator interface
      PairIterator collisionBegin() const { return _pairings.begin(); }
      PairIterator collisionEnd() const { return _pairings.end(); }

      // Basic iterator interface
      PairIterator containerBegin() const { return _containers.begin(); }
      PairIterator containerEnd() const { return _containers.end(); }

      // Do two object collide
      void collides( CollidableObject*, CollidableObject* );
      // Does the first object contain the second
      void contains( CollidableObject*, CollidableObject* );

  };


}

#endif // REGOLITH_COLLISION_HANDLER_H_

