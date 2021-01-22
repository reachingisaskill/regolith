
#include "Regolith/Textures/Tilesheet.h"


namespace Regolith
{

  Tilesheet::Tilesheet() :
    Texture(),
    _rawTexture( nullptr ),
    _tiledSurface( nullptr ),
    _flipflag( SDL_FLIP_NONE ),
    _clip( {0, 0, 0, 0} ),
    _rotation( 0.0 )
  {
  }


  Tilesheet::~Tilesheet()
  {
  }


  void Tilesheet::setRenderedTexture( SDL_Texture* t )
  {
    // Check and destroy if one already exists
    if ( _rawTexture->sdl_texture != nullptr )
    {
      SDL_DestroyTexture( _rawTexture->sdl_texture );
    }

    _rawTexture->sdl_texture = t;
  }


  void Tilesheet::clearSDLTexture()
  {
    if ( _rawTexture->sdl_texture != nullptr )
    {
      SDL_DestroyTexture( _rawTexture->sdl_texture );
      _rawTexture->sdl_texture = nullptr;
    }
  }


  void Tilesheet::configure( Json::Value& json_data, DataHandler& handler )
  {
    validateJson( json_data, "texture_name", JsonType::STRING );
    validateJson( json_data, "tile_set", jsonType::STRING );

    // Load the RawTexture pointer. Raw image data for tiles
    std::string texture_name = json_data["texture_name"].asString();
    _rawTexture = handler.getRawTexture( texture_name );
    DEBUG_STREAM << "Tilesheet::configure : Found tiles texture: " << texture_name << " : " << _rawTexture;

    // Build a TileSet object using the image data and the tile arrangement info.
    std::string tile_set = json_data["tile_set"].asString();
    TileSet the_tiles( tile_set, _rawTexture );

    // Build the surface
    _tiledSurface = the_tiles.buildSurface();

    _clip.x = 0;
    _clip.y = 0;
    _clip.w = _tiledSurface->w;
    _clip.h = _tiledSurface->h;

    DEBUG_STREAM << "Tilesheet::configure : Tiled surface of " << _clip.w << "x" << _clip.h << " Pixels";
  }

}

