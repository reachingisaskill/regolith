
#include "Regolith/GamePlay/ContextLayer.h"

#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/Architecture/Noisy.h"
#include "Regolith/Architecture/Interactable.h"
#include "Regolith/Architecture/Controllable.h"
#include "Regolith/Architecture/Drawable.h"
#include "Regolith/Architecture/Moveable.h"
#include "Regolith/Architecture/Collidable.h"
#include "Regolith/Architecture/Animated.h"
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
    drawables.clear();

    moveables.clear();

    for ( TeamMap::iterator it = teams.begin(); it != teams.end(); ++it )
    {
      it->second.clear();
    }
    teams.clear();

    clickables.clear();

    animated.clear();
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
  // Object spawning, placing and caching

  void ContextLayer::spawn( PhysicalObject* obj, const Vector& position )
  {
    PhysicalObject* object = _owner->_owner->spawn( obj, position );
    cacheObject( object );
  }


  void ContextLayer::cacheObject( GameObject* object )
  {
    if ( object->hasInput() )
    {
      dynamic_cast<Controllable*>( object )->registerActions( _owner->_theInput );
    }
    if ( object->hasClick() )
    {
      _owner->_theFocus.addObject( dynamic_cast<Clickable*>( object ) );
    }
    if ( object->hasTexture() )
    {
      drawables.push_back( dynamic_cast<Drawable*>( object ) );
    }
    if ( object->hasMovement() )
    {
      moveables.push_back( dynamic_cast<Moveable*>( object ) );
    }
    if ( object->hasCollision() )
    {
      Collidable* temp = dynamic_cast<Collidable*>( object );
      teams[ temp->getTeam() ].push_back( temp );
    }
    if ( object->hasAnimation() )
    {
      animated.push_back( dynamic_cast<Animated*>( object ) );
    }
//    if ( object->hasAudio() ) // This is handled by the context group
//    {
//      dynamic_cast<Noisy*>( object )->registerSounds( &_theAudio );
//    }
//    if ( object->hasInteraction() )
//    {
//       // Nothing to do for this one
//    }
  }


}

