
#include "TextureBuilder.h"

#include "Exception.h"
#include "Texture.h"
#include "SpriteSheet.h"
#include "AnimatedSprite.h"
#include "Manager.h"

#include "logtastic.h"


namespace Regolith
{

  TextureBuilder::TextureBuilder() :
    _factories(),
    _theRenderer()
  {
  }


  TextureBuilder::TextureBuilder( SDL_Renderer* rend ) :
    _factories(),
    _theRenderer( rend )
  {
  }


  TextureBuilder::~TextureBuilder()
  {
    // Delete the individual factories
    for ( FactoryMap::iterator it = _factories.begin(); it != _factories.end(); ++it )
    {
      delete it->second;
    }
    _factories.clear();
  }


  void TextureBuilder::setRenderer( SDL_Renderer* rend )
  {
    _theRenderer = rend;
  }


  void TextureBuilder::setScene( const Scene* scene )
  {
    _theScene = scene;
  }


  void TextureBuilder::addFactory( TextureFactory_base* factory )
  {
    std::string name( factory->getTextureName() );

    if ( _factories.find( name ) != _factories.end() )
    {
      ERROR_LOG( "Attempting to add factory with a name that is already loaded" );
      return;
    }

    factory->_parentBuilder = this;

    _factories[name] = factory;
  }


  Drawable* TextureBuilder::build( Json::Value jsonObject )
  {
    std::string object_type = jsonObject["texture_type"].asString();
    INFO_STREAM << "Building texture of type : " << object_type;

    FactoryMap::iterator found = _factories.find( object_type );

    if ( found == _factories.end() )
    {
      ERROR_LOG( "Texture type not found in factory list" );
      Exception ex( "TextureBuilder::build()", "Non-implemented build type", true );
      ex.addDetail( "TypeID", object_type );
      throw ex;
    }

    return found->second->build( jsonObject );
  }



//  Texture* TextureBuilder::buildTexture( Texture* newTexture, Json::Value& json_data )
//  {
//
//    // TODO: Load some clipping info...
//
//    int pos_x = json_data["position"][0].asInt();
//    int pos_y = json_data["position"][1].asInt();
//
//    newTexture->_setClip( { 0, 0, newTexture->getWidth(), newTexture->getHeight() } );
//    newTexture->setPosition( pos_x, pos_y );
//    newTexture->_theRenderer = _theRenderer;
//
//    return newTexture;
//  }
//
//
//  Texture* TextureBuilder::buildSpriteSheet( SpriteSheet* newTexture, Json::Value& json_data )
//  {
//    int pos_x = json_data["position"][0].asInt();
//    int pos_y = json_data["position"][1].asInt();
//
//    int num_rows = json_data["number_rows"].asInt();
//    int num_cols = json_data["number_columns"].asInt();
//    int num_used;
//    if ( json_data.isMember( "number_used_cells" ) )
//    {
//      num_used = json_data["number_used_cells"].asInt();
//    }
//    else
//    {
//      num_used = 0;
//    }
//
//    int start_num = json_data["start_number"].asInt();
//
//    newTexture->configure( num_rows, num_cols, num_used );
//    newTexture->setPosition( pos_x, pos_y );
//    newTexture->setSpriteNumber( start_num );
//    newTexture->_theRenderer = _theRenderer;
//
//    return newTexture;
//  }
//
//
//  Texture* TextureBuilder::buildAnimatedSprite( AnimatedSprite* newTexture, Json::Value& json_data )
//  {
//    int pos_x = json_data["position"][0].asInt();
//    int pos_y = json_data["position"][1].asInt();
//
//    int num_rows = json_data["number_rows"].asInt();
//    int num_cols = json_data["number_columns"].asInt();
//    int num_used;
//    if ( json_data.isMember( "number_used_cells" ) )
//    {
//      num_used = json_data["number_used_cells"].asInt();
//    }
//    else
//    {
//      num_used = 0;
//    }
//
//    int start_num = json_data["start_number"].asInt();
//    int updateRate = json_data["update_rate"].asInt();
//
//    newTexture->configure( num_rows, num_cols, num_used );
//    newTexture->setUpdateRate( updateRate );
//    newTexture->setPosition( pos_x, pos_y );
//    newTexture->setSpriteNumber( start_num );
//    newTexture->_theRenderer = _theRenderer;
//
//    return newTexture;
//  }
}

