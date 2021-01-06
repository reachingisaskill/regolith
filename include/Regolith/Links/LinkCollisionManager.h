
#ifndef REGOLITH_LINKS_LINK_COLLISION_MANAGER_H_
#define REGOLITH_LINKS_LINK_COLLISION_MANAGER_H_

#include "Regolith/Links/Link.h"
#include "Regolith/Managers/CollisionManager.h"


namespace Regolith
{

  // Prevent partial specialisations from passing
  template < class REQUESTER >
  class Link< CollisionManager, REQUESTER >
  {
    struct FalseType : public std::false_type {};
    static_assert( FalseType::value, "A link is not permitted between the requested class and the collision manager" );
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Rendering thread access
  template <>
  class Link< CollisionManager, TestType >
  {
    private:

      CollisionManager& _manager;

    public:

      Link( ContextManager& m ) : _manager( m ) {}

      void addCollisionTeam( std::string name, CollisionTeam id ) { _manager.addCollisionTeam( name, id ); }
      void addCollisionType( std::string name, CollisionType id ) { _manager.addCollisionType( name, id ); }
  };
}

#endif // REGOLITH_LINKS_LINK_COLLISION_MANAGER_H_

