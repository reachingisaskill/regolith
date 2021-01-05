
#include "Regolith/Links/Link.h"
#include "Regolith/Managers/Manager.h"


namespace Regolith
{

  // Prevent partial specialisations from passing
  template < class REQUESTER >
  class Link< Manager, REQUESTER >
  {
    struct FalseType : public std::false_type {};
    static_assert( FalseType::value, "A link is not permitted between the requested classes" );
  };


  
  template <>
  class Link< Manager, TestType >
  {
    private:

      Manager* _manager;

    public:

      Link( Manager* m ) : _manager( m ) {}

      void addCollisionTeam( std::string name, CollisionTeam id ) { _manager->addCollisionTeam( name, id ); }
      void addCollisionType( std::string name, CollisionType id ) { _manager->addCollisionType( name, id ); }

  };
}

