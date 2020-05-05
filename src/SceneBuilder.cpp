
#include "SceneBuilder.h"

#include "Utilities.h"
#include "Exception.h"
#include "Scene.h"
#include "Manager.h"
#include "SceneTitle.h"
#include "ScenePlatformer.h"

#include "logtastic.h"


namespace Regolith
{

  SceneBuilder::SceneBuilder() :
    _factories()
  {
  }


  SceneBuilder::~SceneBuilder()
  {
    for ( SceneFactoryMap::iterator it = _factories.begin(); it != _factories.end(); ++it )
    {
      delete it->second;
    }
    _factories.clear();
  }


  void SceneBuilder::addFactory( SceneFactory* factory )
  {
    std::string name( factory->getSceneName() );

    if ( _factories.find( name ) != _factories.end() )
    {
      ERROR_LOG( "Attempting to add factory with a name that is already loaded" );
      return;
    }

    _factories[ name ] = factory;
  }


  Scene* SceneBuilder::build( Json::Value& json_data )
  {
    Utilities::validateJson( json_data, "type", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "file_name", Utilities::JSON_TYPE_STRING );

    std::string filename = json_data["file_name"].asString();
    std::string type = json_data["type"].asString();

    INFO_STREAM << "Building Scene of type : " << type;

    SceneFactoryMap::iterator found = _factories.find( type );

    if ( found == _factories.end() )
    {
      ERROR_STREAM << "Object type not found in factory list: " << type;
      Exception ex( "ObjectBuilder::build()", "Non-implemented build type", true );
      ex.addDetail( "TypeID", type );
      throw ex;
    }

    Json::Value scene_data;
    try
    {
      std::ifstream input( filename );
      Json::CharReaderBuilder reader_builder;
      Json::CharReader* reader = reader_builder.newCharReader();
      std::string errors;
      bool result = Json::parseFromStream( reader_builder, input, &scene_data, &errors );
      if ( ! result )
      {
        ERROR_LOG( "Found errors parsing json" );
        ERROR_STREAM << "\"" << errors << "\"";
      }
      delete reader;
    }
    catch ( std::ios_base::failure& f )
    {
      Exception ex( "Scene::_loadConfig()", "IFStream failure", false );
      ex.addDetail( "File name", filename );
      ex.addDetail( "What", f.what() );
      throw ex;
    }
    catch ( std::exception& e )
    {
      FAILURE_STREAM << "Unrecoverable error occured while building scene: " << filename;
      Exception ex( "Scene::_loadConfig()", "Json parsing failure" );
      ex.addDetail( "File name", filename );
      ex.addDetail( "What", e.what() );
      throw ex;
    }

    return found->second->build( scene_data );
  }

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Factory methods

//////////////////////////////////////////////////
  // Base class helper functions

  void SceneFactory::configureTeams( Scene* scene ) const
  {
    INFO_LOG( "Preparing collision data structure" );
    scene->_collisionElements.clear();
    unsigned int numberTeams = Manager::getInstance()->getNumberTeams();
    for ( unsigned int i = 0; i < numberTeams; ++i )
    {
      scene->_collisionElements.push_back( ElementList() );
    }
  }

  void SceneFactory::buildSounds( Scene* scene, Json::Value& json_data ) const
  {
    if ( Utilities::validateJson( json_data, "default_music", Utilities::JSON_TYPE_STRING, false ) )
    {
      std::string name = json_data["default_music"].asString();
      scene->_defaultMusic = scene->audioHandler()->getMusicID( name );
    }
  }


  void SceneFactory::buildBackground( Scene* scene, Json::Value& json_data ) const
  {
    // Load the scene background
    INFO_LOG( "Building the background" );
    std::string background_resource = json_data.asString();
    scene->_background = Manager::getInstance()->spawn( background_resource );

    if ( ! scene->_background->hasCollision() )
    {
      FAILURE_LOG( "Backgroud sprite does not have any collision. Cannot define containment within scene" );
      Exception ex( "Scene::_loadCaches()", "Background sprite does not have any associated collision." );
      throw ex;
    }

    if ( scene->_background->hasAnimation() )
    {
      scene->_animatedElements.push_back( scene->_background );
    }
  }


  void SceneFactory::buildCameras( Scene* scene, Json::Value& json_data ) const
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
    scene->_theCamera = new Camera( scene->_background->getWidth(), scene->_background->getHeight(), camera_width, camera_height );
    scene->_theCamera->setPosition( camera_x, camera_y );

    if ( camera_type == "fixed" )
    {
      INFO_LOG( "Setting mode to fixed camera" );
      scene->_theCamera->setMode( CAMERA_FIXED );
    }
    else if ( camera_type == "flying" )
    {
      INFO_LOG( "Setting mode to flying camera" );
      scene->_theCamera->setMode( CAMERA_FLYING );
    }
    else if ( camera_type == "following" )
    {
      INFO_LOG( "Setting mode to flying camera" );
      scene->_theCamera->setMode( CAMERA_FOLLOWING );
    }
    else if ( camera_type == "constant_velocity" )
    {
      INFO_LOG( "Setting mode to flying camera" );
      scene->_theCamera->setMode( CAMERA_CONSTANT_VELOCITY );
    }
    else
    {
      FAILURE_STREAM << "Unknown camera type found : " << camera_type;
      Exception ex( "Scene::_loadCameras()", "Unknown Camera Type" );
      ex.addDetail( "Camera Type", camera_type );
      throw ex;
    }

    scene->_theCamera->registerActions( scene->inputHandler() );

    INFO_LOG( "Configuring HUD camera" );
    scene->_theHUD = new Camera( camera_width, camera_height, camera_width, camera_height );
  }


  void SceneFactory::buildEnvironment( Scene* scene, Json::Value& scene_elements ) const
  {
    INFO_LOG( "Building the Environment" );
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

      scene->spawn( resource_name, pos );
    }
  }


  void SceneFactory::buildHUD( Scene* scene, Json::Value& hud_elements ) const
  {
    // Load the HUD Elements
    INFO_LOG( "Building the HUD" );
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

//      Utilities::jsonProcessPosition( hud_elements[i], dialog->_background, camera );

      scene->spawnHUD( resource_name, pos );
    }

  }


  void SceneFactory::buildDialogs( Scene* scene, Json::Value& dialog_windows ) const
  {
    DialogBuilder* builder = Manager::getInstance()->getDialogBuilder();

    Json::ArrayIndex dialog_windows_size = dialog_windows.size();
    for ( Json::ArrayIndex i = 0; i != dialog_windows_size; ++i )
    {
      Utilities::validateJson( dialog_windows[i], "name", Utilities::JSON_TYPE_STRING );
      std::string name = dialog_windows[i]["name"].asString();

      Dialog* newDialog = builder->build( dialog_windows[i], scene->_theHUD );
      scene->_dialogWindows.addObject( newDialog, name );
      INFO_STREAM << "Building dialog window: " << name << " @ " << newDialog;
    }
  }


  void SceneFactory::buildOptions( Scene*, Json::Value& ) const
  {
  }



//////////////////////////////////////////////////
  // Title scene

  Scene* TitleSceneFactory::build( Json::Value& json_data ) const
  {
    Utilities::validateJson( json_data, "input_mapping", Utilities::JSON_TYPE_STRING, true );
    INFO_LOG( "Configuring Scene InputHandler" );
    std::string input_mapping = json_data["input_mapping"].asString();

    SceneTitle* theScene = new SceneTitle( input_mapping );

    this->configureTeams( theScene );

    Utilities::validateJson( json_data, "sounds", Utilities::JSON_TYPE_OBJECT, true );
    Utilities::validateJson( json_data, "elements", Utilities::JSON_TYPE_OBJECT, true );
    Utilities::validateJson( json_data["elements"], "scene_elements", Utilities::JSON_TYPE_ARRAY, true );
    Utilities::validateJson( json_data["elements"], "hud_elements", Utilities::JSON_TYPE_ARRAY, true );
    Utilities::validateJson( json_data["elements"], "background", Utilities::JSON_TYPE_STRING, true );

    Utilities::validateJson( json_data, "cameras", Utilities::JSON_TYPE_OBJECT, true );
    Utilities::validateJson( json_data, "dialog_windows", Utilities::JSON_TYPE_ARRAY, true );

    INFO_LOG( "Loading sounds" );
    this->buildSounds( theScene, json_data["sounds"] );

    INFO_LOG( "Building Background" );
    this->buildBackground( theScene, json_data["elements"]["background"] );

    INFO_LOG( "Loading cameras" );
    this->buildCameras( theScene, json_data["cameras"] );

    INFO_LOG( "Building Environment" );
    this->buildEnvironment( theScene, json_data["elements"]["scene_elements"] );

    INFO_LOG( "Building HUD" );
    this->buildHUD( theScene, json_data["elements"]["hud_elements"] );

    INFO_LOG( "Building Dialog Windows" );
    this->buildDialogs( theScene, json_data["dialog_windows"] );

    if ( Utilities::validateJson( json_data, "options", Utilities::JSON_TYPE_OBJECT, false ) )
    {
      INFO_LOG( "Configuring Scene options" );
      this->buildOptions( theScene, json_data["options"] );

      Utilities::validateJson( json_data["options"], "starting_dialog", Utilities::JSON_TYPE_STRING, true );
      std::string dialog_name = json_data["options"]["starting_dialog"].asString();
      theScene->_startingDialog = theScene->dialogWindows().get( dialog_name );
      DEBUG_STREAM << "Starting Dialog  @ " << theScene->_startingDialog;
    }

    INFO_LOG( "Registering input actions" );
    theScene->registerActions( theScene->inputHandler() );
    theScene->registerEvents( Manager::getInstance()->getInputManager() );

    return theScene;
  }


//////////////////////////////////////////////////
  // Platformer scene

  Scene* PlatformerSceneFactory::build( Json::Value& json_data ) const
  {
    Utilities::validateJson( json_data, "input_mapping", Utilities::JSON_TYPE_STRING, true );
    INFO_LOG( "Configuring Scene InputHandler" );
    std::string input_mapping = json_data["input_mapping"].asString();

    ScenePlatformer* theScene = new ScenePlatformer( input_mapping );

    this->configureTeams( theScene );

    Utilities::validateJson( json_data, "sounds", Utilities::JSON_TYPE_OBJECT, true );
    Utilities::validateJson( json_data, "elements", Utilities::JSON_TYPE_OBJECT, true );
    Utilities::validateJson( json_data["elements"], "scene_elements", Utilities::JSON_TYPE_ARRAY, true );
    Utilities::validateJson( json_data["elements"], "hud_elements", Utilities::JSON_TYPE_ARRAY, true );
    Utilities::validateJson( json_data["elements"], "background", Utilities::JSON_TYPE_STRING, true );

    Utilities::validateJson( json_data, "cameras", Utilities::JSON_TYPE_OBJECT, true );
    Utilities::validateJson( json_data, "dialog_windows", Utilities::JSON_TYPE_ARRAY, true );
    Utilities::validateJson( json_data, "player", Utilities::JSON_TYPE_OBJECT, true );
    Utilities::validateJson( json_data, "spawn_points", Utilities::JSON_TYPE_ARRAY, true );

    INFO_LOG( "Loading sounds" );
    this->buildSounds( theScene, json_data["sounds"] );

    INFO_LOG( "Building Background" );
    this->buildBackground( theScene, json_data["elements"]["background"] );

    INFO_LOG( "Loading cameras" );
    this->buildCameras( theScene, json_data["cameras"] );

    INFO_LOG( "Building Environment" );
    this->buildEnvironment( theScene, json_data["elements"]["scene_elements"] );

    INFO_LOG( "Building HUD" );
    this->buildHUD( theScene, json_data["elements"]["hud_elements"] );

    INFO_LOG( "Building Dialog Windows" );
    this->buildDialogs( theScene, json_data["dialog_windows"] );


    INFO_LOG( "Loading player details" );
    Json::Value& player = json_data["player"];
    Utilities::validateJson( player, "position", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJson( player, "resource_name", Utilities::JSON_TYPE_STRING );

    std::string player_resource = player["resource_name"].asString();
    int player_x = player["position"][0].asInt();
    int player_y = player["position"][1].asInt();
    Vector pos( player_x, player_y );

    theScene->_player = Manager::getInstance()->getResource( player_resource );
    theScene->_player->setPosition( pos );
    theScene->_currentPlayerSpawn = pos;

    // Add object to collision caches, etc, and register actions
    theScene->addSpawned( theScene->_player );

    // Tell the current camera object to follow the player
    theScene->getCamera()->followMe( theScene->_player );


    INFO_LOG( "Loading Spawn Ponts" );
    Json::Value& spawnpoints = json_data["spawn_points"];
    Json::ArrayIndex spawnpoints_size = spawnpoints.size();
    for ( Json::ArrayIndex i = 0; i != spawnpoints_size; ++i )
    {
      Utilities::validateJsonArray( spawnpoints[i], 2, Utilities::JSON_TYPE_FLOAT );

      float x = spawnpoints[i][0].asFloat();
      float y = spawnpoints[i][1].asFloat();

      theScene->_spawnPoints.push_back( Vector( x, y ) );
    }
    INFO_STREAM << "Loaded " << spawnpoints_size << " spawn points into scene";


    if ( Utilities::validateJson( json_data, "options", Utilities::JSON_TYPE_OBJECT, false ) )
    {
      INFO_LOG( "Configuring Scene options" );
      this->buildOptions( theScene, json_data["options"] );

      std::string dialog_name = json_data["options"]["on_pause"].asString();
      theScene->_pauseMenu = theScene->dialogWindows().get( dialog_name );
    }

    INFO_LOG( "Registering input actions" );
    theScene->registerActions( theScene->inputHandler() );
    theScene->registerEvents( Manager::getInstance()->getInputManager() );

    return theScene;
  }

}

