
#ifndef REGOLITH_MANAGERS_DATA_HANDLER_H_
#define REGOLITH_MANAGERS_DATA_HANDLER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/GamePlay/Texture.h"
#include "Regolith/GamePlay/Noises.h"
#include "Regolith/Utilities/ProxyMap.h"

#include <string>
#include <vector>
#include <mutex>
#include <pair>


namespace Regolith
{
  // Forward Declaration
  class DataManager;

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Data Handler

  class DataHandler
  {
    typedef std::pair< SDL_Surface*, RawTexture* > SurfaceTexturePair;
    typedef std::queue< SurfaceTexturePair > SurfaceRenderQueue;

    private:
      // List of all the textures
      RawTextureMap _rawTextures;

      // List of all the sounds
      RawSoundMap _rawSounds;

      // List of all the music
      RawMusicMap _rawMusic;

      // Queue of surfaces to render in the engine.
      SurfaceRenderQueue _surfaceRenderQueue;


      // Stores a flag to record whether the data is in memory or not
      bool _isLoaded;

      // Mutex controls load/unload/flag access and operations
      mutable std::mutex _loadingMutex;


    public:
      DataHandler();

      ~DataHandler();

      bool isLoaded() const;

      void load();

      void unload();

////////////////////////////////////////////////////////////////////////////////
      // Raw Textures

      // Get an texture with a given name
      RawTexture* getRawTexture( std::string );

      // Get an texture with a given name
      RawSound* getRawSound( std::string );

      // Get an texture with a given name
      RawMusic* getRawMusic( std::string );

  };

}

#endif // REGOLITH_MANAGERS_DATA_HANDLER_H_

