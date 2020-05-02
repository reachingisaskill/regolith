
#include "ObjectFactory.h"

#include "Utilities.h"
#include "Manager.h"
#include "Texture.h"
#include "Sprite.h"
#include "FPSString.h"
#include "Button.h"

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
    return Manager::getInstance()->findRawTexture( name );
  }


  SDL_Renderer* ObjectFactory_base::getRenderer() const
  {
    return _parentBuilder->getRenderer();
  }


  Texture ObjectFactory_base::buildTexture( Json::Value& json_data ) const
  {
    Utilities::validateJson( json_data, "texture_name", Utilities::JSON_TYPE_STRING );

    std::string texture_name = json_data["texture_name"].asString();
    RawTexture raw_texture = findRawTexture( texture_name );

    Texture texture( raw_texture );

    if ( json_data.isMember( "number_rows" ) && json_data.isMember( "number_columns" ) )
    {
      int num_rows = json_data["number_rows"].asInt();
      int num_cols = json_data["number_columns"].asInt();
      int num_used;
      int period = 0;
      int start_num = 0;
      if ( json_data.isMember( "number_used_cells" ) )
      {
        num_used = json_data["number_used_cells"].asInt();
      }
      else
      {
        num_used = 0;
      }

      if ( json_data.isMember( "update_period" ) )
      {
        period = json_data["update_period"].asInt();
      }

      INFO_STREAM << "Configuring sprite: " << num_rows << "x" << num_cols << " -> " << num_used << " T = " << period << " start: " << start_num;

      texture.configure( num_rows, num_cols, num_used, period );

      if ( json_data.isMember( "start_number" ) )
      {
        start_num = json_data["start_number"].asInt();
      }
      texture.setSpriteNumber( start_num );
    }

    return texture;
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
  // Sprites

  SpriteFactory::SpriteFactory()
  {
  }


  SpriteFactory::~SpriteFactory()
  {
  }


  Drawable* SpriteFactory::build( Json::Value& json_data ) const
  {
    DEBUG_LOG( "Building Sprite" );

    Texture texture = buildTexture( json_data );

    // Configure the default collision
    Collision* collision = nullptr;
    if ( json_data.isMember( "collision" ) )
    {
      if ( json_data["collision"].isString() && json_data["collision"].asString() == "default" )
      {
        DEBUG_LOG( "Configuring default collision for sprite" );
        collision = new Collision( Vector( 0.0, 0.0 ), texture.getWidth(), texture.getHeight() );
      }
      else if ( json_data["collision"].isObject() )
      {
        collision = buildCollision( json_data["collision"] );
      }
    }

    Sprite* newSprite = new Sprite( texture );
    newSprite->addCollision( collision );

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


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Sprites

  ButtonFactory::ButtonFactory()
  {
  }


  ButtonFactory::~ButtonFactory()
  {
  }


  Drawable* ButtonFactory::build( Json::Value& json_data ) const
  {
    const char* state_names[Button::STATE_TOTAL] = { "state_normal", "state_focussed", "state_activated", "state_inactive" };
    Texture state_textures[Button::STATE_TOTAL];

    DEBUG_LOG( "Building Button" );
    // Only require the normal state to be present
    Utilities::validateJson( json_data, "state_normal", Utilities::JSON_TYPE_OBJECT );


    // Load the normal texture first
    Texture normal_texture = buildTexture( json_data[ state_names[0] ] );

    // Configure the default collision
    Collision* collision = nullptr;
    if ( json_data.isMember( "collision" ) )
    {
      if ( json_data["collision"].isString() && json_data["collision"].asString() == "default" )
      {
        DEBUG_LOG( "Configuring default collision for sprite" );
        collision = new Collision( Vector( 0.0, 0.0 ), normal_texture.getWidth(), normal_texture.getHeight() );
      }
      else if ( json_data["collision"].isObject() )
      {
        collision = buildCollision( json_data["collision"] );
      }
    }


//    // Load the button's name
//    Utilities::validateJson( json_data, "name", Utilities::JSON_TYPE_STRING );


    // Create the button object
    Button* newButton = new Button( normal_texture, collision );

    // Check and load the other textures
    for ( unsigned int state_number = 1; state_number < Button::STATE_TOTAL; ++state_number )
    {
      if ( json_data.isMember( state_names[ state_number ] ) )
      {
        newButton->setTexture( buildTexture( json_data[ state_names[state_number] ] ), (Button::State)state_number );
      }
    }

    // Set the rest of the drawable properties
    buildDrawable( newButton, json_data );

    return newButton;
  }
}

