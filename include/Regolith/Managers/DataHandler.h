
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
      // List of all the textures required for this data handler - owned by the manager
      RawTextureCache* _requiredTextures;

      // Reference to all th game objects - owned by the manager
      NamedVector<GameObject, true >& _gameObjects;

      // Optional context for the manager to use during loading
      IDNumber _loadScreen;

    public:
      DataHandler();

      ~DataHandler();

//      void configure( std::string );

      IDNumber getID() const { return _handlerID; }


////////////////////////////////////////////////////////////////////////////////
      // Raw Textures

      // Request a raw texture point - caches this texture in the handler to be dynamically loaded
      RawTexture* findTexture( std::string );

      // Returns true if that texture is cached by this handler
      bool isCached( RawTexturePointer ) const;


////////////////////////////////////////////////////////////////////////////////
      // Game Objects


  };

}

#endif // REGOLITH_MANAGERS_DATA_HANDLER_H_

