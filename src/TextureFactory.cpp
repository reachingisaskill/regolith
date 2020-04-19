
#include "TextureFactory.h"

#include "Manager.h"
#include "TextureBuilder.h"
#include "Scene.h"
#include "Texture.h"
#include "SpriteSheet.h"
#include "AnimatedSprite.h"
#include "FPSString.h"

#include "logtastic.h"


namespace Regolith
{

  // Base class
  TextureFactory_base::TextureFactory_base()
  {
  }


  TextureFactory_base::~TextureFactory_base()
  {
  }


  RawTexture TextureFactory_base::findRawTexture( std::string name ) const
  {
    return _parentBuilder->getScene()->findRawTexture( name );
  }


  SDL_Renderer* TextureFactory_base::getRenderer() const
  {
    return _parentBuilder->getRenderer();
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Standard textures
  SimpleFactory::SimpleFactory()
  {
  }


  SimpleFactory::~SimpleFactory()
  {
  }


  Drawable* SimpleFactory::build( Json::Value& json_data ) const
  {
    std::string texture_name = json_data["texture_name"].asString();
    Texture* newTexture = new Texture( findRawTexture( texture_name ) );

    int pos_x = json_data["position"][0].asInt();
    int pos_y = json_data["position"][1].asInt();

    newTexture->setClip( { 0, 0, newTexture->getWidth(), newTexture->getHeight() } );
    newTexture->setPosition( pos_x, pos_y );
    newTexture->setRenderer( getRenderer() );

    return newTexture;
  }



////////////////////////////////////////////////////////////////////////////////////////////////////
  // SpriteSheets
  SpriteSheetFactory::SpriteSheetFactory()
  {
  }


  SpriteSheetFactory::~SpriteSheetFactory()
  {
  }


  Drawable* SpriteSheetFactory::build( Json::Value& json_data ) const
  {
    std::string texture_name = json_data["texture_name"].asString();
    SpriteSheet* newTexture = new SpriteSheet( findRawTexture( texture_name ) );

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
    newTexture->setRenderer( getRenderer() );

    return newTexture;
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
    std::string texture_name = json_data["texture_name"].asString();
    AnimatedSprite* newTexture = new AnimatedSprite( findRawTexture( texture_name ) );

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
    int updateRate = json_data["update_rate"].asInt();

    newTexture->configure( num_rows, num_cols, num_used );
    newTexture->setUpdateRate( updateRate );
    newTexture->setPosition( pos_x, pos_y );
    newTexture->setSpriteNumber( start_num );
    newTexture->setRenderer( getRenderer() );

    return newTexture;
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
    std::string texture_name = json_data["texture_name"].asString();
    FPSString* newTexture = new FPSString();

    int pos_x = json_data["position"][0].asInt();
    int pos_y = json_data["position"][1].asInt();

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

    newTexture->setFrameCount( frameCount );
    newTexture->setPosition( pos_x, pos_y );
    newTexture->setFont( font );
    newTexture->setColor( color );
    newTexture->setRenderer( getRenderer() );

    newTexture->updateString();

    return newTexture;
  }

}

