
#include "Scene.h"

#include "Exception.h"
#include "Utilities.h"
#include "Manager.h"

#include "logtastic.h"

#include <fstream>


namespace Regolith
{


  Scene::Scene( Window* window, SDL_Renderer* renderer, ObjectBuilder* build, std::string json_file ) :
    _rawTextures(),
    _resources(),
    _resourceNames(),
    _teamNames(),
    _theWindow( window ),
    _theRenderer( renderer ),
    _theBuilder( build ),
    _sceneFile( json_file ),
    _background( nullptr ),
    _sceneElements(),
    _hudElements(),
    _collisionElements(),
    _animatedElements(),
    _inputElements(),
    _theCamera(),
    _theHUD()
  {
  }


  void Scene::load()
  {
    this->buildFromJson();
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // All the loading functions to import Json data and build the necessary objects

  Json::Value Scene::_loadConfig()
  {
    try
    {
      Json::Value json_data;

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

      return json_data;
    }
    catch ( std::ios_base::failure& f )
    {
      Exception ex( "Scene::_loadConfig()", "IFSteam failure", false );
      ex.addDetail( "File name", _sceneFile );
      ex.addDetail( "What", f.what() );
      throw ex;
    }
    catch ( std::runtime_error& rt )
    {
      Exception ex( "Scene::_loadConfig()", "Json parsing failure" );
      ex.addDetail( "File name", _sceneFile );
      ex.addDetail( "What", rt.what() );
      throw ex;
    }
  }


  void Scene::_loadTextures( Json::Value& json_data )
  {
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
      Exception ex( "Scene::_loadTextures()", "Json reading failure - building cache" );
      ex.addDetail( "File name", _sceneFile );
      ex.addDetail( "What", rt.what() );
      throw ex;
    }
  }


  void Scene::_loadResources( Json::Value& resources )
  {
    INFO_LOG( "Configuring default teams: \"environment\" & \"player_01\"" );
    // Create the default teams - environment & player_01
    _teamNames[ "environment" ] = 0u;
    _collisionElements.push_back( ElementList() );
    _teamNames[ "player_01" ] = 1u;
    _collisionElements.push_back( ElementList() );

    // Setup out the resources that use the SDL_Texture data
    try
    {
      // Load all the individual resource objects
      INFO_LOG( "Loading resource objects" );
      Json::ArrayIndex resources_size = resources.size();
      for ( Json::ArrayIndex i = 0; i != resources_size; ++i )
      {
        try
        {
          Utilities::validateJson( resources[i], "resource_name", Utilities::JSON_TYPE_STRING );

          std::string resource_name = resources[i]["resource_name"].asString();
          DEBUG_STREAM << "  Building resource: " << resource_name;
          Drawable* newResource =  _theBuilder->build( resources[i] );

          _resources.push_back( newResource );
          _resourceNames[ resource_name ] = _resources.size() - 1; // Save the ID number

          // Handle the team names
          std::string team_name;
          // If one is provided
          if ( resources[i].isMember( "team_name" ) )
            team_name = resources[i]["team_name"].asString();
          else // Else assume environment
            team_name = "environment";

          DEBUG_STREAM << "  Resource Team Name : " << team_name;
          // See if its already been used.
          if ( _teamNames.find( team_name ) == _teamNames.end() )
          {
            // Create it otherwise
            _collisionElements.push_back( ElementList() );
            _teamNames[ team_name ] = _collisionElements.size() - 1;
          }

          // Set the resource team ID
          DEBUG_LOG( "Setting team name" );
          int team_id = _teamNames[ team_name ];
          newResource->setTeam( team_id );
        }
        catch ( Exception& ex )
        {
          if ( ex.isRecoverable() )
          {
            ERROR_STREAM << "An error occured building scene elements: " << ex.what();
            ERROR_LOG( "Skipping element" );
          }
          else
          {
            FAILURE_STREAM << "An error occured building scene elements: " << ex.what();
            FAILURE_LOG( "Error is non-recoverable" );
            throw ex;
          }
        }
      }
    }
    catch ( std::runtime_error& rt )
    {
      Exception ex( "Scene::_loadResources()", "Json reading failure" );
      ex.addDetail( "File name", _sceneFile );
      ex.addDetail( "What", rt.what() );
      throw ex;
    }
  }


  void Scene::_loadCameras( Json::Value& json_data )
  {
    Utilities::validateJson( json_data, "start_position", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJson( json_data, "width", Utilities::JSON_TYPE_INTEGER );
    Utilities::validateJson( json_data, "height", Utilities::JSON_TYPE_INTEGER );

    try
    {
      // Configure the camera objects
      int camera_x = json_data["start_position"][0].asInt();
      int camera_y = json_data["start_position"][1].asInt();
      int camera_width = json_data["width"].asInt();
      int camera_height = json_data["height"].asInt();

      INFO_LOG( "Configuring scene camera" );
      _theCamera.configure( _background->getWidth(), _background->getHeight(), camera_width, camera_height );
      _theCamera.setPosition( camera_x, camera_y );

      INFO_LOG( "Configuring HUD camera" );
      _theHUD.configure( camera_width, camera_height, camera_width, camera_height );
    }
    catch ( std::runtime_error& rt )
    {
      Exception ex( "Scene::_loadCameras()", "Json reading failure" );
      ex.addDetail( "File name", _sceneFile );
      ex.addDetail( "What", rt.what() );
      throw ex;
    }
  }


  void Scene::_loadCaches( Json::Value& json_data )
  {
    Utilities::validateJson( json_data, "background", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "scene_elements", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJson( json_data, "hud_elements", Utilities::JSON_TYPE_ARRAY );
    try
    {
      // Load the scene background
      INFO_LOG( "Building the background" );
      std::string background_resource = json_data["background"].asString();
      unsigned int id_number = _resourceNames[ background_resource ];
      _background = _resources[ id_number ];

      if ( _background->getProperties() & OBJECT_ANIMATED )
      {
        _animatedElements.push_back( _background );
      }


      // Load the Scene Elements
      INFO_LOG( "Building the scene" );
      Json::Value scene_elements = json_data["scene_elements"];
      Json::ArrayIndex scene_elements_size = scene_elements.size();
      for ( Json::ArrayIndex i = 0; i != scene_elements_size; ++i )
      {
        Utilities::validateJson( scene_elements[i], "resource_name", Utilities::JSON_TYPE_STRING );
        Utilities::validateJson( scene_elements[i], "position", Utilities::JSON_TYPE_ARRAY );

        // Load the necessary info
        std::string resource_name = scene_elements[i]["resource_name"].asString();
        int x = scene_elements[i]["position"][0].asInt();
        int y = scene_elements[i]["position"][1].asInt();
        Vector pos( x, y );

        // Check that the resource exists
        if ( _resourceNames.find( resource_name ) == _resourceNames.end() )
        {
          ERROR_STREAM << "Could not locate a resource with the specified name; " << resource_name;
          ERROR_LOG( "Ignoring current element and continuing." );
          continue;
        }

        // Determine the ID number and spawn the element
        unsigned int id_number = _resourceNames[ resource_name ];
        this->spawnAt( id_number, pos );
      }

      // Load the HUD Elements
      INFO_LOG( "Building the HUD" );
      Json::Value hud_elements = json_data["hud_elements"];
      Json::ArrayIndex hud_elements_size = hud_elements.size();
      for ( Json::ArrayIndex i = 0; i != hud_elements_size; ++i )
      {
        Utilities::validateJson( hud_elements[i], "resource_name", Utilities::JSON_TYPE_STRING );
        Utilities::validateJson( hud_elements[i], "position", Utilities::JSON_TYPE_ARRAY );

        // Load the necessary info
        std::string resource_name = hud_elements[i]["resource_name"].asString();
        float x = hud_elements[i]["position"][0].asFloat();
        float y = hud_elements[i]["position"][1].asFloat();

        // Check that the resource exists
        if ( _resourceNames.find( resource_name ) == _resourceNames.end() )
        {
          ERROR_STREAM << "Could not locate a resource with the specified name; " << resource_name;
          ERROR_LOG( "Ignoring current element and continuing." );
          continue;
        }

        // Determine the ID number and spawn the element
        unsigned int id_number = _resourceNames[ resource_name ];
        this->spawnHUD( id_number, Vector( x, y ) );
      }
    }
    catch ( Exception& ex )
    {
      if ( ex.isRecoverable() )
      {
        ERROR_STREAM << "An error occured building scene caches: " << ex.what();
      }
      else
      {
        FAILURE_STREAM << "An error occured building scene caches: " << ex.what();
        FAILURE_LOG( "Error is non-recoverable" );
        throw ex;
      }
    }
    catch ( std::runtime_error& rt )
    {
      Exception ex( "Scene::_loadCaches()", "Json reading failure" );
      ex.addDetail( "File name", _sceneFile );
      ex.addDetail( "What", rt.what() );
      throw ex;
    }
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // The entry to the building algorithm

  void Scene::buildFromJson()
  {
    // Tell the builder who's using it
    INFO_STREAM <<  "Building Scene from json file: " << _sceneFile;
    _theBuilder->setScene( this );

    INFO_LOG( "Builder Configured" );
    INFO_LOG( "Loading configuration data" );

    Json::Value json_data = this->_loadConfig();

    if ( ! json_data.isMember( "textures" ) )
    {
      FAILURE_LOG( "Could not find texture list in json data file" );
      Exception ex( "Scene::_buildFromJson()", "No texture member found in Json Data", false);
      ex.addDetail( "File name", _sceneFile );
      throw ex;
    }
    INFO_LOG( "Loading textures" );
    this->_loadTextures( json_data["textures"] );

    if ( ! json_data.isMember( "resources" ) )
    {
      FAILURE_LOG( "Could not find resource list in json data file" );
      Exception ex( "Scene::_buildFromJson()", "No resource member found in Json Data", false);
      ex.addDetail( "File name", _sceneFile );
      throw ex;
    }
    INFO_LOG( "Loading resources" );
    this->_loadResources( json_data["resources"] );

    if ( ! json_data.isMember( "elements" ) )
    {
      FAILURE_LOG( "Could not find elements data in json data file" );
      Exception ex( "Scene::_buildFromJson()", "No elements member found in Json Data", false);
      ex.addDetail( "File name", _sceneFile );
      throw ex;
    }
    INFO_LOG( "Building caches" );
    this->_loadCaches( json_data["elements"] );

    if ( ! json_data.isMember( "cameras" ) )
    {
      FAILURE_LOG( "Could not find cameras data in json data file" );
      Exception ex( "Scene::_buildFromJson()", "No cameras member found in Json Data", false);
      ex.addDetail( "File name", _sceneFile );
      throw ex;
    }
    INFO_LOG( "Loading cameras" );
    this->_loadCameras( json_data["cameras"] );
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Texture Creation helper functions

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

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Scene update and render functions

  void Scene::update( Uint32 time )
  {
    ElementList::iterator end = _animatedElements.end();
    for ( ElementList::iterator it = _animatedElements.begin(); it != end; ++it )
    {
      (*it)->update( time );
    }
  }


  void Scene::render()
  {
    // Draw the background first
    _background->render( &_theCamera );


    // Render all the elements with respect to the background
    ElementList::iterator end = _sceneElements.end();
    for ( ElementList::iterator it = _sceneElements.begin(); it != end; ++it )
    {
      (*it)->render( &_theCamera );
    }


    // Render all the elements with respect to the window
    end = _hudElements.end();
    for ( ElementList::iterator it = _hudElements.begin(); it != end; ++it )
    {
      (*it)->render( &_theHUD );
    }
  }


  void Scene::handleEvent( SDL_Event& e )
  {
    ElementList::iterator end = _inputElements.end();
    for ( ElementList::iterator it = _inputElements.begin(); it != end; ++it )
    {
      (*it)->handleEvent( e );
    }
  }


  void Scene::resolveCollisions()
  {
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Return existing texture pointer

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


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Spawn functions to dynamically place elements in the scene and HUD

  void Scene::spawnHUD( unsigned int id, Vector vec )
  {
    Drawable* newElement = _resources[id]->clone();
    newElement->setPosition( vec );
    _hudElements.push_back( newElement );

    INFO_STREAM << "Spawning HUD resource, properties: " << newElement->getProperties();

    if ( newElement->getProperties() & OBJECT_ANIMATED )
    {
      _animatedElements.push_back( newElement );
    }
    if ( newElement->getProperties() & OBJECT_HAS_INPUT )
    {
      _inputElements.push_back( newElement );
    }
    // HUD Elements not allowed collision
  }


  void Scene::spawn( unsigned int id )
  {
    Drawable* newElement = _resources[id]->clone();
    _sceneElements.push_back( newElement );

    INFO_STREAM << "Spawning resource, properties: " << newElement->getProperties();

    if ( newElement->getProperties() & OBJECT_ANIMATED )
    {
      _animatedElements.push_back( newElement );
    }
    if ( newElement->getProperties() & OBJECT_HAS_INPUT )
    {
      _inputElements.push_back( newElement );
    }
    if ( newElement->getProperties() & OBJECT_HAS_COLLISION )
    {
      _collisionElements[ newElement->getTeam() ].push_back( newElement );
    }

  }


  void Scene::spawnAt( unsigned int id, Vector vec )
  {
    Drawable* newElement = _resources[id]->cloneAt( vec );
    _sceneElements.push_back( newElement );

    INFO_STREAM << "Spawning resource at " << vec << ", properties: " << newElement->getProperties();
    if ( newElement->getProperties() & OBJECT_ANIMATED )
    {
      _animatedElements.push_back( newElement );
    }
    if ( newElement->getProperties() & OBJECT_HAS_INPUT )
    {
      _inputElements.push_back( newElement );
    }
    if ( newElement->getProperties() & OBJECT_HAS_COLLISION )
    {
      _collisionElements[ newElement->getTeam() ].push_back( newElement );
    }

  }

}

