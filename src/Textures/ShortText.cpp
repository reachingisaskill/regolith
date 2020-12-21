
#include "Regolith/Textures/ShortText.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  ShortText::ShortText() :
    _pen(),
//    _rawText( nullptr ),
    _update( false ),
    _theTexture( nullptr ),
    _theSurface( nullptr ),
    _clip( { 0, 0, 0, 0 } ),
    _flipFlag( SDL_FLIP_NONE ),
    _center( { 0, 0 } ),
    _rotation( 0.0 )
  {
  }


  ShortText::~ShortText()
  {
    if ( _theSurface != nullptr )
    {
      SDL_FreeSurface( _theSurface );
      _theSurface = nullptr;
    }
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
    _update = false;
  }


  void ShortText::clearSDLTexture()
  {
    if ( _theTexture != nullptr )
    {
      SDL_DestroyTexture( _theTexture );
      _theTexture = nullptr;
    }
  }


  void ShortText::configure( Json::Value& json_data, DataHandler& /*handler*/ )
  {
//    Utilities::validateJson( json_data, "text_name", Utilities::JSON_TYPE_STRING );
//    _rawText = handler.getRawText( json_data["text_name"].asString() );

    Utilities::validateJson( json_data, "font", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "size", Utilities::JSON_TYPE_INTEGER );
    Utilities::validateJson( json_data, "colour", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJsonArray( json_data["colour"], 4, Utilities::JSON_TYPE_INTEGER );

    std::string font_name = json_data["font"].asString();
    unsigned int font_size = json_data["size"].asInt();

    SDL_Color font_colour;
    font_colour.r = json_data["colour"][0].asInt();
    font_colour.g = json_data["colour"][1].asInt();
    font_colour.b = json_data["colour"][2].asInt();
    font_colour.a = json_data["colour"][3].asInt();

    _pen = Manager::getInstance()->getFontManager().requestPen( font_name, font_size, font_colour );
  }


  void ShortText::writeText( std::string& text )
  {
    if ( _theSurface != nullptr )
    {
      SDL_FreeSurface( _theSurface );
    }

    _theSurface = _pen.shortWrite( text );

    _clip.w = _theSurface->w;
    _clip.h = _theSurface->h;

    _center.x = 0.5*_clip.w;
    _center.y = 0.5*_clip.h;

    _update = true;
  }

}

