
#ifndef REGOLITH_MANAGERS_COLLISION_MANAGER_H_
#define REGOLITH_MANAGERS_COLLISION_MANAGER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Links/Link.h"


namespace Regolith
{

  class CollisionManager
  {
    // Allow links to access the private members
    template < class T, class R > friend class Link;

    private:
      TeamNameMap _teamNames;
      TypeNameMap _typeNames;

    protected:

    public:
      CollisionManager();
      ~CollisionManager();

      // Set up the collision teams and types.
      void configure( Json::Value& );

      // Clear all the internal lists
      void clear();


////////////////////////////////////////////////////////////////////////////////
      // Teams and types accessors and modifiers

      // Forcibly add a collision team
      void addCollisionTeam( std::string name, CollisionTeam id ) { _teamNames[ name ] = id; }

      // Return the number of teams
      size_t getNumberTeams() const { return _teamNames.size(); }

      // Return the team ID for a given name
      CollisionTeam getCollisionTeam( std::string name );


      // Forcibly add a collision type
      void addCollisionType( std::string name, CollisionType id ) { _typeNames[ name ] = id; }

      // Return the number of teams
      size_t getNumberTypes() const { return _typeNames.size(); }

      // Return the type ID for a given name
      CollisionType getCollisionType( std::string name );

  };

}

#endif // REGOLITH_MANAGERS_COLLISION_MANAGER_H_

