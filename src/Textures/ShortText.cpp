
#include "Regolith/Textures/ShortText.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  ShortText::ShortText() :
    _rawFont( nullptr ),
//    _rawText( nullptr ),
    _udpate( false ),
    _theTexture( nullptr ),
    _theSurface( nullptr ),
    _clip( { 0, 0, 0, 0 } ),
    _flipFlag( SDL_FLIP_NONE ),
    _center( { 0, 0 } ),
    _rotation( 0.0 )
  {
  }


  void ShortText::setRenderedTexture( SDL_Texture* texture )
  {
    // Destroy if one already exists
    if ( _theTexture != nullptr )
    {
      SDL_DestroyTexture( _theTexture );
    }

    // Update the sdl texture pointer
    _theTexture = texture;

    // Change the update flag
    _udpate = false;
  }


  void ShortText::clearSDLTexture()
  {
    if ( _theTexture != nullptr )
    {
      SDL_DestroyTexture( _theTexture );
    }
  }


  void ShortText::configure( Json::Value& json_data, DataHandler& handler )
  {
//    Utilities::validateJson( json_data, "text_name", Utilities::JSON_TYPE_STRING );
//    _rawText = handler.getRawText( json_data["text_name"].asString() );

    Utilities::validateJson( json_data, "font", Utilities::JSON_TYPE_STRING );

    _rawFont = handler.getRawFont( json_data["font"].asString() );
  }


  void ShortText::writeText( std::string& text )
  {
    _theSurface = TTF_RenderText_Solid( _rawFont->ttf_font, text.c_str(), _rawFont->colour );
    if ( _theSurface == nullptr )
    {
      Exception ex( "loadSurfaceFromString()", "Could not render text", true );
      ex.addDetail( "Text string", text );
      ex.addDetail( "TTF error", TTF_GetError() );
      throw ex;
    }

    _update = true;
  }

}

