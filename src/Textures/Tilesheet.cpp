
#include "Regolith/Textures/Tilesheet.h"
#include "Regolith/Handlers/DataHandler.h"
#include "Regolith/Utilities/TileSet.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  Tilesheet::Tilesheet() :
    Texture(),
    _rawTexture( nullptr ),
    _tiledSurface( nullptr ),
    _tiledTexture( nullptr ),
    _flipFlag( SDL_FLIP_NONE ),
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
    if ( _tiledTexture != nullptr )
    {
      SDL_DestroyTexture( _tiledTexture );
    }

    _tiledTexture = t;
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
    validateJson( json_data, "tile_set", JsonType::STRING );

    // Load the RawTexture pointer. Raw image data for tiles
    std::string texture_name = json_data["texture_name"].asString();
    _rawTexture = handler.getRawTexture( texture_name );
    DEBUG_STREAM << "Tilesheet::configure : Found tiles texture: " << texture_name << " : " << _rawTexture;

    // Build a TileSet object using the image data and the tile arrangement info.
    std::string tile_set = json_data["tile_set"].asString();
    TileSet the_tiles( tile_set );

    // Set the whole clip
    _clip.x = 0;
    _clip.y = 0;
    _clip.w = the_tiles.getWidth();
    _clip.h = the_tiles.getHeight();

    // Dimensions of the tile matrix
    size_t num_rows = the_tiles.getNumberRows();
    size_t num_cols = the_tiles.getNumberColumns();

    // Tile dimensions
    int width = the_tiles.getTileWidth();
    int height = the_tiles.getTileHeight();

    SDL_Rect src_rect = { 0, 0, (_rawTexture->width / _rawTexture->columns), (_rawTexture->height / _rawTexture->rows) };

    SDL_Rect dst_rect = { 0, 0, width, height };

    _tiledSurface = SDL_CreateRGBSurface( 0, the_tiles.getWidth(), the_tiles.getHeight(), REGOLITH_PIXEL_DEPTH, REGOLITH_R_MASK, REGOLITH_G_MASK, REGOLITH_B_MASK, REGOLITH_A_MASK );

    if ( _tiledSurface == nullptr )
    {
      Exception ex( "Tilesheet::configure()", "Could not create empty surface." );
      ex.addDetail( "Pixle Depth", REGOLITH_PIXEL_DEPTH );
      ex.addDetail( "Width", the_tiles.getWidth() );
      ex.addDetail( "Height", the_tiles.getHeight() );
      ex.addDetail( "SDL Error", SDL_GetError() );
      throw ex;
    }

    for ( size_t row = 0; row < num_rows; ++row )
    {
      for ( size_t col = 0; col < num_cols; ++col )
      {
        unsigned int num = the_tiles( row, col );

        if ( num > 0 )
        {
          num -= 1; // Now the index for the texture cell

          src_rect.x = (num % _rawTexture->columns) * src_rect.w;
          src_rect.y = (num / _rawTexture->columns) * src_rect.h;

          dst_rect.x = width*col;
          dst_rect.y = height*row;

          DEBUG_STREAM << "Tilesheet::configure : Blitting: " << src_rect.x << ", " << src_rect.y << ", " << src_rect.w << ", " << src_rect.h << "  ==>  " << dst_rect.x << ", " << dst_rect.y << ", " << dst_rect.w << ", " << dst_rect.h;
          
          SDL_BlitSurface( _rawTexture->surface, &src_rect, _tiledSurface, &dst_rect );
        }
      }
    }

    DEBUG_STREAM << "Tilesheet::configure : Tiled surface of " << _clip.w << "x" << _clip.h << " Pixels, @ " << _tiledSurface;
  }

}

