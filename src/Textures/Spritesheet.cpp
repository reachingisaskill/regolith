
#include "Regolith/Textures/Spritesheet.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/Utilities/JsonValidation.h"

#include <cmath>


namespace Regolith
{


////////////////////////////////////////////////////////////////////////////////////////////////////  
  // Spritesheet Member function definitions

  Spritesheet::Spritesheet() :
    Texture(),
    _rawTexture( nullptr ),
    _flipFlag( SDL_FLIP_NONE ),
    _clip( { 0, 0, 0, 0 } ),
    _center( { 0, 0 } ),
    _rotation( 0.0 ),
    _currentSprite( 0 )
  {
  }


  Spritesheet::~Spritesheet()
  {
  }


  void Spritesheet::setRenderedTexture( SDL_Texture* t )
  {
    // Check and destroy if one already exists
    if ( _rawTexture->sdl_texture != nullptr )
    {
      SDL_DestroyTexture( _rawTexture->sdl_texture );
    }

    _rawTexture->sdl_texture = t;
  }


  void Spritesheet::clearSDLTexture()
  {
    if ( _rawTexture->sdl_texture != nullptr )
    {
      SDL_DestroyTexture( _rawTexture->sdl_texture );
      _rawTexture->sdl_texture = nullptr;
    }
  }


  void Spritesheet::setFrameNumber( unsigned int num )
  {
    _currentSprite = num;
    _clip.x = (_currentSprite % _rawTexture->columns) * _clip.w;
    _clip.y = (_currentSprite / _rawTexture->columns) * _clip.h;

    _center.x = _clip.x + _centerOffset.x;
    _center.y = _clip.y + _centerOffset.y;

    DEBUG_STREAM << "Spritesheet::setFrameNumber : " << _currentSprite << " : " << _clip.x << ", " << _clip.y << ", " << _clip.w << ", " << _clip.h;
  }


  void Spritesheet::configure( Json::Value& json_data, DataHandler& handler )
  {
    validateJson( json_data, "texture_name", JsonType::STRING );

    std::string texture_name = json_data["texture_name"].asString();
    _rawTexture = handler.getRawTexture( texture_name );
    DEBUG_STREAM << "Spritesheet::configure : Found texture: " << texture_name << " : " << _rawTexture;

    _clip.x = 0;
    _clip.y = 0;
    _clip.w = _rawTexture->width / _rawTexture->columns;
    _clip.h = _rawTexture->height / _rawTexture->rows;

    if ( validateJson( json_data, "center", JsonType::ARRAY, false ) )
    {
      validateJsonArray( json_data["center"], 2, JsonType::INTEGER );

      _centerOffset.x = json_data["center"].asInt();
      _centerOffset.y = json_data["center"].asInt();
    }

    if ( validateJson( json_data, "rotation", JsonType::FLOAT, false ) )
    {
      _rotation = json_data["rotation"].asFloat();
    }

    if ( validateJson( json_data, "flip_horizontal", JsonType::BOOLEAN, false ) )
    {
      if ( json_data["flip_horizontal"].asBool() )
        _flipFlag = (SDL_RendererFlip) (_flipFlag ^ SDL_FLIP_HORIZONTAL);
    }
    if ( validateJson( json_data, "flip_vertical", JsonType::BOOLEAN, false ) )
    {
      if ( json_data["flip_vertical"].asBool() )
        _flipFlag = (SDL_RendererFlip) (_flipFlag ^ SDL_FLIP_VERTICAL);
    }

    if ( validateJson( json_data, "start_number", JsonType::INTEGER, false ) )
    {
      setFrameNumber( json_data["start_number"].asInt() );
    }
    else
    {
      setFrameNumber( 0 );
    }

    DEBUG_STREAM << "Spritesheet::configure : " << _rawTexture->rows << "x" << _rawTexture->columns << " -> " << _rawTexture->cells << " start: " << _currentSprite;
  }

}

