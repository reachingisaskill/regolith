
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


  Texture* TextureBuilder::build( Json::Value jsonObject )
  {
    std::string object_type = jsonObject["texture_type"].asString();;

    if ( object_type == "simple_texture" )
    {
      return buildTexture( jsonObject );
    }
    else if ( object_type == "text" )
    {
      return buildText( jsonObject );
    }
    else if ( object_type == "spritesheet" )
    {
      return buildSpriteSheet( jsonObject );
    }
    else if ( object_type == "animated" )
    {
    }


    Exception ex( "TextureBuilder::build()", "Non-implemented build type" );
    ex.addDetail( "TypeID", object_type );
    throw ex;
  }



  Texture* TextureBuilder::buildTexture( Json::Value& jsonObject )
  {
    std::string path = jsonObject["texture_path"].asString();
    // Load the image into a surface
    SDL_Texture* loadedTexture = nullptr;
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );

    if ( loadedSurface == nullptr )
    {
      Exception ex( "buildTexture()", "Could not load image data", false );
      ex.addDetail( "Image path", path );
      ex.addDetail( "SDL_img error", IMG_GetError() );
      throw ex;
    }

    // TODO: Load some colour key info
//    if ( key != nullptr )
//    {
//      SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, key->r, key->g, key->b ) );
//    }

    // TODO: Load some clipping info...

    loadedTexture = SDL_CreateTextureFromSurface( _theRenderer, loadedSurface );

    if ( loadedTexture == nullptr )
    {
      SDL_FreeSurface( loadedSurface );
      Exception ex( "buildTexture()", "Could not convert to texture", false );
      ex.addDetail( "Image path", path );
      ex.addDetail( "SDL error", SDL_GetError() );
      throw ex;
    }

    Texture* newTexture = new Texture();

    newTexture->_theTexture = loadedTexture;
    newTexture->_width = loadedSurface->w;
    newTexture->_height = loadedSurface->h;
    newTexture->_setClip( { 0, 0, loadedSurface->w, loadedSurface->h } );
    newTexture->_theRenderer = _theRenderer;

    SDL_FreeSurface( loadedSurface );
    return newTexture;
  }


  Texture* TextureBuilder::buildText( Json::Value& jsonObject )
  {
    Manager* man = Manager::getInstance();
    std::string text_string = jsonObject["texture_string"].asString();

    // Define the text colour
    SDL_Color color;
    if ( jsonObject.isMember( "colour" ) )
    {
      color.r = jsonObject["colour"][0].asInt();
      color.g = jsonObject["colour"][1].asInt();
      color.b = jsonObject["colour"][2].asInt();
      color.a = jsonObject["colour"][3].asInt();
    }
    else
    {
      color = man->getDefaultColour();
    }


    // Find the specified font
    TTF_Font* font = nullptr;
    if ( jsonObject.isMember( "font" ) )
    {
      font = man->getFontPointer( jsonObject["font"].asString() );
    }
    else
    {
      font = man->getDefaultFont();
    }


    // Render the text as TTF
    SDL_Surface* textSurface = TTF_RenderText_Solid( font, text_string.c_str(), color );
    if ( textSurface == nullptr )
    {
      Exception ex( "Texture::loadFromText()", "Could not render text", false );
      ex.addDetail( "Text string", text_string );
      ex.addDetail( "SDL_ttf error", TTF_GetError() );
      throw ex;
    }


    // Create the texture from the surface
    SDL_Texture* loadedTexture = SDL_CreateTextureFromSurface( _theRenderer, textSurface );
    if ( loadedTexture == nullptr )
    {
      // Remove before we throw
      SDL_FreeSurface( textSurface );
      // Throw the exception
      Exception ex( "Texture::loadFromText()", "Could not convert to texture", false );
      ex.addDetail( "Text string", text_string );
      ex.addDetail( "SDL_ttf error", TTF_GetError() );
      throw ex;
    }


    // Build the texture object
    Texture* newTexture = new Texture();

    newTexture->_theTexture = loadedTexture;
    newTexture->_width = textSurface->w;
    newTexture->_height = textSurface->h;
    newTexture->_setClip( { 0, 0, textSurface->w, textSurface->h } );
    newTexture->_theRenderer = _theRenderer;

    // Remove the unneeded surface
    SDL_FreeSurface( textSurface );
    return newTexture;
  }


  Texture* TextureBuilder::buildSpriteSheet( Json::Value& jsonObject )
  {
    std::string path = jsonObject["texture_path"].asString();

    // Load the image into a surface
    SDL_Texture* loadedTexture = nullptr;
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );

    if ( loadedSurface == nullptr )
    {
      Exception ex( "buildSpriteSheet()", "Could not load image data", false );
      ex.addDetail( "Image path", path );
      ex.addDetail( "SDL_img error", IMG_GetError() );
      throw ex;
    }

    // TODO: Load some colour key info
//    if ( key != nullptr )
//    {
//      SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, key->r, key->g, key->b ) );
//    }

    // TODO: Load some clipping info...

    loadedTexture = SDL_CreateTextureFromSurface( _theRenderer, loadedSurface );

    if ( loadedTexture == nullptr )
    {
      SDL_FreeSurface( loadedSurface );
      Exception ex( "buildSpriteSheet()", "Could not convert to texture", false );
      ex.addDetail( "Image path", path );
      ex.addDetail( "SDL error", SDL_GetError() );
      throw ex;
    }

    SpriteSheet* newTexture = new SpriteSheet();

    newTexture->_theTexture = loadedTexture;
    newTexture->_width = loadedSurface->w;
    newTexture->_height = loadedSurface->h;
    newTexture->_setClip( { 0, 0, loadedSurface->w, loadedSurface->h } );
    newTexture->_theRenderer = _theRenderer;

    SDL_FreeSurface( loadedSurface );
    return newTexture;
  }

}

