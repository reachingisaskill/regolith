
#include "Regolith/GamePlay/ContextLayer.h"

#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/Architecture/ControllableInterface.h"
#include "Regolith/GamePlay/Context.h"


namespace Regolith
{

  ContextLayer::ContextLayer() :
    _owner( nullptr ),
    _position( 0.0 ),
    _movementScale( 0.0 ),
    _width( 0.0 ),
    _height( 0.0 ),
    drawables(),
    moveables(),
    teams(),
    clickables(),
    animated()
  {
  }


  ContextLayer::~ContextLayer()
  {
    _sceneGraph.clear();

    for ( TeamMap::iterator it = _teams.begin(); it != _teams.end(); ++it )
    {
      it->second.clear();
    }
    _teams.clear();
  }


  void ContextLayer::configure( Context* owner, Vector pos, Vector move_scale, float width, float height )
  {
    _owner = owner;
    _position = pos;
    _movementScale = move_scale;
    _width = width;
    _height = height;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Frame update functions

  void ContextLayer::update( float time )
  {
    for ( PhysicalObjectList::iterator it = _sceneGraph.begin(); it != _sceneGraph.end(); ++it )
    {
      // If object is marked for destruction, remove it from the scene graph
      if ( (*it)->isDestroyed() )
      {
        _sceneGraph.erase( it );
        continue;
      }

      // If object can be moved, do the physics integration
      if ( (*it)->hasMovement() )
      {
        (*it)->step( time );
      }

      // If the object is animated, update the animation
      if ( (*it)->hasAnimation() )
      {
        (*it)->update( time );
      }
    }


    // RESOLVE COLLISIONS HERE!


    DEBUG_STREAM << " Starting Layer Collision: " << layer_it->second.teams.size();

    // Colliding objects
    CollisionHandler::iterator end = _theCollision.collisionEnd();
    for ( CollisionHandler::iterator it = _theCollision.collisionBegin(); it != end; ++it )
    {
      PhysicalObjectList& team1 = layer_it->second.teams[ it->first ];
      PhysicalObjectList& team2 = layer_it->second.teams[ it->second ];

      PhysicalObjectList::iterator end1 = team1.end();
      PhysicalObjectList::iterator end2 = team2.end();

      for ( CollidableList::iterator it1 = team1.begin(); it1 != end1; ++it1 )
      {
        for ( CollidableList::iterator it2 = team2.begin(); it2 != end2; ++it2 )
        {
          collides( (*it1), (*it2) );
        }
      }
    }
  }


  void ContextLayer::render( Camera& camera )
  {
    for ( PhysicalObjectList::iterator it = _sceneGraph.begin(); it != _sceneGraph.end(); ++it )
    {
      // If the object can be drawn, render it to the back buffer
      if ( (*it)->hasTexture() )
      {
        (*it)->render( camera );
      }
    }
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Object spawning, placing and caching

  void ContextLayer::spawn( PhysicalObject* obj, const Vector& position )
  {
    PhysicalObject* object = _owner->_owner->spawn( obj, position );
    _sceneGraph.push_back( object );
  }

}

