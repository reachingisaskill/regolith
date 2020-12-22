
#include "Regolith/Managers/FontManager.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  FontManager::FontManager() :
    _fonts(),
    _fontFiles()
  {
  }


  FontManager::~FontManager()
  {
  }


  void FontManager::clear()
  {
    for ( FontMap::iterator font_it = _fonts.begin(); font_it != _fonts.end(); ++font_it )
    {
      for ( FontSizeMap::iterator size_it = font_it->second.begin(); size_it != font_it->second.end(); ++size_it )
      {
        // Unload the TTF data
        TTF_CloseFont( size_it->second.first );
      }
      // Clear the size map
      font_it->second.clear();
    }
    // Clear the font map
    _fonts.clear();

    // Clear the map of file locations
    _fontFiles.clear();
  }


  void FontManager::configure( Json::Value& json_data )
  {
    Utilities::validateJson( json_data, "fonts", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJsonArray( json_data["fonts"], 0, Utilities::JSON_TYPE_STRING );

    Json::Value& fonts = json_data["fonts"];
    for ( Json::ArrayIndex i = 0; i < fonts.size(); ++i )
    {
      std::string font_name = fonts[i].asString();
      if ( _fonts.find( font_name ) != _fonts.end() )
      {
        WARN_STREAM << "FontManager::configure : Attempting to add the same font twice. Skipping : " << font_name;
        continue;
      }

      // Get the raw font object
      RawFont raw_font = Manager::getInstance()->getDataManager().buildRawFont( font_name );

      // Test that we can load it
      TTF_Font* ttf_font = TTF_OpenFont( raw_font.filename.c_str(), 10 );
      if ( ttf_font == nullptr ) // Failed to open
      {
        Exception ex( "FontManager::configure()", "Could not validate font" );
        ex.addDetail( "Font Name", font_name );
        ex.addDetail( "File Path", raw_font.filename );
        ex.addDetail( "TTF Error", TTF_GetError() );
        throw ex;
      }
      TTF_CloseFont( ttf_font );

      // Add it to the map
      _fontFiles[ font_name ] = raw_font;
    }
  }


  Pen FontManager::requestPen( std::string font_name, unsigned int size, SDL_Color colour )
  {
    FontFileMap::iterator file_found = _fontFiles.find( font_name );
    if ( file_found == _fontFiles.end() )
    {
      Exception ex( "FontManager::requestPen()", "Could not find requested font. Is it local to this context group?" );
      ex.addDetail( "Font Name", font_name );
      throw ex;
    }

    FontMap::iterator font_found = _fonts.find( font_name );
    if ( font_found == _fonts.end() )
    {
      font_found = _fonts.insert( std::make_pair( font_name, FontSizeMap() ) ).first;
    }

    FontSizeMap::iterator size_found = font_found->second.find( size );
    if ( size_found == font_found->second.end() )
    {
      TTF_Font* ttf_font = TTF_OpenFont( file_found->second.filename.c_str(), size );

      if ( ttf_font == nullptr ) // Failed to open
      {
        Exception ex( "FontManager::requestPen()", "Failed to load font" );
        ex.addDetail( "Font Name", font_name );
        ex.addDetail( "Font Path", file_found->second.filename );
        ex.addDetail( "Font Size", size );
        ex.addDetail( "TTF Error", TTF_GetError() );
        throw ex;
      }

      size_found = font_found->second.insert( std::make_pair( size, FontPair( ttf_font, colour ) ) ).first;
    }

    // Build Pen
    Pen new_pen( size_found->second.first, size_found->second.second );

    // Return Pen
    return new_pen;
  }

}

