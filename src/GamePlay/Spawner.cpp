
#include "Regolith/GamePlay/Spawner.h"
#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/GamePlay/ContextLayer.h"


namespace Regolith
{

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Spawner

  Spawner::Spawner( ContextLayer* layer, SpawnBuffer& buffer ) :
    _owner( buffer ),
    _targetLayer( layer )
  {
  }


  const PhysicalObject* Spawner::spawn( Vector position ) const
  {
    if ( _owner.spawnable() )
    {
      PhysicalObject* temp = _owner.pop();
      temp->setPosition( position );
      _targetLayer->layerGraph[ temp->getCollisionTeam() ].push_back( temp );
      return temp;
    }
    else
    {
      return nullptr;
    }
  }


  const PhysicalObject* Spawner::spawn( Vector position, Vector velocity ) const
  {
    if ( _owner.spawnable() )
    {
      PhysicalObject* temp = _owner.pop();
      temp->setPosition( position );
      temp->setVelocity( velocity );
      _targetLayer->layerGraph[ temp->getCollisionTeam() ].push_back( temp );
      return temp;
    }
    else
    {
      return nullptr;
    }
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Spawn Buffer

  SpawnBuffer::SpawnBuffer() :
    _start( nullptr ),
    _currentItem( nullptr )
  {
  }


  SpawnBuffer::~SpawnBuffer()
  {
    this->clear();
  }


  void SpawnBuffer::fill( unsigned int num, PhysicalObject* master )
  {
    if ( num == 0 )
    {
      Exception ex( "SpawnBuffer::fill()", "Cannot create an empty spawn buffer. Will cause undefined behaviour." );
      throw ex;
    }

    // All objects in the buffer start off destroyed.
    // That way we can determine which ones are still "alive" in the context layers
    _start = new ListItem();
    _start->object = master;
    _start->next = _start;
    _start->object->destroy();

    DEBUG_STREAM << "SpawnBuffer::fill : Master @ " << master;

    // Master object counts as the first one
    --num;
    _currentItem = _start;

    for ( unsigned int i = 0; i < num; ++i )
    {
      ListItem* new_item = new ListItem();
      new_item->object = master->clone();
      new_item->next = _start;
      new_item->object->destroy();

      DEBUG_STREAM << "SpawnBuffer::fill : Clone @ " << new_item->object;

      _currentItem->next = new_item;
      _currentItem = new_item;
    }

    // Reset the current item pointer to the start
    _currentItem = _start;
  }


  void SpawnBuffer::clear()
  {
    // Already clear
    if ( _start == nullptr ) return;

    _currentItem = _start->next;

    while ( _currentItem != _start )
    {
      ListItem* temp = _currentItem->next;

      // Delete the clone
      delete _currentItem->object;
      // Delete the list item
      delete _currentItem;

      _currentItem = temp;
    }

    _currentItem = nullptr;

    // Delete the first item, but not the master copy
    delete _start;
    _start = nullptr;
  }


  bool SpawnBuffer::spawnable() const
  {
    return _currentItem->object->isDestroyed();
  }


  Spawner SpawnBuffer::requestSpawner( ContextLayer* layer )
  {
    return Spawner( layer, *this );
  }


  PhysicalObject* SpawnBuffer::pop()
  {
    // Get the next object
    PhysicalObject* temp = _currentItem->object;

    // Update the current pointer
    _currentItem = _currentItem->next;

    // Reset the state of the object and return it
    temp->reset();
    return temp;
  }

}

