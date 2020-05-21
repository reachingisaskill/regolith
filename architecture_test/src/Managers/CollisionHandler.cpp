
#include "Regolith/Managers/CollisionHandler.h"
#include "Regolith/Managers/Manager.h"

#include "logtastic.h"


namespace Regolith
{

  CollisionHandler::CollisionHandler() :
    _pairings()
  {
  }


  CollisionHandler::~CollisionHandler()
  {
  }


  void CollisionHandler::addCollisionPair( TeamID team1, TeamID team2 )
  {
    CollisionPairList::iterator end = _pairings.end();
    for ( CollisionPairList::iterator it = _pairings.begin(); it != end; ++it )
    {
      if ( it->first == team1 )
      {
        if ( it->second == team2 )
        {
          WARN_LOG( "Attempting to add existing team collision pairing twice" );
          return;
        }
      }
      else if ( it->first == team2 )
      {
        if ( it->second == team1 )
        {
          WARN_LOG( "Attempting to add existing team collision pairing twice" );
          return;
        }
      }
    }

    _pairings.push_back( std::make_pair( team1, team2 ) );
  }


  void CollisionHandler::configure( Json::Value& json_data )
  {
    INFO_LOG( "Configuring Collision Handler" );

    Utilities::validateJson( json_data, "collision_rules", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJsonArray( json_data["collision_rules"], 0, Utilities::JSON_TYPE_ARRAY );

    Json::Value& collision_rules = json_data["collision_rules"];

    for ( Json::ArrayIndex i = 0; i < collision_rules.size(); ++i )
    {
      Utilities::validateJsonArray( collision_rules[i], 2, Utilities::JSON_TYPE_STRING );

      std::string team_name1 = collision_rules[i][0].asString();
      std::string team_name2 = collision_rules[i][1].asString();

      TeamID team1 = Manager::getInstance()->getTeamID( team_name1 );
      TeamID team2 = Manager::getInstance()->getTeamID( team_name2 );

      addCollisionPair( team1, team2 );
    }



    Utilities::validateJson( json_data, "container_rules", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJsonArray( json_data["container_rules"], 0, Utilities::JSON_TYPE_ARRAY );

    Json::Value& container_rules = json_data["container_rules"];

    for ( Json::ArrayIndex i = 0; i < container_rules.size(); ++i )
    {
      Utilities::validateJsonArray( container_rules[i], 2, Utilities::JSON_TYPE_STRING );

      std::string team_name1 = container_rules[i][0].asString();
      std::string team_name2 = container_rules[i][1].asString();

      TeamID team1 = Manager::getInstance()->getTeamID( team_name1 );
      TeamID team2 = Manager::getInstance()->getTeamID( team_name2 );

      addCollisionPair( team1, team2 );
    }
  }

}

