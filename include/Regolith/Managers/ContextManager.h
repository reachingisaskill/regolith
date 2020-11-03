
#ifndef REGOLITH_MANAGERS_CONTEXT_MANAGER_H_
#define REGOLITH_MANAGERS_CONTEXT_MANAGER_H_

#include "Regolith/Managers/ContextGroup.h"
#include "Regolith/Utilities/ProxyMap.h"

#include <thread>
#include <atomic>


namespace Regolith
{

  class ContextManager
  {
    friend void contextManagerLoadingThread();

    private:
      // Flag to indcate if everything is loaded
      bool _loaded;
      mutable std::mutex _loadedMutex;

      // Progress counter!
      float _progress;
      mutable std::mutex _progressMutex;

      // The data that exists in the global scope
      ContextGroup _globalContextGroup;

      // Vector of the individual context handlers
      ProxyMap< ContextGroup* > _contextGroups;

      // Record of the currently loaded context group
      ContextGroup* _currentContextGroup;

      // Pointer to the next context group to load
      ContextGroup* _nextContextGroup;

    protected:
      // Set whether the context group is loaded
      void setLoaded( bool );

      // Set the current loading progress
      void setProgress( float );

    public:
      // Con/Destructors
      ContextManager();
      ~ContextManager();

      // Configure the manager
      void configure( Json::Value& );

      // Destroy everything and join the thread
      void clear();

      // Load the first configuration - halts the calling thread until completion
      void loadEntryPoint();


//////////////////////////////////////////////////////////////////////////////// 
      // Context construction/manipulation

      // Return a pointer to a specific context group
      Proxy<ContextGroup*> requestContextGroup( std::string name ) { return _contextGroups.request( name ); }

      // Set a specific context group to load
      void setNextContextGroup( ContextGroup* );

      // Signal the manager to start the loading process. Should only be called by a global context!
      void loadNextContextGroup();


      // Return a pointer to the currently loaded context group
      ContextGroup* getCurrentContextGroup() { return _currentContextGroup; }

      // Return a pointer to the global context group
      ContextGroup* getGlobalContextGroup() { return &_globalContextGroup; }


//////////////////////////////////////////////////////////////////////////////// 
      // Loading thread interactions

      // Return false while a context group is being loaded
      bool isLoaded() const;

      // Return a float value representing the loading progress
      float loadingProgress() const;
  };

}

#endif // REGOLITH_MANAGERS_CONTEXT_MANAGER_H_

