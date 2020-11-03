
#ifndef REGOLITH_MANAGERS_DATA_MANAGER_H_
#define REGOLITH_MANAGERS_DATA_MANAGER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Utilities/MutexedBuffer.h"
#include "Regolith/Managers/RawObjectDetails.h"

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

  class DataManager
  {
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

      /*
      // Map of texture details
      RawTextureDetailMap _textureDetails;

      // Map of string details
      RawStringDetailMap _stringDetails;

      // Map of music details
      RawMusicDetailMap _musicDetails;

      // Map of sounds details
      RawSoundDetailMap _soundDetails;

//      // Map of font details
//      RawTextureDetailMap _fontDetails;
      */


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

      // Basic configuration of object and build the global objects
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


      // Build a raw texture object with no pointer for a data handler using the index
      RawTexture buildRawTexture( std::string ) const;

      // Build a raw music object with no pointer for a data handler using the index
      RawMusic buildRawMusic( std::string ) const;

      // Build a raw sound object with no pointer for a data handler using the index
      RawSound buildRawSound( std::string ) const;


      // Build a raw texture object with no pointer for a data handler using the index
      void loadRawTexture( std::string, RawTexture& ) const;

      // Build a raw music object with no pointer for a data handler using the index
      void loadRawMusic( std::string, RawMusic& ) const;

      // Build a raw sound object with no pointer for a data handler using the index
      void loadRawSound( std::string, RawSound& ) const;
  };

}

#endif // REGOLITH_MANAGERS_DATA_MANAGER_H_

