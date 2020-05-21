
#ifndef REGOLITH_COLLISION_HANDLER_H_
#define REGOLITH_COLLISION_HANDLER_H_

#include "Regolith/Global/Global.h"

#include <vector>
#include <utility>

namespace Regolith
{

  /*
   * Stores a lookup to determine which teams are allowed to collide with which other teams.
   * Caching the data like this means that we need to do few comparisons at run time, and each context is highly configurable.
   * Each context will store a unique instance of this class, and therefore does not need to care about all the teams.
   */
  class CollisionHandler
  {
    typedef std::vector< std::pair< TeamID, TeamID > > CollisionPairList;

    private:
      CollisionPairList _pairings;

    public:
      // No changing!
      typedef CollisionPairList::const_iterator iterator;

      // Con/De-structors
      CollisionHandler();
      virtual ~CollisionHandler();

      // Configure the pairings itself from the json data
      void configure( Json::Value& );

      // Manually add a pairing
      void addCollisionPair( TeamID, TeamID );

      // Basic iterator interface
      iterator begin() const { return _pairings.begin(); }
      iterator end() const { return _pairings.end(); }
  };

}

#endif // REGOLITH_COLLISION_HANDLER_H_

