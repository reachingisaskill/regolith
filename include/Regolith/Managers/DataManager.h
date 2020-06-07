
#ifndef REGOLITH_MANAGERS_DATA_MANAGER_H_
#define REGOLITH_MANAGERS_DATA_MANAGER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Utilities/MutexedBuffer.h"

#include <thread>
#include <vector>
#include <map>


namespace Regolith
{
  // Forward Declarations
  class DataHandler;


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Data Manager

  class DataManager
  {
    friend void dataLoadingThread();
    friend void dataLoadFunction();
    friend void dataUnloadFunction();

    private:
      // Loading thread container
      std::thread _loadingThread;

      // Path to the list of all texture files and their modifiers
      std::string _indexFile;


      // Loading queue
      MutexedBuffer< DataHandler* > _loadQueue;

      // Unloading queue
      MutexedBuffer< DataHandler* > _unloadQueue;

      // Flag to indicate the loading thread is active
      std::atomic<bool> _loading;

    public:
      // Con/de-struction
      DataManager();
      ~DataManager();

      // Clear all the stored data
      void clear();

      // Basic configuration of object and build the global objects
      void configure( Json::Value& );

      // Validate the game objects
      void validate() const;


////////////////////////////////////////////////////////////////////////////////
      // Dynamic loading/unloading

      // Load all the data in a specific handler
      void load( DataHandler* );

      // Unload all the data in a specific handler
      void unload( DataHandler* );

      // Return true if the loading thread is active
      bool isLoading() const { return _loading; }

  };

}

#endif // REGOLITH_MANAGERS_DATA_MANAGER_H_

