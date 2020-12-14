
#ifndef REGOLITH_MANAGERS_DATA_MANAGER_H_
#define REGOLITH_MANAGERS_DATA_MANAGER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Utilities/MutexedBuffer.h"
#include "Regolith/Assets/RawObjectDetails.h"

#include <thread>
#include <mutex>
#include <vector>
#include <map>


namespace Regolith
{
  // Forward Declarations
  class DataHandler;


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Data Manager

  /*
   * The Data Manager controls the global operations regarding loading the game assets stored on 
   * the hard disk.
   * This includes images/texture files, sound/music files and all fonts.A
   * It is the object that loads and caches the data stored in the index file - a lookup of all
   * asset files of all types and the information required to correctly load them into memory.
   *
   * The Data Loading Thread loads the assets requested by the data handlers into memory for the
   * current context groups to use.
   */
  class DataManager
  {
    // Make these functions friends to make loading/unloading data easier in the Data Manager Thread
    friend void dataManagerLoadingThread();
    friend void dataLoadFunction();
    friend void dataUnloadFunction();

    private:
      // Flag to indicate the loading thread is active
      bool _loading;
      mutable std::mutex _loadFlagMutex;

      // Path to the list of all texture files and their modifiers
      std::string _indexFile;

      // Map of all the assets that can be used
      AssetMap _assets;

      // Loading queue
      MutexedBuffer< DataHandler* > _loadQueue;

      // Unloading queue
      MutexedBuffer< DataHandler* > _unloadQueue;

    protected:
      void setLoading( bool );

    public:
      // Con/de-struction
      DataManager();
      ~DataManager();

      // Clear all the stored data
      void clear();

      // Basic configuration of manager and build the global objects
      void configure( Json::Value& );


////////////////////////////////////////////////////////////////////////////////
      // Dynamic loading/unloading

      // Load all the data in a specific handler
      void load( DataHandler* );

      // Unload all the data in a specific handler
      void unload( DataHandler* );

      // Return true if the loading thread is active
      bool isLoading() const;


////////////////////////////////////////////////////////////////////////////////
      // Inspect the index information

      // Return a reference to the asset info
      const Asset& getAsset( std::string ) const;


////////////////////////////////////////////////////////////////////////////////
      // Access basic loaded assets

      // Build a raw texture object with null pointer for a data handler
      RawTexture buildRawTexture( std::string ) const;

      // Build a raw music object with null pointer for a data handler
      RawMusic buildRawMusic( std::string ) const;

      // Build a raw sound object with null pointer for a data handler
      RawSound buildRawSound( std::string ) const;

      // Build a raw font object with null pointer for a data handler
      RawFont buildRawFont( std::string ) const;

      // Build a raw text object with null pointer for a data handler
      RawText buildRawText( std::string ) const;


      // Load a raw texture object
      void loadRawTexture( std::string, RawTexture& ) const;

      // Load a raw music object
      void loadRawMusic( std::string, RawMusic& ) const;

      // Load a raw sound object
      void loadRawSound( std::string, RawSound& ) const;

      // Load a raw font object
      void loadRawFont( std::string, RawFont& ) const;

      // Load a raw text object
      void loadRawText( std::string, RawText& ) const;

  };

}

#endif // REGOLITH_MANAGERS_DATA_MANAGER_H_

