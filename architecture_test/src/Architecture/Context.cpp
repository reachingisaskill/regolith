
#include "Regolith/Architecture/Context.h"
#include "Regolith/GamePlay/Camera.h"
#include "Regolith/Managers/Manager.h"

#include "logtastic.h"


namespace Regolith
{

  Context::Context() :
    MassProduceable(),
    ControllableInterface(),
    Component(),
    _theInput(),
    _theAudio(),
    _position( 0.0 ),
    _width( 0.0 ),
    _height( 0.0 ),
    _paused( false ),
    _pauseable( false ),
    _childContexts( "Child Contexts" ),
    _layers( "Context Layers" ),
    _spawnedObjects(),
    _animatedObjects()
  {
  }


  Context::~Context()
  {
    INFO_LOG( "Clearing animated cache" );
    _animatedObjects.clear();

    INFO_LOG( "Clearing spawned objects" );
    for ( PhysicalObjectList::iterator it = _spawnedObjects.begin(); it != _spawnedObjects.end(); ++it )
    {
      delete (*it);
    }
    _spawnedObjects.clear();

    INFO_LOG( "Clearing layers" );
    _layers.clear();

    INFO_LOG( "Clearing child contexts" );
    _childContexts.clear();
  }


  void Context::update( float time )
  {
    AnimatedList::iterator it = _animatedObjects.begin();
    AnimatedList::iterator end =  _animatedObjects.end();
    while ( it != end )
    {
      (*it)->update( time );
      ++it;
    }

    NamedVector< ContextLayer, true >::iterator layers_end = _layers.end();
    for ( NamedVector<ContextLayer, true>::iterator layer_it = _layers.begin(); layer_it != layers_end; ++layer_it )
    {
      (*layer_it)->update( time );
    }
  }


  void Context::step( float time )
  {
    NamedVector< ContextLayer, true >::iterator layers_end = _layers.end();
    for ( NamedVector<ContextLayer, true>::iterator layer_it = _layers.begin(); layer_it != layers_end; ++layer_it )
    {
      MoveableList& moveables = (*layer_it)->moveables;
      MoveableList::iterator move_it = moveables.begin();
      MoveableList::iterator move_end = moveables.end();
      while ( move_it != move_end )
      {
        if ( (*move_it)->isDestroyed() )
        {
          moveables.erase( move_it++ );
        }
        else
        {
          (*move_it)->step( time );
          ++move_it;
        }
      }
    }
  }


  void Context::render()
  {
    NamedVector< ContextLayer, true >::iterator layers_end = _layers.end();
    for ( NamedVector<ContextLayer, true>::iterator layer_it = _layers.begin(); layer_it != layers_end; ++layer_it )
    {
      const Camera& layerCamera = (*layer_it)->getCamera();

      DrawableList& drawables = (*layer_it)->drawables;
      DrawableList::iterator draw_it = drawables.begin();
      DrawableList::iterator draw_end = drawables.end();
      while ( draw_it != draw_end )
      {
        if ( (*draw_it)->isDestroyed() )
        {
          drawables.erase( draw_it++ );
        }
        else
        {
          (*draw_it)->render( layerCamera );
          ++draw_it;
        }
      }
    }
  }


  void Context::resolveCollisions()
  {
    NamedVector< ContextLayer, true >::iterator layers_end = _layers.end();
    for ( NamedVector<ContextLayer, true>::iterator layer_it = _layers.begin(); layer_it != layers_end; ++layer_it )
    {
      TeamMap& teams = (*layer_it)->teams;
      TeamMap::iterator team_it = teams.begin();
      TeamMap::iterator opponent_start = ++teams.begin(); // Change this line to allow team self-collisions
      TeamMap::iterator opponents_end = teams.end();

      while ( opponent_start != opponents_end )
      {
        for ( TeamMap::iterator opponent_it = opponent_start; opponent_it != opponents_end; ++opponent_it )
        {

          CollidableList::iterator collidable_end = team_it->second.end();
          for ( CollidableList::iterator collidable_it = team_it->second.begin(); collidable_it != collidable_end; ++collidable_it )
          {
            if ( ! (*collidable_it)->collisionActive() ) continue;

            CollidableList::iterator enemy_end = opponent_it->second.end();
            for ( CollidableList::iterator enemy_it = opponent_it->second.begin(); enemy_it != enemy_end; ++enemy_it )
            {
              if ( ! (*enemy_it)->collisionActive() ) continue;

              collides( (*collidable_it), (*enemy_it) );
            }
          }

        }
        ++team_it;
        ++opponent_start;
      }
    }
  }


  void Context::openChildContext( unsigned int id )
  {
    Manager::getInstance()->openContext( _childContexts[ id ] );
  }


  void Context::spawn( unsigned int id, unsigned int layer, const Vector& position )
  {
  }


  void Context::addSpawnedObject( PhysicalObject*, unsigned int, const Vector& )
  {
  }


  void Context::cacheObject( GameObject* )
  {
  }

//////////////////////////////////////////////////////////////////////////////////////////////////// 
  // Context configuration

  void Context::configure( Json::Value& )
  {
  }

}

