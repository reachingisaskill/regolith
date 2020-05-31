
#ifndef REGOLITH_MANAGERS_DATA_HANDLER_H_
#define REGOLITH_MANAGERS_DATA_HANDLER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/GamePlay/Texture.h"
#include "Regolith/Architecture/GameObject.h"
#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/Utilities/NamedVector.h"

#include <string>
#include <vector>


namespace Regolith
{
  // Forward Declaration
  class DataManager;

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Data Handler

  class DataHandler
  {
    friend class DataManager;
    private:
      // Identification of handler - set by the manager
      IDNumber _handlerID;

      // List of all the textures required for this data handler - owned by the manager
      IDNumberSet* _requiredTextures;

      // Reference to all th game objects - owned by the manager
      NamedVector<GameObject, true >& _gameObjects;

      // Optional context for the manager to use during loading
      IDNumber _loadScreen;

    public:
      DataHandler();

      ~DataHandler();

      void configure( std::string );

      IDNumber getID() const { return _handlerID; }


////////////////////////////////////////////////////////////////////////////////
      // Raw Textures

      // Request a raw texture point - caches this texture in the handler to be dynamically loaded
      RawTexture* findTexture( std::string );

      // Returns true if that texture is cached by this handler
      bool isCached( IDNumber ) const;


////////////////////////////////////////////////////////////////////////////////
      // Game Objects

      // Return the ID for a given object name
      IDNumber requestGameObject( std::string name ) { return _gameObjects.addName( name ); }

      // Return the name for a given object ID
      std::string getGameObjectName( IDNumber id ) { return _gameObjects.getName( id ); }

      // Get a global object pointer
      // Return a pointer to a given object. (Please don't delete it!)
      GameObject* getGameObject( std::string name ) { return _gameObjects.get( name ); }
      GameObject* getGameObject( IDNumber id ) { return _gameObjects[ id ]; }

      // Return a pointer to a given object. (Please don't delete it!)
      PhysicalObject* getPhysicalObject( std::string name ) { return dynamic_cast<PhysicalObject*>( _gameObjects.get( name ) ); }
      PhysicalObject* getPhysicalObject( IDNumber id ) { return dynamic_cast<PhysicalObject*>( _gameObjects[id] ); }

      // Spawn a cloned object - caller accepts ownership of memory
      // Spawn a new instance of a resource and return the memory to the caller
      PhysicalObject* spawn( IDNumber i, const Vector& pos ) { return dynamic_cast<PhysicalObject*>( _gameObjects[i] )->clone( pos ); }
      PhysicalObject* spawn( std::string name, const Vector& pos ) { return dynamic_cast<PhysicalObject*>( _gameObjects.get(name) )->clone( pos ); }

  };

}

#endif // REGOLITH_MANAGERS_DATA_HANDLER_H_

