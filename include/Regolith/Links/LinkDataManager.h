
#ifndef REGOLITH_LINKS_LINK_DATA_MANAGER_H_
#define REGOLITH_LINKS_LINK_DATA_MANAGER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Links/Link.h"
#include "Regolith/Managers/DataManager.h"


namespace Regolith
{

  // Prevent partial specialisations from passing
  template < class REQUESTER >
  class Link< DataManager, REQUESTER >
  {
    struct FalseType : public std::false_type {};
    static_assert( FalseType::value, "A link is not permitted between the requested class and the data manager" );
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Rendering thread access
  class DataHandler;

  template <>
  class Link< DataManager, DataHandler >
  {
    private:

      DataManager& _manager;

    public:

      Link( DataManager& m ) : _manager( m ) {}

      RawTexture buildRawTexture( std::string s ) const { return _manager.buildRawTexture( s ); }
      RawMusic buildRawMusic( std::string s ) const { return _manager.buildRawMusic( s ); }
      RawSound buildRawSound( std::string s ) const { return _manager.buildRawSound( s ); }
      RawFont buildRawFont( std::string s ) const { return _manager.buildRawFont( s ); }
      RawText buildRawText( std::string s ) const { return _manager.buildRawText( s ); }
  };

////////////////////////////////////////////////////////////////////////////////////////////////////
  class FontManger;

  template <>
  class Link< DataManager, FontManager >
  {
    private:

      DataManager& _manager;

    public:

      Link( DataManager& m ) : _manager( m ) {}

      RawFont buildRawFont( std::string s ) const { return _manager.buildRawFont( s ); }
  };

}

#endif // REGOLITH_LINKS_LINK_DATA_MANAGER_H_

