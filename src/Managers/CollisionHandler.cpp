
#include "Regolith/Managers/CollisionHandler.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/ObjectInterfaces/CollidableObject.h"
#include "Regolith/Contexts/ContextLayer.h"


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
    _contact1.other = &_contact2;
    _contact2.other = &_contact1;
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

  void CollisionHandler::collides( CollidableObject* object1, CollidableObject* object2 )
  {
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
          if ( _diff_x >= 0.0 ) 
          {
            _overlap_x = col_it1->width - _diff_x;
//            DEBUG_STREAM << "CollisionHandler::collides : Diff_x = " << _diff_x << ", Overlap = " << _overlap_x;

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
//                DEBUG_STREAM << "CollisionHandler::collides : Diff_y = " << _diff_y << ", Overlap = " << _overlap_y;

                // If hitboxes overlap in y
                if ( _overlap_y > 0.0 )
                {
                  if ( _overlap_x > 0.5*col_it1->width || _overlap_x > 0.5*col_it2->width )
                  {
                    _overlap_x = 0.0;
                  }
                  else if ( _overlap_y > 0.5*col_it1->height || _overlap_y > 0.5*col_it2->height )
                  {
                    _overlap_y = 0.0;
                  }

                  _contact1.type = col_it1->type;
                  _contact2.type = col_it2->type;
                  _contact1.overlap.set( -_overlap_x, -_overlap_y );
                  _contact2.overlap.set(  _overlap_x,  _overlap_y );
                  _contact1.normal = _contact1.overlap.norm();
                  _contact2.normal = _contact2.overlap.norm();
                  callback( object1, object2 );
                }
              }
              ////////////////////////////////////////////////// 
              // IF OBJ2 BEHIND OBJ1 - Y
              else
              {
                _overlap_y = col_it2->height + _diff_y;
//                DEBUG_STREAM << "CollisionHandler::collides : Diff_y = " << _diff_y << ", Overlap = " << _overlap_y;

                // If hitboxes overlap in y
                if ( _overlap_y > 0.0 )
                {
                  if ( _overlap_x > 0.5*col_it1->width || _overlap_x > 0.5*col_it2->width )
                  {
                    _overlap_x = 0.0;
                  }
                  else if ( _overlap_y > 0.5*col_it1->height || _overlap_y > 0.5*col_it2->height )
                  {
                    _overlap_y = 0.0;
                  }

                  _contact1.type = col_it1->type;
                  _contact2.type = col_it2->type;
                  _contact1.overlap.set( -_overlap_x,  _overlap_y );
                  _contact2.overlap.set(  _overlap_x, -_overlap_y );
                  _contact1.normal = _contact1.overlap.norm();
                  _contact2.normal = _contact2.overlap.norm();
                  callback( object1, object2 );
                }
              }
            }
          }
          ////////////////////////////////////////////////// 
          // IF OBJ2 BEHIND OBJ1 - X
          else
          {
            _overlap_x = col_it2->width + _diff_x;
//            DEBUG_STREAM << "CollisionHandler::collides : Diff_x = " << _diff_x << ", Overlap = " << _overlap_x;

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
//                DEBUG_STREAM << "CollisionHandler::collides : Diff_y = " << _diff_y << ", Overlap = " << _overlap_y;

                // If hitboxes overlap in y
                if ( _overlap_y > 0.0 )
                {
                  if ( _overlap_x > 0.5*col_it1->width || _overlap_x > 0.5*col_it2->width )
                  {
                    _overlap_x = 0.0;
                  }
                  else if ( _overlap_y > 0.5*col_it1->height || _overlap_y > 0.5*col_it2->height )
                  {
                    _overlap_y = 0.0;
                  }

                  _contact1.type = col_it1->type;
                  _contact2.type = col_it2->type;
                  _contact1.overlap.set(  _overlap_x, -_overlap_y );
                  _contact2.overlap.set( -_overlap_x,  _overlap_y );
                  _contact1.normal = _contact1.overlap.norm();
                  _contact2.normal = _contact2.overlap.norm();
                  callback( object1, object2 );
                }
              }
              ////////////////////////////////////////////////// 
              // IF OBJ2 BEHIND OBJ1 - Y
              else
              {
                _overlap_y = col_it2->height + _diff_y;
//                DEBUG_STREAM << "CollisionHandler::collides : Diff_y = " << _diff_y << ", Overlap = " << _overlap_y;

                // If hitboxes overlap in y
                if ( _overlap_y > 0.0 )
                {
                  if ( _overlap_x > 0.5*col_it1->width || _overlap_x > 0.5*col_it2->width )
                  {
                    _overlap_x = 0.0;
                  }
                  else if ( _overlap_y > 0.5*col_it1->height || _overlap_y > 0.5*col_it2->height )
                  {
                    _overlap_y = 0.0;
                  }

                  _contact1.type = col_it1->type;
                  _contact2.type = col_it2->type;
                  _contact1.overlap.set(  _overlap_x,  _overlap_y );
                  _contact2.overlap.set( -_overlap_x, -_overlap_y );
                  _contact1.normal = _contact1.overlap.norm();
                  _contact2.normal = _contact2.overlap.norm();
                  callback( object1, object2 );
                }
              }
            }
          }
        }
      }
    }
  }


  void CollisionHandler::contains( CollidableObject* object1, CollidableObject* object2 )
  {
//    DEBUG_LOG( "CollisionHandler::contains : Checking containment" );

    _object_pos1 = object1->position();
    _object_pos2 = object2->position();

    const Collision& collision1 = object1->getCollision();
    const Collision& collision2 = object2->getCollision();

    for ( Collision::iterator col_it1 = collision1.begin(); col_it1 != collision1.end(); ++col_it1 )
    {
      // Position of a container hitbox
      _pos1 = _object_pos1 + col_it1->position;

      // Is the bounding box not contained within the hitbox
      if (  _pos1.x() > _object_pos2.x() ||
            _pos1.y() > _object_pos2.y() ||
            ( _pos1.x() + col_it1->width ) < ( _object_pos2.x() + object2->getWidth() ) ||
            ( _pos1.y() + col_it1->height ) < ( _object_pos2.y() + object2->getHeight() ) )
      {

        // See which hit box is outside the container
        for ( Collision::iterator col_it2 = collision2.begin(); col_it2 != collision2.end(); ++col_it2 )
        {
          // Position of a container hitbox
          _pos2 = _object_pos2 + col_it2->position;
//          DEBUG_STREAM << "CollisionHandler::contains : Pos 1 = " << _pos1 << " Pos = " << _pos2;
    
          _diff_x = _pos2.x() - _pos1.x();
          _diff_y = _pos2.y() - _pos1.y();
          _overlap_x = ( _pos2.x() + col_it2->width ) - ( _pos1.x() + col_it1->width );
          _overlap_y = ( _pos2.y() + col_it2->height ) - ( _pos1.y() + col_it1->height );
//          DEBUG_STREAM << "CollisionHandler::contains : Diff = " << _diff_x << ", " << _diff_y << "   Overlap = " << _overlap_x << ", " << _overlap_y;

          if ( _diff_x < 0.0 ) // Behind container position
          {
            if ( _diff_y < 0.0 )
            {
              _contact1.overlap.set(  _diff_x,  _diff_y );
              _contact2.overlap.set( -_diff_x, -_diff_y );
            }
            else if ( _overlap_y > 0.0 )
            {
              _contact1.overlap.set(  _diff_x,  _overlap_y );
              _contact2.overlap.set( -_diff_x, -_overlap_y );
            }
            else
            {
              _contact1.overlap.set(  _diff_x, 0.0 );
              _contact2.overlap.set( -_diff_x, 0.0 );
            }
            _contact1.type = col_it1->type;
            _contact2.type = col_it2->type;
            _contact1.normal = _contact1.overlap.norm();
            _contact2.normal = _contact2.overlap.norm();
//            DEBUG_STREAM << "CollisionHandler::contains : Overlap 1 = " << _contact1.overlap <<  " -- Normal 1 = " << _contact1.normal;
//            DEBUG_STREAM << "CollisionHandler::contains : Overlap 2 = " << _contact2.overlap <<  " -- Normal 2 = " << _contact2.normal;
            callback( object1, object2 );
          }
          else if ( _overlap_x > 0.0 )
          {
            if ( _diff_y < 0.0 )
            {
              _contact1.overlap.set(  _overlap_x,  _diff_y );
              _contact2.overlap.set( -_overlap_x, -_diff_y );
            }
            else if ( _overlap_y > 0.0 )
            {
              _contact1.overlap.set(  _overlap_x,  _overlap_y );
              _contact2.overlap.set( -_overlap_x, -_overlap_y );
            }
            else
            {
              _contact1.overlap.set(  _overlap_x, 0.0 );
              _contact2.overlap.set( -_overlap_x, 0.0 );
            }
            _contact1.type = col_it1->type;
            _contact2.type = col_it2->type;
            _contact1.normal = _contact1.overlap.norm();
            _contact2.normal = _contact2.overlap.norm();
//            DEBUG_STREAM << "CollisionHandler::contains : Overlap 1 = " << _contact1.overlap <<  " -- Normal 1 = " << _contact1.normal;
//            DEBUG_STREAM << "CollisionHandler::contains : Overlap 2 = " << _contact2.overlap <<  " -- Normal 2 = " << _contact2.normal;
            callback( object1, object2 );
          }
          else
          {
            if ( _diff_y < 0.0 )
            {
              _contact1.overlap.set( 0.0,  _diff_y );
              _contact2.overlap.set( 0.0, -_diff_y );
            }
            else if ( _overlap_y > 0.0 )
            {
              _contact1.overlap.set( 0.0,  _overlap_y );
              _contact2.overlap.set( 0.0, -_overlap_y );
            }
            else
            {
              continue;
            }
            _contact1.type = col_it1->type;
            _contact2.type = col_it2->type;
            _contact1.normal = _contact1.overlap.norm();
            _contact2.normal = _contact2.overlap.norm();
//            DEBUG_STREAM << "CollisionHandler::contains : Overlap 1 = " << _contact1.overlap <<  " -- Normal 1 = " << _contact1.normal;
//            DEBUG_STREAM << "CollisionHandler::contains : Overlap 2 = " << _contact2.overlap <<  " -- Normal 2 = " << _contact2.normal;
            callback( object1, object2 );
          }
        }
      }
    }
  }


  void CollisionHandler::callback( CollidableObject* object1, CollidableObject* object2 )
  {
    _coef_restitution = 1.0 + 0.5 * ( object1->getElasticity() + object2->getElasticity() );

    if ( object1->hasMovement() )
    {
      if ( object2->hasMovement() )
      {
        // If both objects can move we weight the contact vector by the inverse of their masses
        _total_M = object1->getMass() + object2->getMass();

        float temp = _coef_restitution * ( object2->getVelocity()*_contact1.normal - object1->getVelocity()*_contact1.normal );

        _contact1.inertiaRatio = object2->getMass()/_total_M;
        _contact2.inertiaRatio = object1->getMass()/_total_M;
        _contact1.impulse =  _contact1.inertiaRatio * temp * _contact1.normal;
        _contact2.impulse =  _contact2.inertiaRatio * temp * _contact2.normal;

      }
      else
      {
        _contact1.inertiaRatio = 1.0;
        _contact2.inertiaRatio = 0.0;
//        _contact1.impulse = _coef_restitution * (object2->getVelocity() - object1->getVelocity()) % _contact1.normal;
        _contact1.impulse = _coef_restitution * (object2->getVelocity()*_contact1.normal - object1->getVelocity()*_contact1.normal) * _contact1.normal;
        _contact2.impulse.zero();
      }
    }
    else
    {
      if ( object2->hasMovement() )
      {
        _contact1.inertiaRatio = 0.0;
        _contact2.inertiaRatio = 1.0;
        _contact1.impulse.zero();
//        _contact2.impulse = _coef_restitution * (object2->getVelocity() - object1->getVelocity()) % _contact2.normal;
        _contact2.impulse = _coef_restitution * (object1->getVelocity()*_contact2.normal - object2->getVelocity()*_contact2.normal) * _contact2.normal;
      }
      else
      {
        _contact1.inertiaRatio = 0.0;
        _contact2.inertiaRatio = 0.0;
        _contact1.impulse.zero();
        _contact2.impulse.zero();
      }
    }

//    DEBUG_STREAM << "CollisionHandler::callback : Impulse 1 = " << _contact1.impulse <<  " -- Impulse 2 = " << _contact2.impulse;

    object1->onCollision( _contact1, object2 );
    object2->onCollision( _contact2, object1 );
  }

}

