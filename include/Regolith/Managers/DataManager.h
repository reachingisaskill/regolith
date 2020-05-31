
#ifndef REGOLITH_MANAGERS_DATA_MANAGER_H_
#define REGOLITH_MANAGERS_DATA_MANAGER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/GamePlay/Texture.h"
#include "Regolith/Utilities/NamedVector.h"
#include "Regolith/Utilities/NamedReferenceVector.h"

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
    private:
      // Path to the list of all texture files and their modifiers
      std::string _textureFile;

      // Map of all the raw texture pointers
      NamedReferenceVector< RawTexture > _rawTextures;
      
      // List of all the raw texture vectors for the handlers
      NamedReferenceVector< std::vector< unsigned int > > _rawTextureLookup;

      // All objects used by the contexts using this handler.
      NamedVector<GameObject, true > _gameObjects;

      // Handler for data in global space
      DataHandler _globalData;

    public:
      // Con/de-struction
      DataManager();
      ~DataManager();

      // Basic configuration of object and build the global objects
      void configure( Json::Value& );


      // Request a handler. Returns the ID
      void configureHandler( DataHandler*, std::string );


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

      // Load all the data in a specific handler
      void load( unsigned int );

      // Unload all the data in a specific handler
      void unload( unsigned int );


////////////////////////////////////////////////////////////////////////////////
      // Raw Textures

      // Request a texture with a given name
      unsigned int requestRawTexture( std::string name ) { return _rawTextures.addName( name ); }
      RawTexture* getRawTexture( unsigned int id ) { return &_rawTextures[ id ]; }
      RawTexture* getRawTexture( std::string name ) { return &_rawTextures.get( name ); }

      // Returns true if the texture file is owned by the global handler
      bool isGlobal( unsigned int i ) { return _globalData.isCached( i ); }

  };

}

#endif // REGOLITH_MANAGERS_DATA_MANAGER_H_

