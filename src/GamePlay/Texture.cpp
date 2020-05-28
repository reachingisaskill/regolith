//#define LOGTASTIC_DEBUG_OFF

#include "Regolith/GamePlay/Texture.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Utilities/JsonValidation.h"

#include "logtastic.h"

#include <cmath>


namespace Regolith
{

  Texture::Texture() :
    _theTexture( { nullptr, 0, 0 } ),
    _angle( 0.0 ),
    _flipFlag( SDL_FLIP_NONE ),
    _clip( { 0, 0, 0, 0 } ),
    _spriteWidth( 0 ),
    _spriteHeight( 0 ),
    _currentSprite( 0 ),
    _rows( 0 ),
    _columns( 0 ),
    _numSprites( 0 ),
    _updatePeriod( 0 ),
    _count( 0 )
  {
  }


  Texture::Texture( RawTexture tex) :
    _theTexture( tex ),
    _angle( 0.0 ),
    _flipFlag( SDL_FLIP_NONE ),
    _clip( { 0, 0, tex.width, tex.height } ),
    _spriteWidth( tex.width ),
    _spriteHeight( tex.height ),
    _currentSprite( 0 ),
    _rows( 0 ),
    _columns( 0 ),
    _numSprites( 0 ),
    _updatePeriod( 0 ),
    _count( 0 )
  {
  }


  Texture::~Texture()
  {
  }


  void Texture::draw( SDL_Rect* destination )
  {
    SDL_Renderer* renderer = Manager::getInstance()->getRendererPointer();
    // Render it to the window
    DEBUG_STREAM << "Texture::Draw : " << renderer << ", " << _theTexture.texture << ", " << destination->w << ", " << destination->h;
    SDL_RenderCopyEx( renderer, _theTexture.texture, &_clip, destination, _angle, nullptr, _flipFlag );
  }


  void Texture::setClip( SDL_Rect c )
  {
    _spriteWidth = c.w;
    _spriteHeight = c.h;
    _clip = c;
  }


  void Texture::setColor( Uint8 red, Uint8 green, Uint8 blue )
  {
    SDL_SetTextureColorMod( _theTexture.texture, red, green, blue );
  }


  void Texture::setAlpha( Uint8 alpha )
  {
    SDL_SetTextureAlphaMod( _theTexture.texture, alpha );
  }


  void Texture::setBlendMode( SDL_BlendMode blendmode )
  {
    SDL_SetTextureBlendMode( _theTexture.texture, blendmode );
  }


  void Texture::setRotation( double angle )
  {
    _angle = angle;
  }


  void Texture::setFlip( SDL_RendererFlip flip )
  {
    _flipFlag = flip;
  }


  void Texture::setSpriteNumber( int num )
  {
    _currentSprite = num;
    int sprite_x = (_currentSprite % _columns) * _spriteWidth;
    int sprite_y = (_currentSprite / _columns) * _spriteHeight;

    _clip.x = sprite_x;
    _clip.y = sprite_y;
    _clip.w = _spriteWidth;
    _clip.h = _spriteHeight;
  }


  void Texture::update( float timestep )
  {
    _count += timestep;

    int frame_number = _currentSprite;

    frame_number += _count / _updatePeriod;
    frame_number = frame_number % _numSprites;
    this->setSpriteNumber( frame_number );
    _count = std::fmod( _count, _updatePeriod );

    DEBUG_STREAM << "Sprite Animation : _count : " << _count << ", frame No. : " << frame_number << ", update rate : " << _updatePeriod;
  }


  void Texture::configure( Json::Value& json_data )
  {
    Utilities::validateJson( json_data, "texture_name", Utilities::JSON_TYPE_STRING );

    std::string texture_name = json_data["texture_name"].asString();
    _theTexture = Manager::getInstance()->findRawTexture( texture_name );
    INFO_STREAM << "Found texture: " << texture_name;

    // If its a spritesheet
    if ( Utilities::validateJson( json_data, "number_rows", Utilities::JSON_TYPE_INTEGER, false ) &&
          Utilities::validateJson( json_data, "number_columns", Utilities::JSON_TYPE_INTEGER, false ) )
    {
      _rows = json_data["number_rows"].asInt();
      _columns = json_data["number_columns"].asInt();
      if ( json_data.isMember( "number_used_cells" ) )
      {
        _numSprites = json_data["number_used_cells"].asInt();
      }
      else
      {
        _numSprites = _rows * _columns;
      }

      if ( json_data.isMember( "update_period" ) )
      {
        _updatePeriod = json_data["update_period"].asFloat();
      }

      if ( json_data.isMember( "start_number" ) )
      {
        setSpriteNumber( json_data["start_number"].asInt() );
      }
      else
      {
        setSpriteNumber( 0 );
      }
    }
    else
    {
      _columns = 1;
      _rows = 1;
      _numSprites = 1;
    }

    _spriteWidth = _theTexture.width / _columns;
    _spriteHeight = _theTexture.height / _rows;

    if ( Utilities::validateJson( json_data, "clip", Utilities::JSON_TYPE_ARRAY, false ) )
    {
      Utilities::validateJsonArray( json_data["clip"], Utilities::JSON_TYPE_INTEGER );
      int x = json_data["clip"][0].asInt();
      int y = json_data["clip"][1].asInt();
      int w = json_data["clip"][2].asInt();
      int h = json_data["clip"][3].asInt();
      _clip.x = x;
      _clip.y = y;
      _clip.w = w;
      _clip.h = h;
    }
    else
    {
      _clip.x = 0;
      _clip.y = 0;
      _clip.w = _spriteWidth;
      _clip.h = _spriteHeight;
    }

    INFO_STREAM << "Configuring texture: " << _rows << "x" << _columns << " -> " << _numSprites << " T = " << _updatePeriod << " start: " << _currentSprite;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Useful functions

  RawTexture makeTextureFromText( TTF_Font* font, std::string textureString, SDL_Color color )
  {
    static SDL_Renderer* renderer = Manager::getInstance()->getRendererPointer();
    RawTexture theTexture;

    SDL_Surface* textSurface = TTF_RenderText_Solid( font, textureString.c_str(), color );
    if ( textSurface == nullptr )
    {
      Exception ex( "Scene::_addTextureFromText()", "Could not render text", true );
      ex.addDetail( "Text string", textureString );
      ex.addDetail( "SDL_ttf error", TTF_GetError() );
      throw ex;
    }

    // Create the texture from the surface
    SDL_Texture* loadedTexture = SDL_CreateTextureFromSurface( renderer, textSurface );
    if ( loadedTexture == nullptr )
    {
      // Remove before we throw
      SDL_FreeSurface( textSurface );
      // Throw the exception
      Exception ex( "Scene::_addTextureFromText()", "Could not convert to texture", true );
      ex.addDetail( "Text string", textureString );
      ex.addDetail( "SDL_ttf error", TTF_GetError() );
      throw ex;
    }

    // Fill the RawTexture object
    theTexture.texture = SDL_CreateTextureFromSurface( renderer, textSurface );
    theTexture.width = textSurface->w;
    theTexture.height = textSurface->h;

    // Remove the unneeded surface
    SDL_FreeSurface( textSurface );

    return theTexture;
  }


  RawTexture makeTextureFromText( Json::Value& json_data )
  {
    Manager* man = Manager::getInstance();

    Utilities::validateJson( json_data, "name", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "text", Utilities::JSON_TYPE_STRING );

    std::string name = json_data["name"].asString();
    std::string text_string = json_data["text"].asString();
    INFO_STREAM << "Creating and loaded text texture. Name: " << name;

    // Define the text colour
    SDL_Color color;
    if ( Utilities::validateJson( json_data, "colour", Utilities::JSON_TYPE_ARRAY, false ) )
    {
      Utilities::validateJsonArray( json_data["colour"], 4, Utilities::JSON_TYPE_INTEGER );

      color.r = json_data["colour"][0].asInt();
      color.g = json_data["colour"][1].asInt();
      color.b = json_data["colour"][2].asInt();
      color.a = json_data["colour"][3].asInt();
    }
    else
    {
      color = man->getDefaultColour();
    }

    // Find the specified font
    TTF_Font* font = nullptr;
    if ( Utilities::validateJson( json_data, "font", Utilities::JSON_TYPE_STRING, false ) )
    {
      font = man->getFontPointer( json_data["font"].asString() );
      INFO_STREAM << "Creating text using font: " << json_data["font"].asString();
    }
    else
    {
      font = man->getDefaultFont();
      INFO_LOG( "Using default font" );
    }

    return makeTextureFromText( font, text_string, color );
  }


  RawTexture makeTextureFromFile( Json::Value& json_data )
  {
    Manager* man = Manager::getInstance();

    Utilities::validateJson( json_data, "name", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "path", Utilities::JSON_TYPE_STRING );

    std::string name = json_data["name"].asString();
    std::string path = json_data["path"].asString();

    INFO_STREAM << "Creating and loading image texture. Name: " << name;

    // Load the image into a surface
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
    if ( loadedSurface == nullptr )
    {
      Exception ex( "Scene::addTextureFromFile()", "Could not load image data", false );
      ex.addDetail( "Image path", path );
      ex.addDetail( "SDL_img error", IMG_GetError() );
      throw ex;
    }

    // If there is a colour key, apply it
    if ( json_data.isMember("colour_key") )
    {
      int key_red = json_data["colour_key"][0].asInt();
      int key_green = json_data["colour_key"][1].asInt();
      int key_blue = json_data["colour_key"][2].asInt();
      SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, key_red, key_green, key_blue ) );
    }

    // Create SDL_Texture
    SDL_Texture* loadedTexture = SDL_CreateTextureFromSurface( man->getRendererPointer(), loadedSurface );
    if ( loadedTexture == nullptr )
    {
      SDL_FreeSurface( loadedSurface );
      Exception ex( "Scene::addTextureFromFile()", "Could not convert to texture", false );
      ex.addDetail( "Image path", path );
      ex.addDetail( "SDL error", SDL_GetError() );
      throw ex;
    }

    RawTexture theTexture = { loadedTexture, loadedSurface->w, loadedSurface->h };
    SDL_FreeSurface( loadedSurface );

    return theTexture;
  }

}
