
#ifndef REGOLITH_MANAGERS_CONTEXT_MANAGER_H_
#define REGOLITH_MANAGERS_CONTEXT_MANAGER_H_

#include "Regolith/Managers/ContextGroup.h"
#include "Regolith/Utilities/WrapperMap.h"

#include <thread>
#include <atomic>


namespace Regolith
{

  class ContextManager
  {
    friend void contextManagerLoadingThread();

    private:
      // Handle to the loading thread
      std::thread _loadingThread;

      // The data that exists in the global scope
      ContextGroup _globalContextGroup;

      // Vector of the individual context handlers
      WrapperMap< ContextGroup > _contextGroups;

      // Record of the currently loaded context group
      ContextGroup* _currentContextGroup;

      // Pointer to the next context group to load
      ContextGroup* _nextContextGroup;

      // Atomic loaded flag for loading screen to check progress
      std::atomic<bool> _isLoaded;

      // Atomic progress counter
      std::atomic<float> _loadedProgress;

    public:
      // Con/Destructors
      ContextManager();

      ~ContextManager();

      // Configure the manager
      void configure( Json::Value& );

      // Validate the created contexts
      void validate() const;


//////////////////////////////////////////////////////////////////////////////// 
      // Context construction/manipulation

      // Return a pointer to a specific context group
      Wrapper<ContextGroup> requestContextGroup( std::string name ) { return _contextGroups.request( name ); }

      // Load a specific context group
      void loadContextGroup( ContextGroup* );

      // Return a pointer to the currently loaded context group
      ContextGroup* getCurrentContextGroup() { return _currentContextGroup; }

      // Return a pointer to the global context group
      ContextGroup* getGlobalContextGroup() { return &_globalContextGroup; }


//////////////////////////////////////////////////////////////////////////////// 
      // Loading thread interactions

      // Return false while a context group is being loaded
      bool isLoaded() const { return _isLoaded; }

      // Return a float value representing the loading progress
      float loadingProgress() const { return _loadedProgress; }
  };

}

#endif // REGOLITH_MANAGERS_CONTEXT_MANAGER_H_

