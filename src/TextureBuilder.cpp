
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



  Texture* TextureBuilder::buildTexture( Texture* newTexture, Json::Value& )
  {

    // TODO: Load some colour key info
//    if ( key != nullptr )
//    {
//      SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, key->r, key->g, key->b ) );
//    }

    // TODO: Load some clipping info...



    newTexture->_setClip( { 0, 0, newTexture->getWidth(), newTexture->getHeight() } );
    newTexture->_theRenderer = _theRenderer;

    return newTexture;
  }


  Texture* TextureBuilder::buildSpriteSheet( SpriteSheet* newTexture, Json::Value& )
  {

    // TODO: Load some colour key info
//    if ( key != nullptr )
//    {
//      SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, key->r, key->g, key->b ) );
//    }

    // TODO: Load some clipping info...


    newTexture->_setClip( { 0, 0, newTexture->getWidth(), newTexture->getHeight() } );
    newTexture->_theRenderer = _theRenderer;

    return newTexture;
  }

}

