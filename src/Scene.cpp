
#include "Scene.h"

#include "Exception.h"
#include "Manager.h"

#include "logtastic.h"

#include <fstream>


namespace Regolith
{
  Scene::Scene( Window* window, SDL_Renderer* renderer ) :
    _theWindow( window ),
    _theRenderer( renderer ),
    _builder( renderer ),
    _scene_elements(),
    _hud_elements(),
    _background( nullptr ),
    _theCamera(),
    _theHUD()
  {
  }


  Scene::Scene( Window* window, SDL_Renderer* renderer, std::string json_file ) :
    _theWindow( window ),
    _theRenderer( renderer ),
    _builder( renderer ),
    _scene_elements(),
    _hud_elements(),
    _background( nullptr ),
    _theCamera(),
    _theHUD()
  {
    this->buildFromJson( json_file );
  }


  void Scene::buildFromJson( std::string json_file )
  {
    Manager* man = Manager::getInstance();

    // Load the json data
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
        ERROR_LOG( "Scene::buildFromJson() : Found errors parsing json" );
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
      // Load and cache the individual texture files
      Json::Value texture_files = json_data["texture_files"];
      Json::ArrayIndex texture_files_size = texture_files.size();
      for ( Json::ArrayIndex i = 0; i != texture_files_size; ++i )
      {
        std::string name = texture_files[i]["name"].asString();
        std::string file_path = texture_files[i]["path"].asString();

        _addTextureFromFile( name, file_path );
      }


      // Load and cache the individual text textures
      Json::Value texture_strings = json_data["texture_strings"];
      Json::ArrayIndex texture_strings_size = texture_strings.size();
      for ( Json::ArrayIndex i = 0; i != texture_strings_size; ++i )
      {
        std::string name = texture_strings[i]["name"].asString();
        std::string text = texture_strings[i]["text"].asString();
        INFO_STREAM << "Loading creating text texture. Name: " << name;

        // Define the text colour
        SDL_Color color;
        if ( texture_strings[i].isMember( "colour" ) )
        {
          color.r = texture_strings[i]["colour"][0].asInt();
          color.g = texture_strings[i]["colour"][1].asInt();
          color.b = texture_strings[i]["colour"][2].asInt();
          color.a = texture_strings[i]["colour"][3].asInt();
        }
        else
        {
          color = man->getDefaultColour();
        }

        // Find the specified font
        TTF_Font* font = nullptr;
        if ( texture_strings[i].isMember( "font" ) )
        {
          font = man->getFontPointer( texture_strings[i]["font"].asString() );
          INFO_STREAM << "Creating text using font: " << texture_strings[i]["font"].asString();
        }
        else
        {
          font = man->getDefaultFont();
          INFO_LOG( "Using default font" );
        }

        // Add to cache
        _addTextureFromText( name, text, font, color );
      }
    }
    catch ( std::runtime_error& rt )
    {
      Exception ex( "Scene::buildFromJson()", "Json reading failure - building cache" );
      ex.addDetail( "File name", json_file );
      ex.addDetail( "What", rt.what() );
      throw ex;
    }


    // Setup out the Texture objects that use the SDL_Texture data
    try
    {
      // Load the scene background
      RawTexture raw_texture = _rawTextures[ json_data["background"]["texture_name"].asString() ];
      _background = _builder.build( raw_texture, json_data["background"] );

      // Load all the individual scene elements
      Json::Value scene_data = json_data["scene_elements"];
      Json::ArrayIndex scene_size = scene_data.size();
      for ( Json::ArrayIndex i = 0; i != scene_size; ++i )
      {
        RawTexture raw_texture = _rawTextures[ scene_data[i]["texture_name"].asString() ];
        _scene_elements.push_back( _builder.build( raw_texture, scene_data[i] ) );
      }

      // Load all the HUD elements
      Json::Value hud_data = json_data["hud_elements"];
      Json::ArrayIndex hud_size = hud_data.size();
      for ( Json::ArrayIndex i = 0; i != hud_size; ++i )
      {
        RawTexture raw_texture = _rawTextures[ hud_data[i]["texture_name"].asString() ];
        _hud_elements.push_back( _builder.build( raw_texture, hud_data[i] ) );
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
      ex.addDetail( "File name", json_file );
      ex.addDetail( "What", rt.what() );
      throw ex;
    }
  }


  void Scene::_addTextureFromFile( std::string name, std::string path )
  {
    // Load the image into a surface
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
    if ( loadedSurface == nullptr )
    {
      Exception ex( "Scene::addTextureFromFile()", "Could not load image data", false );
      ex.addDetail( "Image path", path );
      ex.addDetail( "SDL_img error", IMG_GetError() );
      throw ex;
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


  void Scene::_addTextureFromText( std::string name, std::string text_string, TTF_Font* font, SDL_Color color )
  {
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

    RawTexture theTexture = { loadedTexture, textSurface->w, textSurface->h };
    _rawTextures[name] = theTexture;

    SDL_FreeSurface( textSurface );
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

}

