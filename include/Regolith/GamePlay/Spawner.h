#ifndef REGOLITH_GAMEPLAY_SPAWNER_H_
#define REGOLITH_GAMEPLAY_SPAWNER_H_

#include "Regolith/Global/Global.h"


namespace Regolith
{

////////////////////////////////////////////////////////////////////////////////

  // Forward declarations
  class ContextLayer;
  class PhysicalObject;
  class AudioHandler;
  class SpawnBuffer;


////////////////////////////////////////////////////////////////////////////////

  class Spawner
  {
    friend class SpawnBuffer;

    private:
      SpawnBuffer& _owner;
      ContextLayer* _targetLayer;

    protected:
      Spawner( ContextLayer*, SpawnBuffer& );

    public:

    // Spawn the next object at the specified position in the specified layer.
    // Will fail if too many objects have already been spawned without removing the oldest
    // Returns a const pointer to the object in case the context wishes to interrogate that specific object
    const PhysicalObject* spawn( Vector ) const;

    const PhysicalObject* spawn( Vector, Vector ) const;

  };


////////////////////////////////////////////////////////////////////////////////

  /*
   * Defines an buffer of copies of a PhysicalObject
   * It enables objects and contexts to spawn objects into a context layer at will.
   * If all the objects are already spawned none will be added however.
   */
  class SpawnBuffer
  {
    private:
      struct ListItem
      {
        PhysicalObject* object;
        ListItem* next;
      };

      // Pointer the first object - the master object. Do not delete this one!
      ListItem* _start;

      // Current position in the circular list
      ListItem* _currentItem;

    public:
      // Specify the number to cache and the base object
      SpawnBuffer();

      // Delete all the clones, but not the master copy!
      ~SpawnBuffer();

      // Specify the number to cache and the base object
      void fill( unsigned int, PhysicalObject*, AudioHandler* );

      // Empty the buffer
      void clear();

      Spawner requestSpawner( ContextLayer* );

      // Return true if the next object can be spawned
      bool spawnable() const;

      // Forcibly pop and object. Check with "spawnable()" first!
      PhysicalObject* pop();
  };

}

#endif // REGOLITH_GAMEPLAY_SPAWNER_H_

