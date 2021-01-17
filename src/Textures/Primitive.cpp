
#include "Regolith/Textures/Primitive.h"
#include "Regolith/Handlers/DataHandler.h"
#include "Regolith/Utilities/JsonValidation.h"

#include <cmath>


namespace Regolith
{


////////////////////////////////////////////////////////////////////////////////////////////////////  
  // Primitive Member function definitions

  Primitive::Primitive() :
    Texture(),
    _update( false ),
    _theTexture( nullptr ),
    _theSurface( nullptr ),
    _clip( { 0, 0, 0, 0 } ),
    _flipFlag( SDL_FLIP_NONE ),
//    _center( { 0, 0 } ),
    _rotation( 0.0 ),
    _blendMod( SDL_BLENDMODE_NONE ),
    _alphaMod( 255 ),
    _colourMod( { 0, 0, 0, 0 } )
  {
  }


  Primitive::~Primitive()
  {
    if ( _theSurface != nullptr )
    {
      SDL_FreeSurface( _theSurface );
      _theSurface = nullptr;
    }
  }


  void Primitive::setRenderedTexture( SDL_Texture* t )
  {
    // Check and destroy if one already exists
    if ( _theTexture != nullptr )
    {
      SDL_DestroyTexture( _theTexture );
    }

    _theTexture = t;

    if ( _blendMod != SDL_BLENDMODE_NONE )
    {
      SDL_SetTextureBlendMode( _theTexture, _blendMod );
    }
    if ( _alphaMod != 255 )
    {
      SDL_SetTextureAlphaMod( _theTexture, _alphaMod );
    }
    if ( _colourMod.a != 0 )
    {
      SDL_SetTextureColorMod( _theTexture, _colourMod.r, _colourMod.g, _colourMod.b );
    }

    _update = false;
  }


  void Primitive::clearSDLTexture()
  {
    if ( _theTexture != nullptr )
    {
      SDL_DestroyTexture( _theTexture );
      _theTexture = nullptr;
    }
  }


  void Primitive::setAlpha( Uint8 alpha )
  {
    _alphaMod = alpha;
    SDL_SetTextureAlphaMod( _theTexture, _alphaMod );
  }


  void Primitive::configure( Json::Value& json_data, DataHandler& /*handler*/ )
  {
    _update = true;

    validateJson( json_data, "shape", JsonType::STRING );
    std::string shape = json_data["shape"].asString();

    if ( shape == "square" || shape == "box" )
    {
      validateJson( json_data, "width", JsonType::INTEGER );
      validateJson( json_data, "height", JsonType::INTEGER );

      validateJson( json_data, "colour", JsonType::ARRAY );
      validateJsonArray( json_data["colour"], 3, JsonType::INTEGER );

      int red = json_data["colour"][0].asInt();
      int green = json_data["colour"][1].asInt();
      int blue = json_data["colour"][2].asInt();

      _clip.x = 0;
      _clip.y = 0;
      _clip.w = json_data["width"].asInt();
      _clip.h = json_data["height"].asInt();

//      _center.x = 0.5*_clip.w;
//      _center.y = 0.5*_clip.h;

      _theSurface = SDL_CreateRGBSurface( 0, _clip.w, _clip.h, REGOLITH_PIXEL_DEPTH, REGOLITH_R_MASK, REGOLITH_G_MASK, REGOLITH_B_MASK, REGOLITH_A_MASK );

      if ( _theSurface == nullptr )
      {
        Exception ex( "Primitive::configure()", "Could not create SDL Surface." );
        ex.addDetail( "Shape", shape );
        ex.addDetail( "Width", _clip.w );
        ex.addDetail( "Height", _clip.h );
        ex.addDetail( "SDL Error", SDL_GetError() );
        throw ex;
      }

      if ( SDL_FillRect( _theSurface, nullptr, SDL_MapRGB( _theSurface->format, red, green, blue ) ) )
      {
        Exception ex( "Primitive::configure()", "SDL_FillRect() Failed." );
        ex.addDetail( "Shape", shape );
        ex.addDetail( "Width", _clip.w );
        ex.addDetail( "Height", _clip.h );
        ex.addDetail( "SDL Error", SDL_GetError() );
        throw ex;
      }

      DEBUG_STREAM << "Primitive::configure : " << shape << " : Surface created : " << _theSurface->w << ", " << _theSurface->h << " - " << red << ", " << green << ", " << blue;
    }
    else
    {
      Exception ex( "Primitive::configure()", "Unknown or unsupported primitive shape requested." );
      ex.addDetail( "Shape", shape );
      throw ex;
    }


    if ( validateJson( json_data, "blend_mode", JsonType::STRING, false ) )
    {
      if ( json_data["blend_mode"].asString() == "blend" )
      {
        _blendMod = SDL_BLENDMODE_BLEND;
      }
      else if ( json_data["blend_mode"].asString() == "add" )
      {
        _blendMod = SDL_BLENDMODE_ADD;
      }
      else if ( json_data["blend_mode"].asString() == "mod" )
      {
        _blendMod = SDL_BLENDMODE_MOD;
      }
      else if ( json_data["blend_mode"].asString() != "none" )
      {
        WARN_STREAM << "Primitive::configure : Unknown blend mode requested : " << json_data["blend_mode"].asString();
      }
    }

    if ( validateJson( json_data, "alpha", JsonType::INTEGER, false ) )
    {
      _alphaMod = (Uint8)json_data["alpha"].asInt();
    }

    if ( validateJson( json_data, "colour_mod", JsonType::ARRAY, false ) )
    {
      validateJsonArray( json_data["colour_mod"], 3, JsonType::INTEGER );
      _colourMod.r = json_data["colour_mode"][0].asInt();
      _colourMod.g = json_data["colour_mode"][1].asInt();
      _colourMod.b = json_data["colour_mode"][2].asInt();
      // Signal that it's been changed with the alpha channel
      _colourMod.a = 1;
    }

    if ( validateJson( json_data, "rotation", JsonType::FLOAT, false ) )
    {
      _rotation = json_data["rotation"].asFloat();
    }

    DEBUG_STREAM << "Primitive::configure : Primitive " << shape << " configured.";
  }

}

