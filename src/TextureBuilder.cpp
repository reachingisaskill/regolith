
#include "TextureBuilder.h"

#include "Exception.h"
#include "SpriteSheet.h"
#include "Manager.h"


namespace Regolith
{


  TextureBuilder::TextureBuilder( SDL_Renderer* rend ) :
    _theRenderer( rend )
  {
  }


  Texture* TextureBuilder::build( RawTexture texture, Json::Value jsonObject )
  {
    std::string object_type = jsonObject["texture_type"].asString();;

    if ( object_type == "simple_texture" )
    {
      return buildTexture( new Texture( texture ), jsonObject );
    }
    else if ( object_type == "spritesheet" )
    {
      return buildSpriteSheet( new SpriteSheet( texture ), jsonObject );
    }
    else if ( object_type == "animated" )
    {
    }


    Exception ex( "TextureBuilder::build()", "Non-implemented build type" );
    ex.addDetail( "TypeID", object_type );
    throw ex;
  }



  Texture* TextureBuilder::buildTexture( Texture* newTexture, Json::Value& json_data )
  {

    // TODO: Load some colour key info
//    if ( key != nullptr )
//    {
//      SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, key->r, key->g, key->b ) );
//    }

    // TODO: Load some clipping info...


    int pos_x = json_data["position"][0].asInt();
    int pos_y = json_data["position"][1].asInt();

    newTexture->_setClip( { 0, 0, newTexture->getWidth(), newTexture->getHeight() } );
    newTexture->setPosition( pos_x, pos_y );
    newTexture->_theRenderer = _theRenderer;

    return newTexture;
  }


  Texture* TextureBuilder::buildSpriteSheet( SpriteSheet* newTexture, Json::Value& json_data )
  {

    // TODO: Load some colour key info
//    if ( key != nullptr )
//    {
//      SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, key->r, key->g, key->b ) );
//    }

    // TODO: Load some clipping info...


    int pos_x = json_data["position"][0].asInt();
    int pos_y = json_data["position"][1].asInt();

    int num_rows = json_data["number_rows"].asInt();
    int num_cols = json_data["number_columns"].asInt();
    int num_used;
    if ( json_data.isMember( "number_used_cells" ) )
    {
      num_used = json_data["number_used_cells"].asInt();
    }
    else
    {
      num_used = 0;
    }

    int start_num = json_data["start_number"].asInt();

    newTexture->configure( num_rows, num_cols, num_used );
    newTexture->setPosition( pos_x, pos_y );
    newTexture->setSpriteNumber( start_num );
    newTexture->_theRenderer = _theRenderer;

    return newTexture;
  }

}

