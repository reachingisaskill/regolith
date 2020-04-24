
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


  void ObjectFactory_base::buildDrawable( Drawable* object, Json::Value& json_data ) const
  {

    // Set the default position
    Vector pos;
    if ( json_data.isMember( "position" ) )
    {
      float pos_x = json_data["position"][0].asFloat();
      float pos_y = json_data["position"][1].asFloat();
      pos = Vector( pos_x, pos_y );
    }
    else
      pos = Vector( 0.0, 0.0 );
    DEBUG_STREAM << "Position = " << pos;

    // Does the object have mass?
    float mass = 0.0;
    if ( json_data.isMember( "mass" ) )
    {
      Utilities::validateJson( json_data, "mass", Utilities::JSON_TYPE_FLOAT );
      mass = std::fabs( json_data["mass"].asFloat() );
    }
    DEBUG_STREAM << "Mass = " << mass;


    // Is there a default velocity
    Vector velocity( 0.0, 0.0 );
    if ( json_data.isMember( "velocity" ) && json_data["velocity"].isArray() )
    {
      velocity.x() = json_data["velocity"][0].asFloat();
      velocity.y() = json_data["velocity"][1].asFloat();
    }
    DEBUG_STREAM << "Initial Velocity = " << velocity;


    object->setPosition( pos );
    object->setMass( mass );
    object->setVelocity( velocity );
    object->setRenderer( getRenderer() );
  }


  Collision* ObjectFactory_base::buildCollision( Json::Value& collision_data ) const
  {
    // Set the collision data
    Utilities::validateJson( collision_data, "position", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJson( collision_data, "width", Utilities::JSON_TYPE_FLOAT );
    Utilities::validateJson( collision_data, "height", Utilities::JSON_TYPE_FLOAT );

    float col_x = collision_data["position"][0].asFloat();
    float col_y = collision_data["position"][1].asFloat();

    Collision* collision = new Collision( Vector( col_x, col_y ), collision_data["width"].asFloat(), collision_data["height"].asFloat() );
    return collision;
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
    DEBUG_LOG( "Building Sprite" );
    Utilities::validateJson( json_data, "texture_name", Utilities::JSON_TYPE_STRING );

    std::string texture_name = json_data["texture_name"].asString();
    RawTexture texture = findRawTexture( texture_name );

    SpriteSheet sheet( texture );


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

    // Configure the default collision
    Collision* collision = nullptr;
    if ( json_data.isMember( "collision" ) )
    {
      if ( json_data["collision"].isString() && json_data["collision"].asString() == "default" )
      {
        DEBUG_LOG( "Configuring default collision for sprite" );
        collision = new Collision( Vector( 0.0, 0.0 ), sheet.getWidth(), sheet.getHeight() );
      }
      else if ( json_data["collision"].isObject() )
      {
        collision = buildCollision( json_data["collision"] );
      }
    }

    Sprite* newSprite = new Sprite( sheet );
    newSprite->setCollision( collision );

    buildDrawable( newSprite, json_data );

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

    // Refresh timing
    int update_rate = json_data["update_rate"].asInt();

    // Configure the default collision
    Collision* collision = nullptr;
    if ( json_data.isMember( "collision" ) )
    {
      if ( json_data["collision"].isString() && json_data["collision"].asString() == "default" )
      {
        DEBUG_LOG( "Configuring default collision for sprite" );
        collision = new Collision( Vector( 0.0, 0.0 ), sheet.getWidth(), sheet.getHeight() );
      }
      else if ( json_data["collision"].isObject() )
      {
        collision = buildCollision( json_data["collision"] );
      }
    }


    AnimatedSprite* newSprite = new AnimatedSprite( sheet, update_rate );
    newSprite->setCollision( collision );

    buildDrawable( newSprite, json_data );

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
    newSprite->setFont( font );
    newSprite->setColor( color );
    newSprite->updateString();

    buildDrawable( newSprite, json_data );

    return newSprite;
  }

}

