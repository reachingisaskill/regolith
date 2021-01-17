
#include "Regolith/Handlers/CollisionHandler.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/ObjectInterfaces/CollidableObject.h"
#include "Regolith/Contexts/ContextLayer.h"

#include <limits>


namespace Regolith
{
////////////////////////////////////////////////////////////////////////////////////////////////////
  // Forward declarations


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Collision handler class member functions

  CollisionHandler::CollisionHandler() :
    _teamCollision(),
    _pairings(),
    _containers(),
    _contact1(),
    _contact2()
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
    INFO_LOG( "CollisionHandler::configure : Configuring Collision Handler" );

    validateJson( json_data, "team_collision", JsonType::ARRAY );
    validateJsonArray( json_data["collision_rules"], 0, JsonType::STRING );

    validateJson( json_data, "collision_rules", JsonType::ARRAY );
    validateJsonArray( json_data["collision_rules"], 0, JsonType::ARRAY );

    validateJson( json_data, "container_rules", JsonType::ARRAY );
    validateJsonArray( json_data["container_rules"], 0, JsonType::ARRAY );


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
      validateJsonArray( collision_rules[i], 2, JsonType::STRING );

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
      validateJsonArray( container_rules[i], 2, JsonType::STRING );

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

      DEBUG_STREAM << "CollisionHandler::collides : Checkig hitboxed. Obj1 : " << _object_pos1 << ", Obj2 : " << _object_pos2;

      for ( Collision::iterator col_it1 = collision1.begin(); col_it1 != collision1.end(); ++col_it1 )
      {
        const HitBox& box1 = (*col_it1);
        for ( Collision::iterator col_it2 = collision2.begin(); col_it2 != collision2.end(); ++col_it2 )
        {
          const HitBox& box2 = (*col_it2);

          _contact1.overlap = std::numeric_limits<float>::lowest();


////////////////////////////////////////////////////////////////////////////////
          // First objects edges.
          for ( unsigned int box1_i = 0; box1_i < box1.number; ++box1_i )
          {
            // Absolute _positions of the hit boxes vertices
            Vector point1 = _object_pos1 + box1.points[box1_i].getRotated( object1->rotation() );
            Vector normal1 = box1.normals[box1_i].getRotated( object1->rotation() );

            DEBUG_STREAM << "CollisionHandler::collides : Checking Object1, Side " << box1_i << " P = " << point1 << ", N = " << normal1;

            float largest_overlap = std::numeric_limits<float>::max();
            Vector overlapping_point;

            for ( unsigned int box2_i = 0; box2_i < box2.number; ++box2_i )
            {
              Vector point2 = _object_pos2 + box2.points[box2_i].getRotated( object2->rotation() );
              float diff = (point2*normal1) - (point1*normal1);

              DEBUG_STREAM << "CollisionHandler::collides :   Object2, P = " << point2 << " - DIFF = " << diff;

              if ( diff < largest_overlap ) // Find the max size of the overlap
              {
                largest_overlap = diff;
                overlapping_point = point2;
              }
            }

            DEBUG_STREAM << "CollisionHandler::collides : Checking Object1, Overlap = " << largest_overlap;
            if ( largest_overlap > 0.0 )
            {
              // Separating axis found. We good!
              goto CollidingSeparatingAxisFound;
            }

            if ( largest_overlap > _contact1.overlap ) // Smallest total overlap is the shortest collision resolution
            {
              _contact1.overlap = largest_overlap;
              _contact1.normal = normal1;
              _contact1.point = overlapping_point;
            }
          }
          _contact2.overlap = _contact1.overlap;
          _contact2.normal = -_contact1.normal;
          _contact2.point = _contact1.point;

          DEBUG_STREAM << "CollisionHandler::collides : Object1, Overlap = " << _contact1.overlap;


////////////////////////////////////////////////////////////////////////////////
          // Second objects edges.
          for ( unsigned int box2_i = 0; box2_i < box2.number; ++box2_i )
          {
            // Absolute _positions of the hit boxes vertices
            Vector point2 = _object_pos2 + box2.points[box2_i].getRotated( object2->rotation() );
            Vector normal2 = box2.normals[box2_i].getRotated( object2->rotation() );

            DEBUG_STREAM << "CollisionHandler::collides : Checking Object2, Side " << box2_i << " P = " << point2 << ", N = " << normal2;

            float largest_overlap = std::numeric_limits<float>::max();
            Vector overlapping_point;

            for ( unsigned int box1_i = 0; box1_i < box1.number; ++box1_i )
            {
              Vector point1 = _object_pos1 + box1.points[box1_i].getRotated( object1->rotation() );
              float diff = (point1*normal2) - (point2*normal2);

              DEBUG_STREAM << "CollisionHandler::collides :   Object1, P = " << point1 << " - DIFF = " << diff;


              if ( diff < largest_overlap ) // Find the max size of the overlap
              {
                largest_overlap = diff;
                overlapping_point = point1;
              }
            }

            DEBUG_STREAM << "CollisionHandler::collides : Checking Object2, Overlap = " << largest_overlap;
            if ( largest_overlap > 0.0 )
            {
              // Separating axis found. We good!
              goto CollidingSeparatingAxisFound;
            }

            if ( largest_overlap > _contact2.overlap ) // Smallest total overlap is the shortest collision resolution
            {
              _contact2.overlap = largest_overlap;
              _contact2.normal = normal2;
              _contact2.point = overlapping_point;
            }
          }
          _contact1.overlap = _contact2.overlap;
          _contact1.normal = -_contact2.normal;
          _contact1.point = _contact2.point;

          DEBUG_STREAM << "CollisionHandler::collides : Object2, Overlap = " << _contact2.overlap;


////////////////////////////////////////////////////////////////////////////////
          // Collision determined
          // Got this far, therefore separating axis not found. Calculate collision parameters

          _contact1.type = col_it1->collisionType;
          _contact2.type = col_it2->collisionType;
          DEBUG_STREAM << "CollisionHandler::collides : Overlap 1 = " << _contact1.overlap <<  " -- Normal 1 = " << _contact1.normal;
          DEBUG_STREAM << "CollisionHandler::collides : Overlap 2 = " << _contact2.overlap <<  " -- Normal 2 = " << _contact2.normal;
          callback( object1, object2 );

CollidingSeparatingAxisFound:
          // Just loop to the next hit box in the collision. Note the labels refer to a statement, so we need a continue or something
          continue;
        }
      }


    }
  }


  void CollisionHandler::contains( CollidableObject* object1, CollidableObject* object2 )
  {
    _object_pos1 = object1->position();
    _object_pos2 = object2->position();

    const Collision& collision1 = object1->getCollision();
    const Collision& collision2 = object2->getCollision();

    for ( Collision::iterator col_it1 = collision1.begin(); col_it1 != collision1.end(); ++col_it1 )
    {
      const HitBox& box1 = (*col_it1);

      // Is the bounding box contained within this hitbox
      for ( unsigned int box1_i = 0; box1_i < box1.number; ++box1_i )
      {
        // Absolute _positions of the hit boxes vertices
        Vector point1 = _object_pos1 + box1.points[box1_i].getRotated( object1->rotation() );
        Vector normal1 = box1.normals[box1_i].getRotated( object1->rotation() );
        float overlap_edge = point1 * normal1;

        Vector point2 = _object_pos2;

        if ( (point2*normal1) > overlap_edge )
        {
          goto ContainingHitBoxCollision;
        }

        point2.x() = point2.x() + object2->getWidth();
        if ( (point2*normal1) > overlap_edge )
        {
          goto ContainingHitBoxCollision;
        }

        point2.y() = point2.y() + object2->getHeight();
        if ( (point2*normal1) > overlap_edge )
        {
          goto ContainingHitBoxCollision;
        }

        point2.x() = point2.x() - object2->getWidth();
        if ( (point2*normal1) > overlap_edge )
        {
          goto ContainingHitBoxCollision;
        }
      }

      // No overlaps found. Break this loop and go to the next parent hitbox
      break;

        // Skipped the continue. Therefore we must check for an overlap
ContainingHitBoxCollision:
      DEBUG_LOG( "CollisionHandler::contains : Collision determined" );

      _contact1.overlap = std::numeric_limits<float>::lowest();

        // Iterate through the hit boxes of the daughter objects
      for ( Collision::iterator col_it2 = collision2.begin(); col_it2 != collision2.end(); ++col_it2 )
      {
        const HitBox& box2 = (*col_it2);

        // This happens in reverse.
        float largest_overlap = std::numeric_limits<float>::lowest();
        Vector overlapping_point;

        // Is the daughter hit box contained within the parent hitbox
        for ( unsigned int box1_i = 0; box1_i < box1.number; ++box1_i )
        {
          // Absolute _positions of the hit boxes vertices
          Vector point1 = _object_pos1 + box1.points[box1_i].getRotated( object1->rotation() );
          Vector normal1 = box1.normals[box1_i].getRotated( object1->rotation() );

          DEBUG_STREAM << "CollisionHandler::contains : Checking parent, Side " << box1_i << " P = " << point1 << ", N = " << normal1;

          for ( unsigned int box2_i = 0; box2_i < box2.number; ++box2_i )
          {
            Vector point2 = _object_pos2 + box2.points[box2_i].getRotated( object2->rotation() );
            float diff = (point2*normal1) - (point1*normal1);

            DEBUG_STREAM << "CollisionHandler::contains :   Daughter, P = " << point2 << " - DIFF = " << diff;

            if ( diff > largest_overlap ) // Find the max size of the overlap
            {
              largest_overlap = diff;
              overlapping_point = point2;
              DEBUG_STREAM << "CollisionHandler::contains :   Daughter, new overlap = " << largest_overlap;
            }
          }

          if ( largest_overlap > _contact1.overlap ) // Smallest total overlap is the shortest collision resolution
          {
            _contact1.overlap = largest_overlap;
            _contact1.normal = normal1;
            _contact1.point = overlapping_point;
          }
        }

        DEBUG_STREAM << "CollisionHandler::contains : Overlap = " << _contact1.overlap << " N = " << _contact1.normal;

        if ( _contact1.overlap < 0.0 ) continue;

        _contact2.overlap = _contact1.overlap;
        _contact2.normal = -_contact1.normal;
        _contact2.point = _contact1.point;

        callback( object1, object2 );

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
        _total_L = object1->getInertia() + object2->getInertia();


        float temp = _coef_restitution * ( object2->getVelocity()*_contact1.normal - object1->getVelocity()*_contact1.normal );

        _contact1.massRatio = object2->getMass()/_total_M;
        _contact2.massRatio = object1->getMass()/_total_M;
        _contact1.impulse =  _contact1.massRatio * temp * _contact1.normal;
        _contact2.impulse =  _contact2.massRatio * temp * _contact2.normal;

        _contact1.inertiaRatio = object2->getInertia()/_total_L;
        _contact2.inertiaRatio = object1->getInertia()/_total_L;

        Vector lever_arm_1 = ( _contact1.point - object1->getPosition() ).norm();
        Vector lever_arm_2 = ( _contact2.point - object2->getPosition() ).norm();

        DEBUG_STREAM << "CollisionHandler::callback : Lever Arms : " << _contact1.point << " ^ " << object1->position() << " = " << lever_arm_1;
        DEBUG_STREAM << "CollisionHandler::callback : Lever Arms : " << _contact2.point << " ^ " << object2->position() << " = " << lever_arm_2;

        _contact1.angularImpulse = _contact1.inertiaRatio * temp * ( _contact1.normal ^ lever_arm_1 );
        _contact2.angularImpulse = _contact2.inertiaRatio * temp * ( _contact2.normal ^ lever_arm_2 );

      }
      else
      {
        _contact1.massRatio = 1.0;
        _contact2.massRatio = 0.0;
        _contact1.impulse = _coef_restitution * (object2->getVelocity()*_contact1.normal - object1->getVelocity()*_contact1.normal) * _contact1.normal;
        _contact2.impulse.zero();

        _contact1.inertiaRatio = 1.0;
        _contact2.inertiaRatio = 0.0;
      }
    }
    else
    {
      if ( object2->hasMovement() )
      {
        _contact1.massRatio = 0.0;
        _contact2.massRatio = 1.0;
        _contact1.impulse.zero();
        _contact2.impulse = _coef_restitution * (object1->getVelocity()*_contact2.normal - object2->getVelocity()*_contact2.normal) * _contact2.normal;

        Vector lever_arm_2 = ( _contact2.point - object2->center() ).norm();

        _contact1.inertiaRatio = 0.0;
        _contact2.inertiaRatio = 1.0;

        _contact1.angularImpulse = 0.0;
        _contact2.angularImpulse =  _contact2.impulse ^ lever_arm_2;
      }
      else
      {
        _contact1.massRatio = 0.0;
        _contact2.massRatio = 0.0;
        _contact1.impulse.zero();
        _contact2.impulse.zero();

        Vector lever_arm_1 = ( _contact1.point - object1->center() ).norm();

        _contact1.inertiaRatio = 0.0;
        _contact2.inertiaRatio = 0.0;

        _contact1.angularImpulse = _contact1.impulse ^ lever_arm_1;
        _contact2.angularImpulse = 0.0;
      }
    }

//    DEBUG_STREAM << "CollisionHandler::callback : Impulse 1 = " << _contact1.impulse <<  " -- Impulse 2 = " << _contact2.impulse;

    object1->onCollision( _contact1, object2 );
    object2->onCollision( _contact2, object1 );
  }





////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
  // DEPRECATED COLLISION ALGORITHMS.

  /*

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

*/

}

