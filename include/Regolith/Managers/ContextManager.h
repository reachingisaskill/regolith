
#ifndef REGOLITH_MANAGERS_CONTEXT_MANAGER_H_
#define REGOLITH_MANAGERS_CONTEXT_MANAGER_H_

#include "Regolith/Managers/ContextGroup.h"
#include "Regolith/Utilities/Condition.h"
#include "Regolith/Utilities/MutexedBuffer.h"

#include <thread>
#include <atomic>
#include <map>


namespace Regolith
{

  class ContextManager
  {
    // Allow links to access the private members
    template < class T, class R > friend class Link;

    // Useful typedefs
    public:
      typedef std::map<std::string, ContextGroup*> ContextGroupMap;
      typedef std::pair< ContextGroup*, bool > BufferElement;
      typedef MutexedBuffer< BufferElement > ContextGroupBuffer;


    private:
      // The data that exists in the global scope
      ContextGroup _globalContextGroup;

      // Vector of the individual context handlers
      ContextGroupMap _contextGroups;

      // Entry point on load
      ContextGroup* _entryPoint;

      // Maintains a queue of the jobs for the loading thread
      ContextGroupBuffer _contextGroupBuffer;

      // Pointer to the context group to be rendered
      Condition< ContextGroup* > _renderContextGroup;
      mutable std::mutex _renderGroupMutex;


      // Signals a ContextGroup is ready to be loaded
      std::condition_variable _loadingThreadCondition;
      mutable std::mutex _loadingThreadActive;


    protected:
////////////////////////////////////////////////////////////////////////////////
      // Rendering thread accessible functions

      // Performs rendering operations on a context group. For the engine to use.
      void renderContextGroup( Camera& );


////////////////////////////////////////////////////////////////////////////////
      // Engine accessible functions

      // Load/unload new context group
      void loadContextGroup( ContextGroup* );
      void unloadContextGroup( ContextGroup* );


//////////////////////////////////////////////////////////////////////////////// 
      // Context accessible functions

      // Return a pointer to a specific context group
      ContextGroup* getContextGroup( std::string );

      // Return a pointer to the global context group
      ContextGroup* getGlobalContextGroup() { return &_globalContextGroup; }


//////////////////////////////////////////////////////////////////////////////// 
      // ContextGroup accessible functions

      // Requests the rendering of the provided context group. Thread execution is halted until it is complete.
      // This function is for context groups, during loading/unloading, to wait on the redering process from the engine.
      void requestRenderContextGroup( ContextGroup* );

    public:
      // Con/Destructors
      ContextManager();
      ~ContextManager();

      // Configure the manager
      void configure( Json::Value& );

      // Destroy everything and join the thread
      void clear();

      // Load the first configuration - halts the calling thread until completion
      ContextGroup* loadEntryPoint();


//////////////////////////////////////////////////////////////////////////////// 
      // Interaction with the loading thread

      // Return true when the loading thread is active
      bool isLoading() const;
  };

}

#endif // REGOLITH_MANAGERS_CONTEXT_MANAGER_H_

