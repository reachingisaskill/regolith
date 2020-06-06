
#ifndef REGOLITH_MANAGERS_DATA_MANAGER_H_
#define REGOLITH_MANAGERS_DATA_MANAGER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/GamePlay/Texture.h"
#include "Regolith/Utilities/NamedVector.h"
#include "Regolith/Utilities/NamedReferenceVector.h"
#include "Regolith/Utilities/MutexedBuffer.h"

#include <thread>
#include <vector>
#include <map>


namespace Regolith
{
  // Forward Declarations
  class DataBuilder;


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Data Manager

  class DataManager
  {
    friend void theLoadingThread();
    friend void loadFunction();
    friend void unloadFunction();

    private:
      // Loading thread container
      std::thread _loadingThread;

      // Path to the list of all texture files and their modifiers
      std::string _textureFile;

      // Map of all the raw texture pointers
      RawTextureMap _rawTextures;
      
      // List of all the raw texture vectors for the handlers
      NamedReferenceVector< RawTextureCache > _rawTextureCaches;

      // Map of the status of all IDNumberSets
      std::map<IDNumber, bool> _loadedCaches;

      // All objects used by the contexts using this handler.
      NamedVector<GameObject, true > _gameObjects;

      // Handler for data in global space
      DataHandler _globalData;


      // Loading queue
      MutexedBuffer< IDNumber > _loadQueue;

      // Unloading queue
      MutexedBuffer< IDNumber > _unloadQueue;

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

      // Request a handler. Returns the ID
      void configureHandler( DataHandler&, std::string );


      // Special function to load the entry point context handler in the current thread
      void loadEntryPoint( IDNumber );


////////////////////////////////////////////////////////////////////////////////
      // Handler access

      // Return the global data handler
      const DataHandler& globalData() const { return _globalData; }

      // Return the global data handler
      DataHandler* globalDataPointer() { return &_globalData; }

      // Return a reference to the game objects list
      NamedVector<GameObject, true>& gameObjects() { return _gameObjects; }


////////////////////////////////////////////////////////////////////////////////
      // Dynamic loading/unloading

      // Load the data for a specific context group
      void loadHandler( IDNumber );

      // Load all the data in a specific handler
      void load( IDNumber );

      // Unload all the data in a specific handler
      void unload( IDNumber );


////////////////////////////////////////////////////////////////////////////////
      // Raw Textures

      // Request a texture with a given name
      RawTexturePointer requestRawTexture( std::string name );
      RawTexture* getRawTexture( std::string name ) { return &_rawTextures[ name ]; }

      // Returns true if the texture file is owned by the global handler
      bool isGlobal( RawTexturePointer p ) { return _globalData.isCached( p ); }


//      void print();
  };

}

#endif // REGOLITH_MANAGERS_DATA_MANAGER_H_

