
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
  // Forward declarations
  class DataManager;


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Data Handler

  class DataHandler
  {

    private:
      // Pointer to the data manager. Slightly faster this way.
      DataManager* _manager;

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


    public:
      DataHandler();

      ~DataHandler();

      // Data handlers must be non-copyable, etc.
      DataHandler( const DataHandler& ) = delete;
      DataHandler( DataHandler&& ) = delete;
      DataHandler& operator=( const DataHandler& ) = delete;
      DataHandler& operator=( DataHandler&& ) = delete;


      // Deletes all the loaded data
      void clear();


////////////////////////////////////////////////////////////////////////////////
      // Inferfaces to request proxies to the asset data

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

