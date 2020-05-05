
#include "Scene.h"

#include "Exception.h"
#include "Utilities.h"
#include "Manager.h"
#include "Collision.h"

#include "logtastic.h"

#include <fstream>


namespace Regolith
{


  Scene::Scene( std::string inputHandler ) :
    Context( inputHandler ),
    _isLoaded( false ),
    _paused( false ),
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
  // Scene update and render functions

  void Scene::update( Uint32 time )
  {
    DEBUG_STREAM << "Updating " << _animatedElements.size() << " elements";
    ElementList::iterator end = _animatedElements.end();
    for ( ElementList::iterator it = _animatedElements.begin(); it != end; ++it )
    {
      (*it)->update( time );
    }

    // Update the scene camera position
    _theCamera->update( time );
    // The HUD camera doesn't change! (At least not in this version...)

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


  void Scene::onStart()
  {
    DEBUG_LOG( "On start called" );
    if ( _defaultMusic >= 0 )
    {
      audioHandler()->setSong( _defaultMusic );
    }
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Controllable interface

  void Scene::registerEvents( InputManager* manager )
  {
    manager->registerEventRequest( this, REGOLITH_EVENT_QUIT );
    manager->registerEventRequest( this, REGOLITH_EVENT_SCENE_END );
    manager->registerEventRequest( this, REGOLITH_EVENT_SCENE_PAUSE );
    manager->registerEventRequest( this, REGOLITH_EVENT_CONTEXT_END );
    manager->registerEventRequest( this, REGOLITH_EVENT_WIN_CONDITION );
    manager->registerEventRequest( this, REGOLITH_EVENT_LOSE_CONDITION );
    manager->registerEventRequest( this, REGOLITH_EVENT_GAMEOVER );

    manager->registerEventRequest( this, REGOLITH_EVENT_CAMERA_RESIZE );
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
        this->onStop();
        break;

      case REGOLITH_EVENT_SCENE_PAUSE :
        if ( ! this->isPaused() )
          this->pause();
        break;

      case REGOLITH_EVENT_SCENE_RESUME :
        if ( this->isPaused() )
          this->resume();
        break;

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

