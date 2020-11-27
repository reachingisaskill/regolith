
#include "Regolith/Managers/CollisionHandler.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/GamePlay/ContextLayer.h"


namespace Regolith
{
////////////////////////////////////////////////////////////////////////////////////////////////////
  // Forward declarations


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Collision handler class member functions

  CollisionHandler::CollisionHandler() :
    _teamCollision(),
    _pairings(),
    _containers()
  {
  }


  CollisionHandler::~CollisionHandler()
  {
  }


  void CollisionHandler::addTeamCollision( CollisionTeam team )
  {
    _teamCollision.insert( team );
  }


  void CollisionHandler::addCollisionPair( CollisionTeam team1, CollisionTeam team2 )
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


  void CollisionHandler::addContainerPair( CollisionTeam team1, CollisionTeam team2 )
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
          ERROR_LOG( "Cannot add an inverse contaimment rule." );
          return;
        }
      }
    }

    _containers.push_back( std::make_pair( team1, team2 ) );
  }


  void CollisionHandler::configure( Json::Value& json_data )
  {
    INFO_LOG( "Configuring Collision Handler" );

    Utilities::validateJson( json_data, "team_collision", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJsonArray( json_data["collision_rules"], 0, Utilities::JSON_TYPE_STRING );

    Utilities::validateJson( json_data, "collision_rules", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJsonArray( json_data["collision_rules"], 0, Utilities::JSON_TYPE_ARRAY );

    Utilities::validateJson( json_data, "container_rules", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJsonArray( json_data["container_rules"], 0, Utilities::JSON_TYPE_ARRAY );


    // Load the team collision rules
    Json::Value& team_rules = json_data["team_collision"];
    for ( Json::ArrayIndex i = 0; i < team_rules.size(); ++i )
    {
      std::string team_name = team_rules[i].asString();
      CollisionTeam team = Manager::getInstance()->getCollisionTeam( team_name );

      addTeamCollision( team );
      INFO_STREAM << "Added Team Collision: " << team_name;
    }

    // Load the collision rules
    Json::Value& collision_rules = json_data["collision_rules"];
    for ( Json::ArrayIndex i = 0; i < collision_rules.size(); ++i )
    {
      Utilities::validateJsonArray( collision_rules[i], 2, Utilities::JSON_TYPE_STRING );

      std::string team_name1 = collision_rules[i][0].asString();
      std::string team_name2 = collision_rules[i][1].asString();

      CollisionTeam team1 = Manager::getInstance()->getCollisionTeam( team_name1 );
      CollisionTeam team2 = Manager::getInstance()->getCollisionTeam( team_name2 );

      addCollisionPair( team1, team2 );
      INFO_STREAM << "Added Collision Rule: " << team_name1 << " vs " << team_name2;
    }

    // Load the containment rules
    Json::Value& container_rules = json_data["container_rules"];
    for ( Json::ArrayIndex i = 0; i < container_rules.size(); ++i )
    {
      Utilities::validateJsonArray( container_rules[i], 2, Utilities::JSON_TYPE_STRING );

      std::string team_name1 = container_rules[i][0].asString();
      std::string team_name2 = container_rules[i][1].asString();

      CollisionTeam team1 = Manager::getInstance()->getCollisionTeam( team_name1 );
      CollisionTeam team2 = Manager::getInstance()->getCollisionTeam( team_name2 );

      addContainerPair( team1, team2 );
      INFO_STREAM << "Added Container Rule: " << team_name1 << " <- " << team_name2;
    }
  }


  void CollisionHandler::setupEmptyLayer( ContextLayer& layer ) const
  {
    for ( CollisionPairList::const_iterator it = _pairings.begin(); it != _pairings.end(); ++it )
    {
      if ( layer.layerGraph.find( it->first ) == layer.layerGraph.end() )
      {
        layer.layerGraph[ it->first ] = PhysicalObjectList();
      }
      if ( layer.layerGraph.find( it->second ) == layer.layerGraph.end() )
      {
        layer.layerGraph[ it->second ] = PhysicalObjectList();
      }
    }

    for ( CollisionPairList::const_iterator it = _containers.begin(); it != _containers.end(); ++it )
    {
      if ( layer.layerGraph.find( it->first ) == layer.layerGraph.end() )
      {
        layer.layerGraph[ it->first ] = PhysicalObjectList();
      }
      if ( layer.layerGraph.find( it->second ) == layer.layerGraph.end() )
      {
        layer.layerGraph[ it->second ] = PhysicalObjectList();
      }
    }
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Collision algorithm

  void CollisionHandler::collides( PhysicalObject* object1, PhysicalObject* object2 )
  {
    DEBUG_LOG( "CollisionHandler::collides : Checking Collision" );

    _object_pos1 = object1->position();
    _object_pos2 = object2->position();


    // Step 1 : Do the bounding boxes overlap

    if ( ( _object_pos1.x() < ( _object_pos2.x() + object2->getWidth() ) ) &&
         ( ( _object_pos1.x() + object1->getWidth() ) > _object_pos2.x() ) &&
         ( _object_pos1.y() < ( _object_pos2.y() + object2->getHeight() ) ) &&
         ( ( _object_pos1.x() + object1->getHeight() ) > _object_pos2.x() ) )
    {

      const Collision& collision1 = object1->getCollision();
      const Collision& collision2 = object2->getCollision();

      for ( Collision::iterator col_it1 = collision1.begin(); col_it1 != collision1.end(); ++col_it1 )
      {
        // Absolute _positions of the hit boxes
        _pos1 = _object_pos1 + col_it1->position;

        for ( Collision::iterator col_it2 = collision2.begin(); col_it2 != collision2.end(); ++col_it2 )
        {
          // Absolute _positions of the hit boxes
          _pos2 = _object_pos2 + col_it2->position;

          // X Axis
          _diff_x = _pos2.x() - _pos1.x();

          ////////////////////////////////////////////////// 
          // IF OBJ2 IN FRONT OBJ1 - X
          if ( _diff_x > 0.0 ) 
          {
            _overlap_x = col_it1->width - _diff_x;
            DEBUG_STREAM << "CollisionHandler::collides :  Diff_x = " << _diff_x << ", Overlap = " << _overlap_x;

            // If hitboxes overlap in x
            if ( _overlap_x > 0.0 )
            {
              // Y Axis
              _diff_y = _pos2.y() - _pos1.y();

              ////////////////////////////////////////////////// 
              // IF OBJ2 IN FRONT OBJ1 - Y
              if ( _diff_y >= 0.0 )
              {
                _overlap_y = col_it1->height - _diff_y;
                DEBUG_STREAM << "CollisionHandler::collides : Diff_y = " << _diff_y << ", Overlap = " << _overlap_y;

                // If hitboxes overlap in y
                if ( _overlap_y > 0.0 )
                {
                  _type1 = col_it1->type;
                  _type2 = col_it2->type;

                  // Both X & Y overlap - set the reference contact object
                  if ( _overlap_x >= _overlap_y )
                  {
                    _contact_vector.set( 0.0, _overlap_y );
                    callback( object1, object2 );
                  }
                  else
                  {
                    _contact_vector.set( _overlap_x, 0.0 );
                    callback( object1, object2 );
                  }
                }
              }
              ////////////////////////////////////////////////// 
              // IF OBJ2 BEHIND OBJ1 - Y
              else
              {
                _overlap_y = col_it2->height + _diff_y;
                DEBUG_STREAM << "CollisionHandler::collides : Diff_y = " << _diff_y << ", Overlap = " << _overlap_y;

                // If hitboxes overlap in y
                if ( _overlap_y > 0.0 )
                {
                  _type1 = col_it1->type;
                  _type2 = col_it2->type;

                  // Both X & Y overlap - set the reference contact object
                  if ( _overlap_x >= _overlap_y )
                  {
                    _contact_vector.set( 0.0, -_overlap_y );
                    callback( object1, object2 );
                  }
                  else
                  {
                    _contact_vector.set( _overlap_x, 0.0 );
                    callback( object1, object2 );
                  }
                }
              }
            }
          }
          ////////////////////////////////////////////////// 
          // IF OBJ2 BEHIND OBJ1 - X
          else
          {
            _overlap_x = col_it2->width + _diff_x;
            DEBUG_STREAM << "CollisionHandler::collides : Diff_x = " << _diff_x << ", Overlap = " << _overlap_x;

            // If hitboxes overlap in x
            if ( _overlap_x > 0.0 )
            {
              // Y Axis
              _diff_y = _pos2.y() - _pos1.y();
              ////////////////////////////////////////////////// 
              // IF OBJ2 IN FRONT OBJ1 - Y
              if ( _diff_y >= 0.0 )
              {
                _overlap_y = col_it1->height - _diff_y;
                DEBUG_STREAM << "CollisionHandler::collides : Diff_y = " << _diff_y << ", Overlap = " << _overlap_y;

                // If hitboxes overlap in y
                if ( _overlap_y > 0.0 )
                {
                  _type1 = col_it1->type;
                  _type2 = col_it2->type;

                  // Both X & Y overlap - set the reference contact object
                  if ( _overlap_x >= _overlap_y )
                  {
                    _contact_vector.set( 0.0, _overlap_y );
                    callback( object1, object2 );
                  }
                  else
                  {
                    _contact_vector.set( -_overlap_x, 0.0 );
                    callback( object1, object2 );
                  }
                }
              }
              ////////////////////////////////////////////////// 
              // IF OBJ2 BEHIND OBJ1 - Y
              else
              {
                _overlap_y = col_it2->height + _diff_y;
                DEBUG_STREAM << "CollisionHandler::collides : Diff_y = " << _diff_y << ", Overlap = " << _overlap_y;

                // If hitboxes overlap in y
                if ( _overlap_y > 0.0 )
                {
                  _type1 = col_it1->type;
                  _type2 = col_it2->type;

                  // Both X & Y overlap - set the reference contact object
                  if ( _overlap_x >= _overlap_y )
                  {
                    _contact_vector.set( 0.0, -_overlap_y );
                    callback( object1, object2 );
                  }
                  else
                  {
                    _contact_vector.set( -_overlap_x, 0.0 );
                    callback( object1, object2 );
                  }
                }
              }
            }
          }
        }
      }
    }
  }


  void CollisionHandler::callback( PhysicalObject* object1, PhysicalObject* object2 )
  {
    if ( object1->hasMovement() )
    {
      if ( object2->hasMovement() )
      {
        // If both objects can move we weight the contact vector by the inverse of their masses
        _invM1 = object1->getInverseMass();
        _invM2 = object2->getInverseMass();
        _total_invM = _invM1 + _invM2;

        object1->onCollision( -(_invM1/_total_invM)*_contact_vector, _type1, _type2, object2 );
        object2->onCollision(  (_invM2/_total_invM)*_contact_vector, _type2, _type1, object1 );
      }
      else
      {
        object1->onCollision( -_contact_vector, _type1, _type2, object2 );
        object2->onCollision(       zeroVector, _type2, _type1, object1 );
      }
    }
    else
    {
      if ( object2->hasMovement() )
      {
        object1->onCollision(      zeroVector, _type1, _type2, object2 );
        object2->onCollision( _contact_vector, _type2, _type1, object1 );
      }
      else
      {
        object1->onCollision( zeroVector, _type1, _type2, object2 );
        object2->onCollision( zeroVector, _type2, _type1, object1 );
      }
    }

  }

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Collides functions


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Contains function


  /*
  bool contains( Collidable* parent, Collidable* child )
  {
    DEBUG_LOG( "Checking Containment" );

    const Collision& parent_coll = parent->getCollision();
    const Collision& child_coll = child->getCollision();

    Vector parent_pos;
    Vector child_pos;

    parent_pos = parent->position() + parent_coll.position(); // Move into global coordinate system
    DEBUG_STREAM << " Global Pos1 : " << parent_pos << " W = " << parent_coll.width() << " H = " << parent_coll.height();

    child_pos = child->position() + child_coll.position(); // Move into global coordinate system
    DEBUG_STREAM << "   Global Pos2 : " << child_pos << " W = " << child_coll.width() << " H = " << child_coll.height();


    // X Axis
    float diff_x = child_pos.x() - parent_pos.x();
    if ( ( diff_x > -child_coll.width() ) && ( diff_x < parent_coll.width() ) ) 
    {
      // Y Axis
      float diff_y = child_pos.y() - parent_pos.y();
      if ( ( diff_y > child_coll.height() ) && ( diff_y < parent_coll.height() ) )
      {
        return true;
      }
    }

    return false;
  }
  */



//  void contains( Collidable* parent, Collidable* child )
//  {
//    DEBUG_LOG( "Checking Containment" );
//
//    const Collision& parent_coll = parent->getCollision();
//    const Collision& child_coll = child->getCollision();
//
//    Vector parent_pos;
//    Vector child_pos;
//
//    parent_pos = parent->position() + parent_coll.position(); // Move into global coordinate system
//    DEBUG_STREAM << " Global Pos1 : " << parent_pos << " W = " << parent_coll.width() << " H = " << parent_coll.height();
//
//    child_pos = child->position() + child_coll.position(); // Move into global coordinate system
//    DEBUG_STREAM << " Global Pos2 : " << child_pos << " W = " << child_coll.width() << " H = " << child_coll.height();
//
//
//    // X Axis
//    float diff_x = child_pos.x() - parent_pos.x();
//    float diff_y = child_pos.y() - parent_pos.y();
//
//    if ( ( diff_x >= 0.0 ) && ( ( diff_x + child_coll.width()) <= parent_coll.width() ) )
//      if ( ( diff_y >= 0.0 ) && ( ( diff_y + child_coll.height()) <= parent_coll.height() ) )
//        return; // Contained
//
//    if ( diff_x > 0.0 )
//    {
//      diff_x += child_coll.width() - parent_coll.width();
//      if ( diff_x < 0.0 ) diff_x = 0.0;
//    }
//
//    if ( diff_y > 0.0 )
//    {
//      diff_y += child_coll.height() - parent_coll.height();
//      if ( diff_y < 0.0 ) diff_y = 0.0;
//    }
//
//    Vector normal( diff_x, diff_y );
//    float length = normal.mod();
//
//    DEBUG_STREAM << "DIFF_X = " << diff_x << " ,  DIFF_Y = " << diff_y << " L = " << length;
//
//
//    callback( child, parent, (normal /= length), length );
//  }


//  bool contains( ContextLayer& layer, Collidable* object )
//  {
//    DEBUG_LOG( "Checking Layer Containement" );
//
//    const Collision& object_coll = object->getCollision();
//
//    const Vector& layer_pos = layer.getPosition();
//    Vector object_pos;
//
//    object_pos = object->position() + object_coll.position(); // Move into global coordinate system
//    DEBUG_STREAM << "   Global Pos2 : " << object_pos << " W = " << object_coll.width() << " H = " << object_coll.height();
//
//
//    // X Axis
//    float diff_x = object_pos.x() - layer_pos.x();
//    if ( ( diff_x > -object_coll.width() ) && ( diff_x < layer.getWidth() ) ) 
//    {
//      // Y Axis
//      float diff_y = object_pos.y() - layer_pos.y();
//      if ( ( diff_y > object_coll.height() ) && ( diff_y < layer.getHeight() ) )
//      {
//        return true;
//      }
//    }
//
//    return false;
//  }


//  bool contains( Collidable* object, const Vector& point )
//  {
//    DEBUG_LOG( "Checking Point Containment" );
//
//    const Collision& collision = object->getCollision();
//    Vector pos = object->position() + collision.position(); // Move into global coordinate system
//    DEBUG_STREAM << " Global Pos1 : " << pos << " W = " << collision.width() << " H = " << collision.height();
//
//    // X Axis
//    float diff_x = point.x() - pos.x();
//    if ( ( diff_x > 0.0 ) && ( diff_x < collision.width() ) )
//    {
//      float diff_y = point.y() - pos.y();
//      if ( ( diff_y > 0.0 ) && ( diff_y < collision.height() ) ) 
//      {
//        return true;
//      }
//    }
//
//    return false;
//  }

}

