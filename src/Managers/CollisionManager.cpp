
#include "Regolith/Managers/CollisionManager.h"


namespace Regolith
{

  CollisionManager::CollisionManager() :
    _teamNames(),
    _typeNames()
  {
  }


  CollisionManager::~CollisionManager()
  {
  }



  void CollisionManager::configure( Json::Value& json_data )
  {
    INFO_LOG( "CollisionManager::configure : Configuring global collision information" );

    Json::Value& team_data = json_data["collision_teams"];
    for ( Json::Value::const_iterator it = team_data.begin(); it != team_data.end(); ++it )
    {
      std::string team_name = it.key().asString();
      CollisionTeam id = (CollisionTeam) it->asInt();
      _teamNames[ team_name ] = id;
    }
    INFO_STREAM << "CollisionManager::configure : Created " << _teamNames.size() << " collision teams.";

    Json::Value& type_data = json_data["collision_types"];
    for ( Json::Value::const_iterator it = type_data.begin(); it != type_data.end(); ++it )
    {
      std::string type_name = it.key().asString();
      CollisionType id = (CollisionType) it->asInt();
      _typeNames[ type_name ] = id;
    }
    INFO_STREAM << "CollisionManager::configure : Created " << _typeNames.size() << " collision types.";
  }


  void CollisionManager::clear()
  {
    _teamNames.clear();
    _typeNames.clear();
  }


  CollisionTeam CollisionManager::getCollisionTeam( std::string name )
  {
    TeamNameMap::iterator found = _teamNames.find( name );
    if ( found == _teamNames.end() )
    {
      Exception ex( "Manager::getCollisionTeam()", "Could not find requested team name." );
      ex.addDetail( "Team Name", name );
      throw ex;
    }
    return found->second;
  }


  CollisionType CollisionManager::getCollisionType( std::string name )
  {
    TypeNameMap::iterator found = _typeNames.find( name );
    if ( found == _typeNames.end() )
    {
      Exception ex( "Manager::getCollisionType()", "Could not find requested type name." );
      ex.addDetail( "Type Name", name );
      throw ex;
    }
    return found->second;
  }

}

