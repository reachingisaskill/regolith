
#ifndef REGOLITH_MANAGERS_DATA_HANDLER_H_
#define REGOLITH_MANAGERS_DATA_HANDLER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Assets/RawTexture.h"
#include "Regolith/Assets/RawSound.h"
#include "Regolith/Assets/RawMusic.h"
#include "Regolith/Assets/RawFont.h"
#include "Regolith/Assets/RawText.h"
#include "Regolith/Components/Engine.h"
#include "Regolith/Utilities/ProxyMap.h"

#include <string>
#include <queue>
#include <mutex>


namespace Regolith
{
  // Forward Declaration
  class DataManager;

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Data Handler

  class DataHandler
  {
    // Rendering thread is a friend so that it can render SDL surfaces in the background
    friend void engineRenderingThread();

    typedef std::queue< RawTexture* > SurfaceRenderQueue;

    private:
      // List of all the textures
      RawTextureMap _rawTextures;

      // List of all the sounds
      RawSoundMap _rawSounds;

      // List of all the music
      RawMusicMap _rawMusic;

      // List of all the fonts
      RawFontMap _rawFonts;

      // List of all the fonts
      RawTextMap _rawTexts;

      // Queue of surfaces to render in the engine.
      SurfaceRenderQueue _surfaceRenderQueue;


      // Stores a flag to record whether the data is in memory or not
      bool _isLoaded;
      std::atomic<bool> _isRendered;

      // Mutex controls load/unload/flag access and operations
      mutable std::mutex _loadingMutex;

    protected:
      // Pop the next unrendered surface for the rendering thread.
      // Should use something like a private visitor/mediator object to control access to specific functions
      RawTexture* popRenderTexture();


    public:
      DataHandler();

      ~DataHandler();

      // Data handlers must be non-copyable, etc. Ensures DataManager knows who is using what asset
      DataHandler( const DataHandler& ) = delete;
      DataHandler( DataHandler&& ) = delete;
      DataHandler& operator=( const DataHandler& ) = delete;
      DataHandler& operator=( DataHandler&& ) = delete;

////////////////////////////////////////////////////////////////////////////////
      // State information

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

      // Get a font with a given name
      RawFont* getRawFont( std::string );

      // Get a font with a given name
      RawText* getRawText( std::string );

  };

}

#endif // REGOLITH_MANAGERS_DATA_HANDLER_H_

