
#include "Regolith/Contexts/Context.h"

#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/Links/LinkContextManager.h"
#include "Regolith/ObjectInterfaces/DrawableObject.h"
#include "Regolith/ObjectInterfaces/NoisyObject.h"
#include "Regolith/ObjectInterfaces/CollidableObject.h"
#include "Regolith/ObjectInterfaces/ButtonObject.h"
#include "Regolith/ObjectInterfaces/AnimatedObject.h"
#include "Regolith/ObjectInterfaces/ControllableObject.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/GamePlay/Camera.h"


namespace Regolith
{

  // Local function declarations

  void configureObject( ContextLayer&, PhysicalObject*, Json::Value& );

////////////////////////////////////////////////////////////////////////////////////////////////////

  Context::Context() :
    MassProduceable(),
    ControllableInterface(),
    _owner( nullptr ),
    _theInput(),
    _theFocus(),
    _theCollision(),
    _closed( false ),
    _paused( false ),
    _pauseable( false ),
    _layers()
  {
  }


  Context::~Context()
  {
    INFO_LOG( "Context::~Context : Destroying Context" );
  }

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Accessors and modifiers

  ContextLayer& Context::getLayer( std::string name )
  {
    ContextLayerList::iterator it = _layers.begin();

    while ( it->getName() != name ) ++it;

    if ( it == _layers.end() )
    {
      Exception ex( "Context::getLayer()", "Requested context layer not found" );
      ex.addDetail( "Name", name );
      throw ex;
    }

    return *it;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Context stack call back functions

  void Context::startContext()
  {
    this->onStart();
  }


  void Context::stopContext()
  {
    this->onStop();
  }


  void Context::pauseContext()
  {
    if ( (! _paused) && _pauseable )
    {
      _paused = true;
      _owner->getAudioHandler().pause();
      this->onPause();
    }
  }


  void Context::resumeContext()
  {
    if ( _paused )
    {
      _paused = false;
      _owner->getAudioHandler().play();
      this->onResume();
    }
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Frame update functions

  void Context::update( float time )
  {
    DEBUG_LOG( "Context::update : Context Update" );

    // Update all the animated objects
    ContextLayerList::iterator layer_end = _layers.end();
    for ( ContextLayerList::iterator layer_it = _layers.begin(); layer_it != layer_end; ++layer_it )
    {
      for ( LayerGraph::iterator team_it = layer_it->layerGraph.begin(); team_it != layer_it->layerGraph.end(); ++team_it )
      {
        DEBUG_STREAM << "Context::update : Updating " << team_it->second.size() << " objects.";
        for ( PhysicalObjectList::iterator obj_it = team_it->second.begin(); obj_it != team_it->second.end(); /*++obj_it*/ )
        {
          // If object is marked for destruction, remove it from the scene graph
          if ( (*obj_it)->isDestroyed() )
          {
            DEBUG_LOG( "Context::update : Removing object from layer." );
            obj_it = team_it->second.erase( obj_it );
            continue;
          }
          else
          {

            // If object can be moved, do the physics integration
            if ( (*obj_it)->hasMovement() )
            {
              (*obj_it)->step( time );
            }

            // If the object is animated, update the animation
            if ( (*obj_it)->hasAnimation() )
            {
              dynamic_cast<AnimatedObject*>(*obj_it)->update( time );
            }

            if ( (*obj_it)->hasPhysics() )
            {
              this->updatePhysics( (*obj_it), time );
            }

            // Update the iterator.
            ++obj_it;
          }
        }
      }
    }

    // Update the context state
    updateContext( time );

    // Update the camera position
    _cameraPosition = updateCamera( time );


    DEBUG_STREAM << "Context::update : Starting Team Collision";
    CollisionHandler::SetIterator team_end = _theCollision.teamCollisionEnd();

    for ( ContextLayerList::iterator layer_it = _layers.begin(); layer_it != layer_end; ++layer_it )
    {
      for ( CollisionHandler::SetIterator rule_it = _theCollision.teamCollisionBegin(); rule_it != team_end; ++rule_it )
      {
        PhysicalObjectList& team = layer_it->layerGraph[ *rule_it ];
        if ( team.size() < 2 ) continue;

        PhysicalObjectList::iterator end = team.end();
        PhysicalObjectList::iterator it1 = team.begin();
        PhysicalObjectList::iterator it2 = team.begin();

        while ( it1 != end )
        {
          it2 = it1;
          ++it2;
          while ( it2 != end )
          {
            _theCollision.collides( dynamic_cast<CollidableObject*>((*it1)), dynamic_cast<CollidableObject*>((*it2)) );
            ++it2;
          }
          ++it1;
        }
      }
    }


    DEBUG_STREAM << "Context::update : Starting Layer Collision";
    CollisionHandler::PairIterator collides_end = _theCollision.collisionEnd();

    for ( ContextLayerList::iterator layer_it = _layers.begin(); layer_it != layer_end; ++layer_it )
    {
      for ( CollisionHandler::PairIterator rule_it = _theCollision.collisionBegin(); rule_it != collides_end; ++rule_it )
      {
        PhysicalObjectList& team1 = layer_it->layerGraph[ rule_it->first ];
        if ( team1.size() == 0 ) continue;
        PhysicalObjectList& team2 = layer_it->layerGraph[ rule_it->second ];
        if ( team2.size() == 0 ) continue;

        PhysicalObjectList::iterator end1 = team1.end();
        PhysicalObjectList::iterator end2 = team2.end();

        for ( PhysicalObjectList::iterator it1 = team1.begin(); it1 != end1; ++it1 )
        {
          for ( PhysicalObjectList::iterator it2 = team2.begin(); it2 != end2; ++it2 )
          {
            _theCollision.collides( dynamic_cast<CollidableObject*>((*it1)), dynamic_cast<CollidableObject*>((*it2)) );
          }
        }
      }
    }


    DEBUG_STREAM << "Context::update : Starting Layer Containment";
    collides_end = _theCollision.containerEnd();

    for ( ContextLayerList::iterator layer_it = _layers.begin(); layer_it != layer_end; ++layer_it )
    {
      for ( CollisionHandler::PairIterator rule_it = _theCollision.containerBegin(); rule_it != collides_end; ++rule_it )
      {
        PhysicalObjectList& team1 = layer_it->layerGraph[ rule_it->first ];
        if ( team1.size() == 0 ) continue;
        PhysicalObjectList& team2 = layer_it->layerGraph[ rule_it->second ];
        if ( team2.size() == 0 ) continue;

        PhysicalObjectList::iterator end1 = team1.end();
        PhysicalObjectList::iterator end2 = team2.end();

        for ( PhysicalObjectList::iterator it1 = team1.begin(); it1 != end1; ++it1 )
        {
          for ( PhysicalObjectList::iterator it2 = team2.begin(); it2 != end2; ++it2 )
          {
            _theCollision.contains( dynamic_cast<CollidableObject*>( *it1 ), dynamic_cast<CollidableObject*>( *it2 ) );
          }
        }
      }
    }
  }


  void Context::render( Camera& camera )
  {
    DEBUG_LOG( "Context::render : Context Render" );
    ContextLayerList::iterator layer_end = _layers.end();
    for ( ContextLayerList::iterator layer_it = _layers.begin(); layer_it != layer_end; ++layer_it )
    {
      const Vector& layer_position = layer_it->getPosition();
      const Vector& movement_scale = layer_it->getMovementScale();

      // % - Directional dot-product
      Vector camera_position = ( _cameraPosition - layer_position ) % movement_scale;

      for ( LayerGraph::iterator team_it = layer_it->layerGraph.begin(); team_it != layer_it->layerGraph.end(); ++team_it )
      {
        DEBUG_STREAM << "Context::render : Rendering team : " << team_it->first;
        for ( PhysicalObjectList::iterator it = team_it->second.begin(); it != team_it->second.end(); ++it )
        {
          // If the object can be drawn, render it to the back buffer
          if ( (*it)->hasTexture() )
          {
            camera.renderDrawableObject( dynamic_cast<DrawableObject*>(*it), camera_position );
          }
        }
      }
    }

    // Call inherited function to do any context-specific rendering. (e.g. transition effects)
    renderContext( camera );
  }

//////////////////////////////////////////////////////////////////////////////////////////////////// 
  // Context configuration

  void Context::configure( Json::Value& json_data, ContextGroup& handler )
  {
    INFO_LOG( "Context::configure : Configuring Context" );
    _owner = &handler;

    DEBUG_STREAM << "Context::configure : Owner @ " << _owner;

    validateJson( json_data, "layers", JsonType::OBJECT );
    validateJson( json_data, "collision_handling", JsonType::OBJECT );

    // Configure the collision rules
    _theCollision.configure( json_data["collision_handling"] );

    // Configure the input mapping if one is required
    if ( validateJson( json_data, "input_mapping", JsonType::STRING, false ) )
    {
      _theInput.configure( json_data["input_mapping"].asString() );
    }


    // Pause behaviour
    if ( validateJson( json_data, "pauseable", JsonType::BOOLEAN, false ) )
    {
      _pauseable = json_data["pauseable"].asBool();
      DEBUG_STREAM << "Context::configure : Context " << ( _pauseable ? "is" : "is not" ) << " pauseable";
    }


    DEBUG_LOG( "Context::configure : Building context layers" );
    Json::Value& layers = json_data["layers"];
    for ( Json::Value::iterator layer_it = layers.begin(); layer_it != layers.end(); ++layer_it )
    {
      Json::Value layer_data;
      // If the layer is a file name, open it
      if ( layer_it->isString() )
      {
        loadJsonData( layer_data, layer_it->asString() );
      }
      else
      {
        layer_data = *layer_it;
      }

      // Validate required keys
      validateJson( layer_data, "position", JsonType::ARRAY );
      validateJson( layer_data, "width", JsonType::FLOAT );
      validateJson( layer_data, "height", JsonType::FLOAT );
      validateJson( layer_data, "objects", JsonType::ARRAY );
      validateJson( layer_data, "spawns", JsonType::ARRAY );
      validateJson( layer_data, "movement_scale", JsonType::ARRAY );

      // Validate array types
      validateJsonArray( layer_data["position"], 2, JsonType::FLOAT );
      validateJsonArray( layer_data["objects"], 0, JsonType::OBJECT );
      validateJsonArray( layer_data["spawns"], 0, JsonType::OBJECT );
      validateJsonArray( layer_data["movement_scale"], 2, JsonType::FLOAT );

      std::string layer_name = layer_it.key().asString();
      INFO_STREAM << "Building context layer: " << layer_name;

      float x = layer_data["position"][0].asFloat();
      float y = layer_data["position"][1].asFloat();
      float dx = layer_data["movement_scale"][0].asFloat();
      float dy = layer_data["movement_scale"][1].asFloat();
      float w = layer_data["width"].asFloat();
      float h = layer_data["height"].asFloat();

      // Create the layer
      _layers.emplace_back();
      ContextLayer& the_layer = _layers.back();

      the_layer.configure( this, layer_name, Vector( x, y ), Vector( dx, dy ), w, h );


      // Tell the collision handler to create the map of expected collision teams in the new layer
      _theCollision.setupEmptyLayer( the_layer );


      // Find and place all the requested elements
      Json::Value& object_data = layer_data["objects"];
      for  ( Json::ArrayIndex i = 0; i < object_data.size(); ++i )
      {
        validateJson( object_data[i], "name", JsonType::STRING );

        std::string object_name = object_data[i]["name"].asString();
        INFO_STREAM << "Adding game object into context layer: " << object_name;

        PhysicalObject* object;

        // If a global object is requested
        if ( validateJson( object_data[i], "global", JsonType::BOOLEAN, false ) && object_data[i]["global"].asBool() )
        {
          object = Manager::getInstance()->getContextManager<Context>().getGlobalContextGroup()->getPhysicalObject( object_name );
        }
        else
        {
          object = _owner->getPhysicalObject( object_name );
        }

        if ( object == nullptr )
        {
          Exception ex( "Context::configure()", "Object not found." );
          ex.addDetail( "Object Name", object_name );
          ex.addDetail( "Layer Name", layer_name );
          throw ex;
        }

        // If the object is a button add it to the focus handler
        if ( object->hasButton() )
        {
          _theFocus.addObject( dynamic_cast< ButtonObject* >( object ) );
        }

        // If the object expects input register it with the input handler
        if ( object->hasInput() )
        {
          dynamic_cast< ControllableObject* >( object )->registerActions( _theInput );
        }

        // Configure the location of the object within the specified layer
        configureObject( the_layer, object, object_data[i] );

        // and insert!
        the_layer.layerGraph[ object->getCollisionTeam() ].push_back( object );
      }


      // Find and place all the spawned elements
      Json::Value& spawn_data = layer_data["spawns"];
      for ( Json::ArrayIndex j = 0; j < spawn_data.size(); ++j )
      {
        validateJson( spawn_data[j], "name", JsonType::STRING );

        std::string spawn_name = spawn_data[j]["name"].asString();
        INFO_STREAM << "Context::configure : Spawning game object into context layer: " << spawn_name;

        PhysicalObject* object;

        // If a global object is requested
        if ( validateJson( spawn_data[j], "global", JsonType::BOOLEAN, false ) && spawn_data[j]["global"].asBool() )
        {

          object = Manager::getInstance()->getContextManager<Context>().getGlobalContextGroup()->spawnPhysicalObject( spawn_name );
        }
        else
        {
          object = _owner->spawnPhysicalObject( spawn_name );
        }

        if ( object == nullptr )
        {
          Exception ex( "Context::configure()", "Object could not be spawned. Too many copies exist already." );
          ex.addDetail( "Object Name", spawn_name );
          ex.addDetail( "Layer Name", layer_name );
          throw ex;
        }

//        // If the object is a button add it to the focus handler. Should spawnable objects have a focus property?
//        if ( object->hasButton() )
//        {
//          _theFocus.addObject( dynamic_cast< ButtonObject* >( object ) );
//        }

        // If the object expects input register it with the input handler
        if ( object->hasInput() )
        {
          dynamic_cast< ControllableObject* >( object )->registerActions( _theInput );
        }

        // Configure the location of the object within the specified layer
        configureObject( the_layer, object, spawn_data[j] );

        // and insert!
        the_layer.layerGraph[ object->getCollisionTeam() ].push_back( object );
      }
    }


//    // Let the focus handler register input actions
//    _theFocus.registerActions( _theInput );

    // Register the input actions that this context can receive
    this->registerActions( _theInput );
  }


////////////////////////////////////////////////////////////////////////////////////////////////////

  void configureObject( ContextLayer& layer, PhysicalObject* object, Json::Value& json_data )
  {
    DEBUG_LOG( "Context::configureObject : Placing in Layer" );

    // Load the absolute position
    if ( validateJson( json_data, "position", JsonType::ARRAY, false ) )
    {
      validateJsonArray( json_data["position"], 2, JsonType::FLOAT );
      Vector pos( 0.0 );

      float x = json_data["position"][0].asFloat();
      float y = json_data["position"][1].asFloat();
      pos.set( x, y );

      object->setPosition( pos );
    }

    // Load the rotation of the object
    if ( validateJson( json_data, "rotation", JsonType::FLOAT, false ) )
    {
      object->setRotation( json_data["rotation"].asFloat()*degrees_to_radians );
    }

    // Load the alignment info
    if ( validateJson( json_data, "alignment", JsonType::ARRAY, false ) )
    {
      validateJsonArray( json_data["alignment"], 2, JsonType::STRING );
      Vector offset( 0.0 );

      if ( json_data["alignment"][0].asString() == "center" )
      {
        offset.x() = layer.getPosition().x() + 0.5*( layer.getWidth() - object->getWidth() );
      }
      else if ( json_data["alignment"][0].asString() == "right" )
      {
        offset.x() = layer.getPosition().x() + ( layer.getWidth() - object->getWidth() );
      }

      if ( json_data["alignment"][1].asString() == "center" )
      {
        offset.y() = layer.getPosition().y() + 0.5*( layer.getHeight() - object->getHeight() );
      }
      else if ( json_data["alignment"][1].asString() == "bottom" )
      {
        offset.y() = layer.getPosition().y() + ( layer.getHeight() - object->getHeight() );
      }

      object->setPosition( object->getPosition() + offset );
    }

    // Load the velocity
    if ( validateJson( json_data, "velocity", JsonType::ARRAY, false ) )
    {
      validateJsonArray( json_data["velocity"], 2, JsonType::FLOAT );
      Vector vel( 0.0 );

      vel.x() = json_data["velocity"][0].asFloat();
      vel.y() = json_data["velocity"][1].asFloat();

      object->setVelocity( vel );
    }

    // Load the angular velocity
    if ( validateJson( json_data, "angular_velocity", JsonType::FLOAT, false ) )
    {
      float ang_vel = json_data["angular_velocity"].asFloat()*degrees_to_radians;
      object->setAngularVelocity( ang_vel );
    }
  }
}

