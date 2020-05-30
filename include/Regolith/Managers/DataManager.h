
#ifndef REGOLITH_MANAGERS_DATA_MANAGER_H_
#define REGOLITH_MANAGERS_DATA_MANAGER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Managers/DataManager.h"


namespace Regolith
{

  class DataManager
  {
    typedef std::vector< std::string, RawTexture > RawTextureMap;
    friend class DataManager::DataBuilder;

    public:
      class DataBuilder
      {
        private:
          // Quick reference to the manager - makesure one exists!
          DataManager& _manager;

          // All the texture data
          Json::Value _data;

        protected:
          DataBuilder( DataManager&, std::string );

        public:
          void loadTexture( unsigned int );

          void unloadTexture( unsigned int );
      };

    private:
      // Path to the list of all texture files and their modifiers
      std::string _textureFile;

      // Map of all the raw texture pointers
      NamedVector< RawTexture, false > _rawTextures;

      // All objects used by the contexts using this handler.
      NamedVector<GameObject, true > _gameObjects;

      // Handler for data in global space
      DataHandler _globalData;

      // Context specific data handlers
      NamedVector< DataHandler, true > _dataHandlers;

    public:
      DataManager();

      ~DataManager();


      // Basic configuration of object and build the global objects
      void configure( Json::Value& );

////////////////////////////////////////////////////////////////////////////////
      // Handler access

      // Return the global data handler
      const DataHandler& globalData() const { return _globalData; }

      // Request a handler. Returns the ID
      DataHandler* requestHandler( std::string );

      // Return a pointer to a specific data handler
      DataHandler* getHandler( unsigned int i ) { return _dataHandlers[i]; }


////////////////////////////////////////////////////////////////////////////////
      // Dynamic loading/unloading

      // Load all the data in a specific handler
      void load( unsigned int );

      // Unload all the data in a specific handler
      void unload( unsigned int );


////////////////////////////////////////////////////////////////////////////////
      // Raw Textures

      // Request a texture with a given name
      unsigned int requestRawTexture( std::string name ) { return _rawTextures.addName( name ); }
      RawTexture* getRawTexture( unsigned int );

      // Returns true if the texture file is owned by the global handler
      bool isGlobal( unsigned int i ) { return _globalData.isCached( i ); }

  };

}

#endif // REGOLITH_MANAGERS_DATA_MANAGER_H_

