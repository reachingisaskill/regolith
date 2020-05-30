
#ifndef REGOLITH_MANAGERS_DATA_HANDLER_H_
#define REGOLITH_MANAGERS_DATA_HANDLER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/GamePlay/Texture.h"

#include <map>


namespace Regolith
{
  // Forward Declaration
  class DataManager::DataBuilder;


  class DataHandler
  {
    private:
      // List of all the textures required for this data handler
      std::vector< unsigned int > _requiredTextures;

      // Reference to all th game objects - owned by the manager
      NamedVector<GameObject, true >& _gameObjects;

      // Optional context for the manager to use during loading
      unsigned int _loadScreen;

    public:
      DataHandler( NamedVector<GameObject, true >& );

      ~DataHandler();

      // Load all the textures into memory
      void load( DataManager::DataBuilder& );

      // Unload all the textures from memory
      void unload( DataManager::DataBuilder& );


////////////////////////////////////////////////////////////////////////////////
      // Raw Textures

      // Request a raw texture point - caches this texture in the handler to be dynamically loaded
      RawTexture* findTexture( std::string );

      // Returns true if that texture is cached by this handler
      bool isCached( unsigned int );


////////////////////////////////////////////////////////////////////////////////
      // Game Objects

      // Return the ID for a given object name
      unsigned int requestGameObject( std::string name ) { return _gameObjects.addName( name ); }

      // Return the name for a given object ID
      std::string getGameObjectName( unsigned int id ) { return _gameObjects.getName( id ); }

      // Get a global object pointer
      // Return a pointer to a given object. (Please don't delete it!)
      GameObject* getGameObject( std::string name ) { return _gameObjects.get( name ); }
      GameObject* getGameObject( unsigned int id ) { return _gameObjects[ id ]; }

      // Return a pointer to a given object. (Please don't delete it!)
      PhysicalObject* getPhysicalObject( std::string name ) { return dynamic_cast<PhysicalObject*>( _gameObjects.get( name ) ); }
      PhysicalObject* getPhysicalObject( unsigned int id ) { return dynamic_cast<PhysicalObject*>( _gameObjects[id] ); }

      // Spawn a cloned object - caller accepts ownership of memory
      // Spawn a new instance of a resource and return the memory to the caller
      PhysicalObject* spawn( unsigned int i, const Vector& pos ) { return dynamic_cast<PhysicalObject*>( _gameObjects[i] )->clone( pos ); }
      PhysicalObject* spawn( std::string name, const Vector& pos ) { return dynamic_cast<PhysicalObject*>( _gameObjects.get(name) )->clone( pos ); }

  };

}

#endif // REGOLITH_MANAGERS_DATA_HANDLER_H_

