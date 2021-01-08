
#ifndef REGOLITH_MANAGERS_DATA_MANAGER_H_
#define REGOLITH_MANAGERS_DATA_MANAGER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/Component.h"
#include "Regolith/Utilities/MutexedBuffer.h"
#include "Regolith/Assets/RawObjectDetails.h"

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

  /*
   * The Data Manager controls the global operations regarding loading the game assets stored on 
   * the hard disk.
   * This includes images/texture files, sound/music files and all fonts.A
   * It is the object that loads and caches the data stored in the index file - a lookup of all
   * asset files of all types and the information required to correctly load them into memory.
   *
   * The Data Loading Thread loads the assets requested by the data handlers into memory for the
   * current context groups to use.
   */
  class DataManager : public Component
  {
    // Allow links to access the private members
    template < class T, class R > friend class Link;

    // Non-copyable, etc
    DataManager( const DataManager& ) = delete;
    DataManager( DataManager&& ) = delete;
    DataManager operator=( const DataManager& ) = delete;
    DataManager operator=( DataManager&& ) = delete;

    private:
      // Path to the list of all texture files and their modifiers
      std::string _indexFile;

      // Map of all the assets that can be used
      AssetMap _assets;


    protected:

      // Only data handlers can own data

      // Build a raw texture object with null pointer for a data handler
      RawTexture buildRawTexture( std::string ) const;

      // Build a raw music object with null pointer for a data handler
      RawMusic buildRawMusic( std::string ) const;

      // Build a raw sound object with null pointer for a data handler
      RawSound buildRawSound( std::string ) const;

      // Build a raw font object with null pointer for a data handler
      RawFont buildRawFont( std::string ) const;

      // Build a raw text object with null pointer for a data handler
      RawText buildRawText( std::string ) const;


    public:
      // Con/de-struction
      DataManager();
      ~DataManager();

      // Clear all the stored data
      void clear();

      // Basic configuration of manager and build the global objects
      void configure( Json::Value& );


////////////////////////////////////////////////////////////////////////////////
      // Inspect the index information

      // Return a reference to the asset info
      const Asset& getAsset( std::string ) const;


////////////////////////////////////////////////////////////////////////////////
      // Component Interface
      // Register game-wide events with the manager
      virtual void registerEvents( InputManager& ) override {}

      // Regolith events
      virtual void eventAction( const RegolithEvent&, const SDL_Event& ) override {}

  };

}

#endif // REGOLITH_MANAGERS_DATA_MANAGER_H_

