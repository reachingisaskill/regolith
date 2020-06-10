
#ifndef REGOLITH_MANAGERS_CONTEXT_HANDLER_H_
#define REGOLITH_MANAGERS_CONTEXT_HANDLER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/Context.h"
#include "Regolith/Managers/AudioHandler.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/Utilities/NamedVector.h"
#include "Regolith/Utilities/ProxyMap.h"

#include <list>
#include <queue>


namespace Regolith
{
  class LoadScreen;
  class PhysicalObject;
  class Context;

  class ContextGroup
  {
    public:
      class Operation;

    private:
      typedef std::list<PhysicalObject*> SpawnedList;
      typedef std::queue<Operation> OperationQueue;

////////////////////////////////////////////////////////////////////////////////
    private:
      // Flag to indicate that this is the global context group
      bool _isGlobalGroup;

      // Audio handlers are local to the context group
      AudioHandler _theAudio;

      // List of data handlers that can be individually loaded/unloaded
      ProxyMap< DataHandler* > _dataHandlers;

      // File name to load
      std::string _fileName;

      // Pointer to the load screen
      LoadScreen* _loadScreen;

      // List of the contexts controlled by this handler
      ProxyMap< Context* > _contexts;

      // List of all the game objects owned by the contexts
      ProxyMap< GameObject* > _gameObjects;

      // List of all the spawned physical objects used by the contexts
      SpawnedList _spawnedObjects;

      // Operations to trigger on load - Messaging the contexts before they have loaded.
      OperationQueue _onLoadOperations;

      // Starting point when this context group is loaded
      Context* _entryPoint;

      // Flag to indicate the group is loaded
      bool _isLoaded;

////////////////////////////////////////////////////////////////////////////////
    public:
      // Constructor
      ContextGroup();

      // Destructor
      ~ContextGroup();

      // Set the filename
      void configure( std::string, bool isGlobal = false );

      // Return a reference to the audio handler
      AudioHandler& getAudioHandler() { return _theAudio; }

      // Load all the contexts and relevant data
      void load();

      // Remove as much as possible while this context group is not loaded
      void unload();

      // Return the ID of the load screen
      LoadScreen* getLoadScreen() const { return _loadScreen; }

      // Set the entry point when this context group loads
      void setEntryPoint( Context* c ) { _entryPoint = c; }

      // Return the entry point for this context group
      Context* getEntryPoint() { return _entryPoint; }

      // Return a flag to indicate the group is loaded into memory
      bool isLoaded() const { return _isLoaded; }


////////////////////////////////////////////////////////////////////////////////
      // Context functions

      // Return a proxy for a requested context.
      Proxy<Context*> requestContext( std::string name ) { return _contexts.request( name ); }

      // Return a pointer to a requested context.
      Context* getContext( std::string name ) { return _contexts.get( name ); }


////////////////////////////////////////////////////////////////////////////////
      // Game Object functions

      // Return a proxy for a game object
      Proxy<GameObject*> requestGameObject( std::string name ) { return _gameObjects.request( name ); }

      // Return a pointer to a given object.
      GameObject* getGameObject( std::string name ) { return _gameObjects.get( name ); }


      // Return a proxy for a physical object (Pointer is implecitly converted in wrappers)
      Proxy<PhysicalObject*> requestPhysicalObject( std::string name ) { return _gameObjects.request( name ); }

      // Return a pointer to a given object.
      PhysicalObject* getPhysicalObject( std::string name ) { return dynamic_cast<PhysicalObject*>( _gameObjects.get( name ) ); }


      // Spawn a new instance of a resource and return the pointer to the caller
      PhysicalObject* spawn( std::string name, const Vector& pos );

      // Spawn a new instance of a resource and return the pointer to the caller. Memory ownership stays with the context group, not the caller
      PhysicalObject* spawn( PhysicalObject*, const Vector& pos );



////////////////////////////////////////////////////////////////////////////////////////////////////
    // Context Group Operations

    class Operation
    {
      public:
        enum OperationType
        {
          ACTION_NULL,
          ACTION_SET_ENTRY_POINT
        };

      private:
        OperationType _operation;
        std::string _key;
        std::string _value;

      public:
        Operation( OperationType, std::string, std::string = std::string("") );

        void trigger( ContextGroup* );

    };

  };


}

#endif // REGOLITH_MANAGERS_CONTEXT_HANDLER_H_

