
#include "Architecture/Context.h"

#include "logtastic.h"


namespace Regolith
{
  namespace Interfaces
  {

    Context::Context( Vector pos, float w, float h ) :
      ControllableInterface(),
      Component(),
      _theInput(),
      _theAudio(),
      _position( pos ),
      _width( w ),
      _height( h ),
      _paused( false ),
      _pauseable( false ),
      _childContexts( "Child Contexts" ),
      _layers( "Context Layers" ),
      _animatedObjects(),
      _spawnedObjects()
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


    void Context::update( Uint32 time )
    {
      AnimatedList::iterator it = _animatedObjects.begin();
      AnimatedList::iterator end =  _animatedObjects.end();
      while ( it != end )
      {
        (*it)->udpate( time );
        ++it;
      }
    }


    void Context::step( Uint32 time )
    {
      NamedVector< Layer, true >:: iterator layers_end = _layers.end();
      for ( NamedVector<Layer, true>::iterator layer_it = _layers.begin(); layer_it != layers_end; ++layer_it )
      {
        MoveableList& moveables = (*layer_it)->moveables;
        MoveableList::iterator move_it = moveables.begin();
        MoveableList::iterator move_end = moveables.end();
        while ( move_it != move.end )
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
      NamedVector< Layer, true >:: iterator layers_end = _layers.end();
      for ( NamedVector<Layer, true>::iterator layer_it = _layers.begin(); layer_it != layers_end; ++layer_it )
      {
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
            (*draw_it)->render(...);
            ++draw_it;
          }
        }
      }
    }


    void Context::resolveCollisions()
    {
      Contact contact;

      NamedVector< Layer, true >:: iterator layers_end = _layers.end();
      for ( NamedVector<Layer, true>::iterator layer_it = _layers.begin(); layer_it != layers_end; ++layer_it )
      {
        TeamMap& teams = (*layer_it)->teams;
        TeamMap::iterator team_it = teams.begin();
        TeamMap::iterator opponent_start = team_it + 1; // Change this line to allow team self-collisions
        TeamMap::iterator opponents_end = teams.end();

        while ( opponent_start != opponents_end )
        {
          for ( TeamMap::iterator opponent_it = opponent_start; opponent_it != oppnents_end; ++opponent_it )
          {
            CollidableList::iterator collidable_end = team_it->second.end();
            for ( CollidableList::iterator collidable_it = team_it->second.begin(); collidable_it != collidable_end(); ++collidable_it )
            {
              CollidableList::iterator enemy_end = opponent_it->second.end();
              for ( CollidableList::iterator enemy_it = enemy_it->second.begin(); enemy_it != enemy_end(); ++enemy_it )
              {
                if ( collides( (*collidable_it), (*enemy_it), contact ) )
                {
                  contact->applyContact();
                }
              }
            }
          ++team_it;
          ++opponent_start;
          }
        }
      }
    }

  }
}

