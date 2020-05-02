
#include "Scene.h"

#include "Exception.h"
#include "Utilities.h"
#include "Manager.h"
#include "Collision.h"

#include "logtastic.h"

#include <fstream>


namespace Regolith
{


  Scene::Scene( std::string json_file ) :
    _sceneFile( json_file ),
    _paused( false ),
    _pauseMenu( nullptr ),
    _defaultMusic( -1 ),
    _background( nullptr ),
    _sceneElements(),
    _hudElements(),
    _dialogWindows( "dialog_windows" ),
    _collisionElements(),
    _animatedElements(),
    _theCamera( nullptr ),
    _theHUD( nullptr )
  {
  }


  Scene::~Scene()
  {
    INFO_LOG( "Destroying Scene" );
    INFO_LOG( "Clearing caches" );
    _collisionElements.clear();
    _animatedElements.clear();

    INFO_LOG( "Deleting spawned objects" );
    for ( ElementList::iterator it = _hudElements.begin(); it != _hudElements.end(); ++it )
    {
      delete (*it);
    }
    _hudElements.clear();

    for ( ElementList::iterator it = _sceneElements.begin(); it != _sceneElements.end(); ++it )
    {
      delete (*it);
    }
    _sceneElements.clear();

    INFO_LOG( "Deleting dialog windows" );
    _dialogWindows.clear();

    INFO_LOG( "Deleting the background" );
    delete _background;
    _background = nullptr;

    INFO_LOG( "Deleting the cameras" );
    delete _theCamera;
    _theCamera = nullptr;

    delete _theHUD;
    _theHUD = nullptr;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // The entry to the building algorithm

  void Scene::load()
  {
    INFO_STREAM <<  "Building Scene from json file: " << _sceneFile;

    INFO_LOG( "Preparing collision data structure" );
    _collisionElements.clear();
    unsigned int numberTeams = Manager::getInstance()->getNumberTeams();
    for ( unsigned int i = 0; i < numberTeams; ++i )
    {
      _collisionElements.push_back( ElementList() );
    }



    INFO_LOG( "Loading Json Data" );
    Json::Value json_data = this->_loadConfig();

    try
    {

      Utilities::validateJson( json_data, "sounds", Utilities::JSON_TYPE_OBJECT, true );
      Utilities::validateJson( json_data, "elements", Utilities::JSON_TYPE_OBJECT, true );
      Utilities::validateJson( json_data, "cameras", Utilities::JSON_TYPE_OBJECT, true );
      Utilities::validateJson( json_data, "dialog_windows", Utilities::JSON_TYPE_ARRAY, true );
      Utilities::validateJson( json_data, "options", Utilities::JSON_TYPE_OBJECT, true );

      INFO_LOG( "Loading sounds" );
      this->_loadSounds( json_data["sounds"] );

      INFO_LOG( "Building caches" );
      this->_loadCaches( json_data["elements"] );

      INFO_LOG( "Loading cameras" );
      this->_loadCameras( json_data["cameras"] );

      INFO_LOG( "Building Dialog Windows" );
      this->_loadDialogs( json_data["dialog_windows"] );

      INFO_LOG( "Configuring Scene options" );
      this->_loadOptions( json_data["options"] );

      INFO_LOG( "Loading Scene-Specific Components" );
      this->_loadSceneSpecificComponents( json_data );

      INFO_LOG( "Registering input actions" );
      this->registerActions( inputHandler() );

    }
    catch ( Exception& ex )
    {
      FAILURE_LOG( "Scene Failed to Load" );
      throw ex;
    }
    catch ( std::exception& e )
    {
      FAILURE_STREAM << "Scene Failed to Load with unexpected exception: " << e.what();
      Exception ex( "Scene::load()", "Unexpected Exception" );
      ex.addDetail( "What", e.what() );
      throw ex;
    }
    catch ( ... )
    {
      FAILURE_LOG( "Scene Failed to Load with unknown exception" );
      Exception ex( "Scene::load()", "Unknown Exception" );
      throw ex;
    }

    INFO_LOG( "Scene Successfuly Built" );
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
      Exception ex( "Scene::_loadConfig()", "IFStream failure", false );
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


  void Scene::_loadSounds( Json::Value& json_data )
  {
    if ( Utilities::validateJson( json_data, "default_music", Utilities::JSON_TYPE_STRING, false ) )
    {
      std::string name = json_data["default_music"].asString();
      _defaultMusic = audioHandler()->getMusicID( name );
    }
  }


  void Scene::_loadCaches( Json::Value& json_data )
  {
    Utilities::validateJson( json_data, "background", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "scene_elements", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJson( json_data, "hud_elements", Utilities::JSON_TYPE_ARRAY );

    // Load the scene background
    INFO_LOG( "Building the background" );
    std::string background_resource = json_data["background"].asString();
    _background = Manager::getInstance()->spawn( background_resource );

    if ( ! _background->hasCollision() )
    {
      FAILURE_LOG( "Backgroud sprite does not have any collision. Cannot define containment within scene" );
      Exception ex( "Scene::_loadCaches()", "Background sprite does not have any associated collision." );
      throw ex;
    }

    if ( _background->hasAnimation() )
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

      this->spawn( resource_name, pos );
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
      Vector pos( x, y );

      this->spawnHUD( resource_name, pos );
    }
  }


  void Scene::_loadDialogs( Json::Value& dialog_windows )
  {
    Json::ArrayIndex dialog_windows_size = dialog_windows.size();
    for ( Json::ArrayIndex i = 0; i != dialog_windows_size; ++i )
    {
      Utilities::validateJson( dialog_windows[i], "name", Utilities::JSON_TYPE_STRING );
      Utilities::validateJson( dialog_windows[i], "dialog", Utilities::JSON_TYPE_OBJECT );

      std::string name = dialog_windows[i]["name"].asString();

      INFO_STREAM << "Building dialog window: " << name;
      Dialog* newDialog = new Dialog( _theHUD, dialog_windows[i]["dialog"] );

      _dialogWindows.addObject( newDialog, name );
    }
  }


  void Scene::_loadCameras( Json::Value& json_data )
  {
    Utilities::validateJson( json_data, "start_position", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJson( json_data, "width", Utilities::JSON_TYPE_INTEGER );
    Utilities::validateJson( json_data, "height", Utilities::JSON_TYPE_INTEGER );
    Utilities::validateJson( json_data, "type", Utilities::JSON_TYPE_STRING );

    // Configure the camera objects
    int camera_x = json_data["start_position"][0].asInt();
    int camera_y = json_data["start_position"][1].asInt();
    int camera_width = json_data["width"].asInt();
    int camera_height = json_data["height"].asInt();

    INFO_LOG( "Configuring scene camera" );
    std::string camera_type = json_data["type"].asString();
    _theCamera = new Camera( _background->getWidth(), _background->getHeight(), camera_width, camera_height );
    _theCamera->setPosition( camera_x, camera_y );
    _theCamera->registerActions( inputHandler() );

    if ( camera_type == "flying" )
    {
      INFO_LOG( "Setting mode to flying camera" );
      _theCamera->setMode( CAMERA_FLYING );
    }
    else if ( camera_type == "following" )
    {
      INFO_LOG( "Setting mode to flying camera" );
      _theCamera->setMode( CAMERA_FOLLOWING );
    }
    else if ( camera_type == "constant_velocity" )
    {
      INFO_LOG( "Setting mode to flying camera" );
      _theCamera->setMode( CAMERA_CONSTANT_VELOCITY );
    }
    else
    {
      FAILURE_STREAM << "Unknown camera type found : " << camera_type;
      Exception ex( "Scene::_loadCameras()", "Unknown Camera Type" );
      ex.addDetail( "Camera Type", camera_type );
      throw ex;
    }

    INFO_LOG( "Configuring HUD camera" );
    _theHUD = new Camera( camera_width, camera_height, camera_width, camera_height );
  }


  void Scene::_loadOptions( Json::Value& options )
  {
    if ( Utilities::validateJson( options, "on_pause", Utilities::JSON_TYPE_STRING, false ) )
    {
      std::string dialog_name = options["on_pause"].asString();
      _pauseMenu = _dialogWindows.getByName( dialog_name );
    }
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Scene update and render functions

  void Scene::update( Uint32 time )
  {
    DEBUG_STREAM << "Updating " << _animatedElements.size() << " elements";
    ElementList::iterator end = _animatedElements.end();
    for ( ElementList::iterator it = _animatedElements.begin(); it != end; ++it )
    {
      (*it)->update( time );
    }

    // Update the camera position
    _theCamera->update( time );

    // Call the derived class udpated function
    this->_update( time );


    // Now the update is finished resolve the collisions
    this->resolveCollisions();
  }


  void Scene::render()
  {
    // Draw the background first
    _background->render( _theCamera );


    // Render all the elements with respect to the background
    ElementList::iterator end = _sceneElements.end();
    for ( ElementList::iterator it = _sceneElements.begin(); it != end; ++it )
    {
      (*it)->render( _theCamera );
    }

    // Call the derived class render function
    this->_render();


    // Render all the HUD elements with respect to the window
    end = _hudElements.end();
    for ( ElementList::iterator it = _hudElements.begin(); it != end; ++it )
    {
      (*it)->render( _theHUD );
    }
  }


  void Scene::resolveCollisions()
  {
    ElementList& environmentElements = _collisionElements[DEFAULT_TEAM_ENVIRONMENT];
    ElementList::iterator environment_end = environmentElements.end();

    size_t numberEnvironment = environmentElements.size();
    size_t numberTeams = _collisionElements.size();

    DEBUG_STREAM << "Resolving Collisions No. environment: " << numberEnvironment << ", number teams: " << numberTeams;

    Contact contact; // Cache the contact info here

    for ( size_t i = DEFAULT_TEAM_PLAYER; i < numberTeams; ++i )
    {
      ElementList& team = _collisionElements[i];

      DEBUG_STREAM << "Team Size: " << team.size();

      ElementList::iterator team_end = team.end();
      for ( ElementList::iterator it = team.begin(); it != team_end; ++it )
      {
        for ( ElementList::iterator env_it = environmentElements.begin(); env_it != environment_end; ++env_it )
        {
          if ( collides( (*it), (*env_it), contact ) )
          {
            contact.applyContact();
          }
        }
      }
    }

    // Resolve the derived class specific collisions
    this->_resolveCollisions();
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Spawn functions to dynamically place elements in the scene and HUD

  void Scene::spawnHUD( std::string name, Vector vec )
  {
    Manager* man = Manager::getInstance();

    Drawable* newElement = man->spawn( name, vec );

    _hudElements.push_back( newElement );

    INFO_STREAM << "Spawning HUD resource, Team: " << newElement->getTeam();

    if ( newElement->hasAnimation() )
    {
      _animatedElements.push_back( newElement );
    }
    if ( newElement->hasInput() )
    {
      newElement->registerActions( inputHandler() );
    }
    if ( newElement->hasSound() )
    {
      newElement->registerSounds( audioHandler() );
    }
    // HUD Elements not allowed collision
  }


  void Scene::spawnHUD( unsigned int id, Vector vec )
  {
    static Manager* man = Manager::getInstance();

    Drawable* newElement = man->spawn( id, vec );

    _hudElements.push_back( newElement );

    INFO_STREAM << "Spawning HUD resource, Team: " << newElement->getTeam();

    if ( newElement->hasAnimation() )
    {
      _animatedElements.push_back( newElement );
    }
    if ( newElement->hasInput() )
    {
      newElement->registerActions( inputHandler() );
    }
    if ( newElement->hasSound() )
    {
      newElement->registerSounds( audioHandler() );
    }
    // HUD Elements not allowed collision
  }


  void Scene::spawn( std::string name, Vector vec )
  {
    Manager* man = Manager::getInstance();

    Drawable* newElement = man->spawn( name, vec );
    _sceneElements.push_back( newElement );

    INFO_STREAM << "Spawning scene resource, Team: " << newElement->getTeam() << ", at: " << vec;

    if ( newElement->hasAnimation() )
    {
      _animatedElements.push_back( newElement );
    }
    if ( newElement->hasInput() )
    {
      newElement->registerActions( inputHandler() );
    }
    if ( newElement->hasCollision() )
    {
      _collisionElements[ newElement->getTeam() ].push_back( newElement );
    }
    if ( newElement->hasSound() )
    {
      newElement->registerSounds( audioHandler() );
    }

  }


  void Scene::spawn( unsigned int id, Vector vec )
  {
    Manager* man = Manager::getInstance();

    Drawable* newElement = man->spawn( id, vec );
    _sceneElements.push_back( newElement );

    INFO_STREAM << "Spawning scene resource, Team: " << newElement->getTeam() << ", at: " << vec;

    if ( newElement->hasAnimation() )
    {
      _animatedElements.push_back( newElement );
    }
    if ( newElement->hasInput() )
    {
      newElement->registerActions( inputHandler() );
    }
    if ( newElement->hasCollision() )
    {
      _collisionElements[ newElement->getTeam() ].push_back( newElement );
    }
    if ( newElement->hasSound() )
    {
      newElement->registerSounds( audioHandler() );
    }
  }


  void Scene::addSpawned( Drawable* object )
  {
    INFO_STREAM << "Adding existing object, Team: " << object->getTeam();

    if ( object->hasAnimation() )
    {
      _animatedElements.push_back( object );
    }
    if ( object->hasInput() )
    {
      object->registerActions( inputHandler() );
    }
    if ( object->hasCollision() )
    {
      _collisionElements[ object->getTeam() ].push_back( object );
    }
    if ( object->hasSound() )
    {
      object->registerSounds( audioHandler() );
    }
  }

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Miscellaneous scene events

  void Scene::onPause()
  {
    DEBUG_LOG( "On pause called" );
    if ( _pauseMenu != nullptr )
    {
      transferFocus( _pauseMenu );
    }
  }


  void Scene::onStart()
  {
    DEBUG_LOG( "On start called" );
    if ( _defaultMusic >= 0 )
    {
      audioHandler()->setSong( 0 );
    }
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Controllable interface

  void Scene::registerEvents( InputManager* manager )
  {
    manager->registerInputRequest( this, REGOLITH_EVENT_QUIT );
    manager->registerInputRequest( this, REGOLITH_EVENT_SCENE_END );
    manager->registerInputRequest( this, REGOLITH_EVENT_SCENE_PAUSE );
    manager->registerInputRequest( this, REGOLITH_EVENT_CONTEXT_END );
    manager->registerInputRequest( this, REGOLITH_EVENT_WIN_CONDITION );
    manager->registerInputRequest( this, REGOLITH_EVENT_LOSE_CONDITION );
    manager->registerInputRequest( this, REGOLITH_EVENT_GAMEOVER );

    manager->registerInputRequest( this, REGOLITH_EVENT_CAMERA_RESIZE );
  }


  void Scene::registerActions( InputHandler* handler )
  {
    handler->registerInputRequest( this, INPUT_ACTION_QUIT );
    handler->registerInputRequest( this, INPUT_ACTION_PAUSE );
  }


  void Scene::eventAction( const RegolithEvent& regolith_event, const SDL_Event& )
  {
    static Manager* man = Manager::getInstance();
    switch ( regolith_event )
    {
      case REGOLITH_EVENT_QUIT :
      case REGOLITH_EVENT_SCENE_END :
      case REGOLITH_EVENT_CONTEXT_END :
        this->onQuit();
        break;

      case REGOLITH_EVENT_SCENE_PAUSE :
        if ( ! this->isPaused() )
          this->pause();
        break;

      case REGOLITH_EVENT_SCENE_RESUME :
        if ( this->isPaused() )
          this->resume();

      case REGOLITH_EVENT_FULLSCREEN :
        break;

      case REGOLITH_EVENT_WIN_CONDITION :
        break;

      case REGOLITH_EVENT_LOSE_CONDITION :
        break;

      case REGOLITH_EVENT_GAMEOVER :
        break;

      case REGOLITH_EVENT_CAMERA_RESIZE :
        Camera::updateScale( man->getWindowPointer()->getWidth(), man->getWindowPointer()->getHeight() );
        break;

      default:
        break;
    }
  }


  void Scene::booleanAction( const InputAction& action, bool value )
  {
    switch ( action )
    {
      case INPUT_ACTION_PAUSE :
        if ( value )
        {
          DEBUG_LOG( "Received pause signal" );
          if ( this->isPaused() )
          {
            INFO_LOG( "Resuming Scene" );
            this->resume();
          }
          else
          {
            INFO_LOG( "Pausing Scene" );
            this->pause();
          }
        }
        break;

      case INPUT_ACTION_QUIT :
        if ( value )
        {
          INFO_LOG( "Quitting Program" );
          this->pause();
          Manager::getInstance()->raiseEvent( REGOLITH_EVENT_QUIT );
        }
        break;

      default :
        break;
    }
  }

}

