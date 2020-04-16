
#include "Scene.h"

#include "Exception.h"
#include "Manager.h"

#include "logtastic.h"

#include <fstream>


namespace Regolith
{
  Scene::Scene( Window* window, SDL_Renderer* renderer ) :
    _theWindow( window ),
    _builder( renderer ),
    _scene_elements(),
    _hud_elements(),
    _background( nullptr )
  {
  }


  Scene::Scene( Window* window, SDL_Renderer* renderer, std::string json_file ) :
    _theWindow( window ),
    _builder( renderer ),
    _scene_elements(),
    _hud_elements(),
    _background( nullptr )
  {
    this->buildFromJson( json_file );
  }


  void Scene::buildFromJson( std::string json_file )
  {
    Json::Value json_data;
    try
    {
      std::ifstream input( json_file );
      Json::CharReaderBuilder reader_builder;
      Json::CharReader* reader = reader_builder.newCharReader();
      std::string errors;
      int result = Json::parseFromStream( reader_builder, input, &json_data, &errors );
      if ( result )
      {
        ERROR_LOG( "Founnd errors parsing json" );
        ERROR_STREAM << errors;
      }
      delete reader;
    }
    catch ( std::ios_base::failure& f )
    {
      Exception ex( "Scene::buildFromJson()", "IFSteam failure", false );
      ex.addDetail( "File name", json_file );
      ex.addDetail( "What", f.what() );
      throw ex;
    }
    catch ( std::runtime_error& rt )
    {
      Exception ex( "Scene::buildFromJson()", "Json parsing failure" );
      ex.addDetail( "File name", json_file );
      ex.addDetail( "What", rt.what() );
      throw ex;
    }


    try
    {
      // Load the scene background
      _background = _builder.build( json_data["background"] );

      // Load all the individual scene elements
      Json::Value scene_data = json_data["scene_elements"];
      Json::ArrayIndex scene_size = scene_data.size();
      for ( Json::ArrayIndex i = 0; i != scene_size; ++i )
      {
        _scene_elements.push_back( _builder.build( scene_data[i] ) );
      }

      // Load all the HUD elements
      Json::Value hud_data = json_data["hud_elements"];
      Json::ArrayIndex hud_size = hud_data.size();
      for ( Json::ArrayIndex i = 0; i != hud_size; ++i )
      {
        _hud_elements.push_back( _builder.build( hud_data[i] ) );
      }

    }
    catch ( std::runtime_error& rt )
    {
      Exception ex( "Scene::buildFromJson()", "Json reading failure" );
      ex.addDetail( "File name", json_file );
      ex.addDetail( "What", rt.what() );
      throw ex;
    }

  }

}

