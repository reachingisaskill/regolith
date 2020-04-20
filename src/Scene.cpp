
#include "Scene.h"

#include "Exception.h"
#include "Manager.h"

#include "logtastic.h"

#include <fstream>


namespace Regolith
{
//  Scene::Scene( Window* window, SDL_Renderer* renderer ) :
//    _theWindow( window ),
//    _theRenderer( renderer ),
//    _scene_elements(),
//    _hud_elements(),
//    _background( nullptr ),
//    _theCamera(),
//    _theHUD()
//  {
//  }


  Scene::Scene( Window* window, SDL_Renderer* renderer, TextureBuilder* build, std::string json_file ) :
    _theWindow( window ),
    _theRenderer( renderer ),
    _theBuilder( build ),
    _sceneFile( json_file ),
    _scene_elements(),
    _hud_elements(),
    _background( nullptr ),
    _theCamera(),
    _theHUD()
  {
  }


  void Scene::load()
  {
    this->buildFromJson();
  }


  void Scene::buildFromJson()
  {
    // Tell the builder who's using it
    _theBuilder->setScene( this );

    // Load the json data
    Json::Value json_data;
    try
    {
      std::ifstream input( _sceneFile );
      Json::CharReaderBuilder reader_builder;
      Json::CharReader* reader = reader_builder.newCharReader();
      std::string errors;
      bool result = Json::parseFromStream( reader_builder, input, &json_data, &errors );
      if ( ! result )
      {
        ERROR_LOG( "Found errors parsing json" );
        ERROR_STREAM << "\"" << errors << "\"";
      }
      delete reader;
    }
    catch ( std::ios_base::failure& f )
    {
      Exception ex( "Scene::buildFromJson()", "IFSteam failure", false );
      ex.addDetail( "File name", _sceneFile );
      ex.addDetail( "What", f.what() );
      throw ex;
    }
    catch ( std::runtime_error& rt )
    {
      Exception ex( "Scene::buildFromJson()", "Json parsing failure" );
      ex.addDetail( "File name", _sceneFile );
      ex.addDetail( "What", rt.what() );
      throw ex;
    }

    try
    {
      // Load and cache the individual texture files
      Json::Value texture_files = json_data["texture_files"];
      Json::ArrayIndex texture_files_size = texture_files.size();
      for ( Json::ArrayIndex i = 0; i != texture_files_size; ++i )
      {
        _addTextureFromFile( texture_files[i] );
      }

      // Load and cache the individual text textures
      Json::Value texture_strings = json_data["texture_strings"];
      Json::ArrayIndex texture_strings_size = texture_strings.size();
      for ( Json::ArrayIndex i = 0; i != texture_strings_size; ++i )
      {
        _addTextureFromText( texture_strings[i] );
      }
    }
    catch ( std::runtime_error& rt )
    {
      Exception ex( "Scene::buildFromJson()", "Json reading failure - building cache" );
      ex.addDetail( "File name", _sceneFile );
      ex.addDetail( "What", rt.what() );
      throw ex;
    }


    // Setup out the Texture objects that use the SDL_Texture data
    try
    {
      // Load the scene background
      _background = _theBuilder->build( json_data["background"] );
      if ( _background->getProperties() & OBJECT_ANIMATED )
      {
        _animated_elements.push_back( _background );
      }

      // Load all the individual scene elements
      Json::Value scene_data = json_data["scene_elements"];
      Json::ArrayIndex scene_size = scene_data.size();
      for ( Json::ArrayIndex i = 0; i != scene_size; ++i )
      {
        Drawable* newTexture =  _theBuilder->build( scene_data[i] );
        _scene_elements.push_back( newTexture );
        INFO_STREAM << "Object properties: " << newTexture->getProperties();
        if ( newTexture->getProperties() & OBJECT_ANIMATED )
        {
          _animated_elements.push_back( newTexture );
        }
      }

      // Load all the HUD elements
      Json::Value hud_data = json_data["hud_elements"];
      Json::ArrayIndex hud_size = hud_data.size();
      for ( Json::ArrayIndex i = 0; i != hud_size; ++i )
      {
        Drawable* newTexture = _theBuilder->build( hud_data[i] );
        _hud_elements.push_back( newTexture );
        INFO_STREAM << "Object properties: " << newTexture->getProperties();
        if ( newTexture->getProperties() & OBJECT_ANIMATED )
        {
          _animated_elements.push_back( newTexture );
        }
      }


      // Configure the camera objects
      int camera_x = json_data["camera"]["position"][0].asInt();
      int camera_y = json_data["camera"]["position"][1].asInt();
      int camera_width = json_data["camera"]["width"].asInt();
      int camera_height = json_data["camera"]["height"].asInt();

      INFO_LOG( "Configuring scene camera" );
      _theCamera.configure( _background->getWidth(), _background->getHeight(), camera_width, camera_height );
      _theCamera.setPosition( camera_x, camera_y );
      INFO_LOG( "Configuring HUD camera" );
      _theHUD.configure( camera_width, camera_height, camera_width, camera_height );
    }
    catch ( std::runtime_error& rt )
    {
      Exception ex( "Scene::buildFromJson()", "Json reading failure" );
      ex.addDetail( "File name", _sceneFile );
      ex.addDetail( "What", rt.what() );
      throw ex;
    }
  }


  void Scene::_addTextureFromFile( Json::Value& json_data )
  {

    std::string name = json_data["name"].asString();
    std::string path = json_data["path"].asString();

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
    SDL_Texture* loadedTexture = SDL_CreateTextureFromSurface( _theRenderer, loadedSurface );
    if ( loadedTexture == nullptr )
    {
      SDL_FreeSurface( loadedSurface );
      Exception ex( "Scene::addTextureFromFile()", "Could not convert to texture", false );
      ex.addDetail( "Image path", path );
      ex.addDetail( "SDL error", SDL_GetError() );
      throw ex;
    }

    RawTexture theTexture = { loadedTexture, loadedSurface->w, loadedSurface->h };
    _rawTextures[name] = theTexture;

    SDL_FreeSurface( loadedSurface );
  }


  void Scene::_addTextureFromText( Json::Value& json_data )
  {
    Manager* man = Manager::getInstance();

    std::string name = json_data["name"].asString();
    std::string text_string = json_data["text"].asString();
    INFO_STREAM << "Creating and loaded text texture. Name: " << name;

    // Define the text colour
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

    // Find the specified font
    TTF_Font* font = nullptr;
    if ( json_data.isMember( "font" ) )
    {
      font = man->getFontPointer( json_data["font"].asString() );
      INFO_STREAM << "Creating text using font: " << json_data["font"].asString();
    }
    else
    {
      font = man->getDefaultFont();
      INFO_LOG( "Using default font" );
    }

    // Render the text as TTF
    SDL_Surface* textSurface = TTF_RenderText_Solid( font, text_string.c_str(), color );
    if ( textSurface == nullptr )
    {
      Exception ex( "Scene::_addTextureFromText()", "Could not render text", true );
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
      Exception ex( "Scene::_addTextureFromText()", "Could not convert to texture", true );
      ex.addDetail( "Text string", text_string );
      ex.addDetail( "SDL_ttf error", TTF_GetError() );
      throw ex;
    }

    RawTexture theTexture = { loadedTexture, textSurface->w, textSurface->h };
    _rawTextures[name] = theTexture;

    SDL_FreeSurface( textSurface );
  }


  void Scene::update( Uint32 time )
  {
    TextureList::iterator end = _animated_elements.end();
    for ( TextureList::iterator it = _animated_elements.begin(); it != end; ++it )
    {
      (*it)->update( time );
    }
  }


  void Scene::render()
  {
    // Draw the background first
    _background->render( &_theCamera );


    // Render all the elements with respect to the background
    TextureList::iterator end = _scene_elements.end();
    for ( TextureList::iterator it = _scene_elements.begin(); it != end; ++it )
    {
      (*it)->render( &_theCamera );
    }


    // Render all the elements with respect to the window
    end = _hud_elements.end();
    for ( TextureList::iterator it = _hud_elements.begin(); it != end; ++it )
    {
      (*it)->render( &_theHUD );
    }
  }


  void Scene::handleEvent( SDL_Event* e )
  {
  }


  RawTexture Scene::findRawTexture( std::string name ) const
  {
    RawTextureMap::const_iterator found = _rawTextures.find( name );
    if ( found == _rawTextures.end() )
    {
      ERROR_STREAM << "Failed to find raw texture with name: " << name;
      Exception ex( "Scene::findRawTexture()", "Could not find raw texture", true );
      ex.addDetail( "Texture name", name );
      throw ex;
    }

    return found->second;
  }

}

