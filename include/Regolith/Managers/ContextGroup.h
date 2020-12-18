
#ifndef REGOLITH_MANAGERS_CONTEXT_HANDLER_H_
#define REGOLITH_MANAGERS_CONTEXT_HANDLER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/GameObject.h"
#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/Managers/AudioHandler.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/GamePlay/Spawner.h"
#include "Regolith/Utilities/NamedVector.h"
#include "Regolith/Utilities/ProxyMap.h"

#include <list>
#include <string>
#include <map>
#include <queue>


namespace Regolith
{

  class Context;
  class Camera;

////////////////////////////////////////////////////////////////////////////////

  class ContextGroup
  {
    public:
//      class Operation;

////////////////////////////////////////////////////////////////////////////////
    private:
      typedef std::map<std::string, SpawnBuffer> SpawnBufferMap;
//      typedef std::queue<Operation> OperationQueue;
      typedef std::map<std::string, Context*> ContextMap;

////////////////////////////////////////////////////////////////////////////////
    private:
      // Set the number of objects to render per frame when loading this group
      unsigned int _renderRate;


      // Flag to indicate that this is the global context group
      bool _isGlobalGroup;

      // Audio handlers are local to the context group
      AudioHandler _theAudio;

      // Data handler that owns the asset raw data
      DataHandler _theData;

      // File name to load
      std::string _fileName;

      // Pointer to the load screen
      Context** _loadScreen;

      // List of the contexts controlled by this handler
      ContextMap _contexts;

      // List of all the game objects owned by the contexts
      PhysicalObjectMap _gameObjects;

      // List of all the spawned physical objects used by the contexts
      SpawnBufferMap _spawnBuffers;

//      // Operations to trigger on load - Messaging the contexts before they have loaded.
//      OperationQueue _onLoadOperations;

      // Starting point when this context group is loaded
      Context** _entryPoint;

////////////////////////////////////////////////////////////////////////////////
      // Flags and variables to interface with loading functions

      // Flag to indicate the group is loaded
      bool _isLoaded;
      bool _loadingState;
      unsigned int _loadProgress;
      unsigned int _loadTotal;
      mutable std::mutex _mutexProgress;

      // Iterator to track the current rendering position
      PhysicalObjectMap::iterator _renderPosition;

      // Indicates the engine has finished rendering/destroying textures
      bool _isRendered;


////////////////////////////////////////////////////////////////////////////////
    protected:
      // Lock the mutex before setting the load progress
      void resetProgress();
      void loadElement();


////////////////////////////////////////////////////////////////////////////////
    public:
      // Constructor
      ContextGroup();

      // Destructor
      ~ContextGroup();

      // Return a reference to the audio handler
      AudioHandler& getAudioHandler() { return _theAudio; }

      // Return a pointer to a specific data handler within this context group
      DataHandler& getDataHandler() { return _theData; }


      // Set the filename
      void configure( std::string, bool isGlobal = false );

      // Load all the contexts and relevant data
      void load();

      // Remove as much as possible while this context group is not loaded
      void unload();


      // Return a flag to indicate the group is loaded into memory
      bool isLoaded() const;
      float getLoadProgress() const;

      bool isRendered() const { return _isRendered; }

      // Flag to indicate this is the global context group. Don't unload accidentally!
      bool isGlobal() const { return _isGlobalGroup; }


      // Rendering thread asks to render object textures in the background
      bool engineRenderLoadedObjects( Camera& );


////////////////////////////////////////////////////////////////////////////////
      // Load screen and entry points

      // Return the load screen
      Context** getLoadScreen() const { return _loadScreen; }

      // Set the entry point when this context group loads
      void setEntryPoint( Context** c ) { _entryPoint = c; }

      // Return the entry point for this context group
      Context* getEntryPoint() { return *_entryPoint; }


////////////////////////////////////////////////////////////////////////////////
      // Accessors

      // Return a pointer to a requested context.
      Context* getContext( std::string );

      // Return a pointer to a pointer to a requested context.
      Context** getContextPointer( std::string );

      // Return a pointer to a given object.
      PhysicalObject* getPhysicalObject( std::string );

      // Return a pointer to a pointer to a given object.
      PhysicalObject** getPhysicalObjectPointer( std::string );

      // Spawn an object and return a pointer to it
      PhysicalObject* spawnPhysicalObject( std::string );

      // Return a spawner for inserting spawnable objects into a specified layer
      Spawner getSpawner( std::string, ContextLayer* );

  };












  /*
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
  */


}

#endif // REGOLITH_MANAGERS_CONTEXT_HANDLER_H_

