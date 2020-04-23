
#include "ObjectFactory.h"

#include "Utilities.h"
#include "Manager.h"
#include "ObjectBuilder.h"
#include "Scene.h"
#include "Texture.h"
#include "SpriteSheet.h"
#include "AnimatedSprite.h"
#include "FPSString.h"

#include "logtastic.h"


namespace Regolith
{

  // Base class
  ObjectFactory_base::ObjectFactory_base()
  {
  }


  ObjectFactory_base::~ObjectFactory_base()
  {
  }


  RawTexture ObjectFactory_base::findRawTexture( std::string name ) const
  {
    return _parentBuilder->getScene()->findRawTexture( name );
  }


  SDL_Renderer* ObjectFactory_base::getRenderer() const
  {
    return _parentBuilder->getRenderer();
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
//  // Standard textures
//  SimpleFactory::SimpleFactory()
//  {
//  }
//
//
//  SimpleFactory::~SimpleFactory()
//  {
//  }
//
//
//  Drawable* SimpleFactory::build( Json::Value& json_data ) const
//  {
//    std::string texture_name = json_data["texture_name"].asString();
//    Texture* newTexture = new Texture( findRawTexture( texture_name ) );
//
//    int pos_x = json_data["position"][0].asInt();
//    int pos_y = json_data["position"][1].asInt();
//
//    newTexture->setClip( { 0, 0, newTexture->getWidth(), newTexture->getHeight() } );
//    newTexture->setPosition( pos_x, pos_y );
//    newTexture->setRenderer( getRenderer() );
//
//    return newTexture;
//  }



////////////////////////////////////////////////////////////////////////////////////////////////////
  // SpriteSheets
  SpriteFactory::SpriteFactory()
  {
  }


  SpriteFactory::~SpriteFactory()
  {
  }


  Drawable* SpriteFactory::build( Json::Value& json_data ) const
  {
    Utilities::validateJson( json_data, "texture_name", Utilities::JSON_TYPE_STRING );

    std::string texture_name = json_data["texture_name"].asString();
    RawTexture texture = findRawTexture( texture_name );

    SpriteSheet sheet( texture );

    Vector pos;
    if ( json_data.isMember( "position" ) )
    {
      float pos_x = json_data["position"][0].asFloat();
      float pos_y = json_data["position"][1].asFloat();
      pos = Vector( pos_x, pos_y );
    }
    else
      pos = Vector( 0.0, 0.0 );

    if ( json_data.isMember( "number_rows" ) && json_data.isMember( "number_columns" ) )
    {
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
      sheet.configure( num_rows, num_cols, num_used );
    }
    else
    {
      // Only one sprite on the texture
      sheet.configure( 1, 1, 1 );
    }

    if ( json_data.isMember( "start_number" ) )
    {
      int start_num = json_data["start_number"].asInt();
      sheet.setSpriteNumber( start_num );
    }

    Sprite* newSprite = new Sprite( sheet );
    newSprite->setPosition( pos );
    newSprite->setRenderer( getRenderer() );

    return newSprite;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
// Animated SpriteSheets
  AnimatedFactory::AnimatedFactory()
  {
  }


  AnimatedFactory::~AnimatedFactory()
  {
  }


  Drawable* AnimatedFactory::build( Json::Value& json_data ) const
  {
    Utilities::validateJson( json_data, "texture_name", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "number_rows", Utilities::JSON_TYPE_FLOAT );
    Utilities::validateJson( json_data, "number_columns", Utilities::JSON_TYPE_FLOAT );
    Utilities::validateJson( json_data, "update_rate", Utilities::JSON_TYPE_FLOAT );

    std::string texture_name = json_data["texture_name"].asString();
    RawTexture texture = findRawTexture( texture_name );

    SpriteSheet sheet( texture );

    Vector pos;
    if ( json_data.isMember( "position" ) )
    {
      float pos_x = json_data["position"][0].asFloat();
      float pos_y = json_data["position"][1].asFloat();
      pos = Vector( pos_x, pos_y );
    }
    else
      pos = Vector( 0.0, 0.0 );


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

    DEBUG_LOG( "Configuring sprite sheet" );
    sheet.configure( num_rows, num_cols, num_used );

    if ( json_data.isMember( "start_number" ) )
    {
      int start_num = json_data["start_number"].asInt();
      sheet.setSpriteNumber( start_num );
    }

    int update_rate = json_data["update_rate"].asInt();

    AnimatedSprite* newSprite = new AnimatedSprite( sheet, update_rate );
    newSprite->setPosition( pos );
    newSprite->setRenderer( getRenderer() );

    return newSprite;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
// Updating FPS string
  FPSStringFactory::FPSStringFactory()
  {
  }


  FPSStringFactory::~FPSStringFactory()
  {
  }


  Drawable* FPSStringFactory::build( Json::Value& json_data ) const
  {
    Manager* man = Manager::getInstance();

    INFO_LOG( "Building FPS String" );
    Vector pos;
    if ( json_data.isMember( "position" ) )
    {
      float pos_x = json_data["position"][0].asFloat();
      float pos_y = json_data["position"][1].asFloat();
      pos = Vector( pos_x, pos_y );
    }
    else
      pos = Vector( 0.0, 0.0 );


    int frameCount = json_data["frame_count"].asInt();

    TTF_Font* font = nullptr;
    if ( json_data.isMember( "font" ) )
    {
      font = man->getFontPointer( json_data["font"].asString() );
      INFO_STREAM << "Using font: " << json_data["font"].asString();
    }
    else
    {
      font = man->getDefaultFont();
      INFO_LOG( "Using default font" );
    }

    SDL_Color color;
    if ( json_data.isMember( "colour" ) )
    {
      color.r = json_data["colour"][0].asInt();
      color.g = json_data["colour"][1].asInt();
      color.b = json_data["colour"][2].asInt();
      color.a = json_data["colour"][3].asInt();
    }
    else
    {
      color = man->getDefaultColour();
    }


    FPSString* newSprite = new FPSString();

    newSprite->setFrameCount( frameCount );
    newSprite->setPosition( pos );
    newSprite->setFont( font );
    newSprite->setColor( color );
    newSprite->setRenderer( getRenderer() );

    newSprite->updateString();

    return newSprite;
  }

}

