
#ifndef REGOLITH_MANAGERS_CONTEXT_MANAGER_H_
#define REGOLITH_MANAGERS_CONTEXT_MANAGER_H_

#include "Regolith/Managers/ContextGroup.h"

#include <thread>
#include <atomic>
#include <map>


namespace Regolith
{

  class ContextManager
  {
    friend void contextManagerLoadingThread();

    typedef std::map<std::string, ContextGroup*> ContextGroupMap;

    private:
      // The data that exists in the global scope
      ContextGroup _globalContextGroup;

      // Vector of the individual context handlers
      ContextGroupMap _contextGroups;

      // Record of the currently loaded context group
      ContextGroup* _currentContextGroup;
      mutable std::mutex _currentGroupMutex;

      // Pointer to the next context group to load
      ContextGroup* _nextContextGroup;
      mutable std::mutex _nextGroupMutex;


    protected:
      // Set whether the context group is loaded
      void setLoaded( bool );

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
      ContextGroup* getContextGroup( std::string );

      // Set a specific context group to load
      void setNextContextGroup( ContextGroup* );

      // Signal the manager to start the loading process. Should only be called by a global context!
      void loadNextContextGroup();


      // Return a pointer to the currently loaded context group
      ContextGroup* getCurrentContextGroup();

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

