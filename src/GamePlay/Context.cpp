
#include "Regolith/GamePlay/Context.h"

#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/Architecture/NoisyObject.h"
#include "Regolith/Architecture/ButtonObject.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Components/Camera.h"
#include "Regolith/GamePlay/ContextLayer.h"


namespace Regolith
{

  // Local function declarations

  Vector placeInLayer( ContextLayer&, PhysicalObject*, Json::Value& );

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
    ContextLayerMap::iterator found = _layers.find( name );
    if ( found == _layers.end() )
    {
      Exception ex( "Context::getLayer()", "Requested context layer not found" );
      ex.addDetail( "Name", name );
      throw ex;
    }
    return found->second;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Context stack call back functions

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
    ContextLayerMap::iterator layer_end = _layers.end();
    for ( ContextLayerMap::iterator layer_it = _layers.begin(); layer_it != layer_end; ++layer_it )
    {
      for ( LayerGraph::iterator team_it = layer_it->second.layerGraph.begin(); team_it != layer_it->second.layerGraph.end(); ++team_it )
      {
        for ( PhysicalObjectList::iterator it = team_it->second.begin(); it != team_it->second.end(); ++it )
        {
          // If object is marked for destruction, remove it from the scene graph
          if ( (*it)->isDestroyed() )
          {
            team_it->second.erase( it );
            continue;
          }

          // If object can be moved, do the physics integration
          if ( (*it)->hasMovement() )
          {
            (*it)->step( time );
          }

          // If the object is animated, update the animation
          if ( (*it)->hasAnimation() )
          {
            (*it)->update( time );
          }
        }
      }
    }

    // Update the context state
    updateContext( time );

    // Update the camera position
    _cameraPosition = updateCamera( time );


//    DEBUG_STREAM << "ContextLayer::update : Starting Layer Collision";
//
//    // Colliding objects
//    CollisionHandler::iterator end = _theCollision.collisionEnd();
//    for ( CollisionHandler::iterator it = _theCollision.collisionBegin(); it != end; ++it )
//    {
//      PhysicalObjectList& team1 = _layerGraph[ it->first ];
//      PhysicalObjectList& team2 = _layerGraph[ it->second ];
//
//      PhysicalObjectList::iterator end1 = team1.end();
//      PhysicalObjectList::iterator end2 = team2.end();
//
//      for ( CollidableList::iterator it1 = team1.begin(); it1 != end1; ++it1 )
//      {
//        for ( CollidableList::iterator it2 = team2.begin(); it2 != end2; ++it2 )
//        {
//          collides( (*it1), (*it2) );
//        }
//      }
//    }
  }


  void Context::render( Camera& camera )
  {
    DEBUG_LOG( "Context::render : Context Render" );
    ContextLayerMap::iterator layer_end = _layers.end();
    for ( ContextLayerMap::iterator layer_it = _layers.begin(); layer_it != layer_end; ++layer_it )
    {
      const Vector& layer_position = layer_it->second.getPosition();
      const Vector& movement_scale = layer_it->second.getMovementScale();

      // % - Directional dot-product
      Vector camera_position = ( _cameraPosition - layer_position ) % movement_scale;

      for ( LayerGraph::iterator team_it = layer_it->second.layerGraph.begin(); team_it != layer_it->second.layerGraph.end(); ++team_it )
      {
        for ( PhysicalObjectList::iterator it = team_it->second.begin(); it != team_it->second.end(); ++it )
        {
          // If the object can be drawn, render it to the back buffer
          if ( (*it)->hasTexture() )
          {
            camera.renderPhysicalObject( (*it), camera_position );
          }
        }
      }
    }
  }

//////////////////////////////////////////////////////////////////////////////////////////////////// 
  // Context configuration

  void Context::configure( Json::Value& json_data, ContextGroup& handler )
  {
    INFO_LOG( "Context::configure : Configuring Context" );
    _owner = &handler;

    Utilities::validateJson( json_data, "layers", Utilities::JSON_TYPE_OBJECT );
    Utilities::validateJson( json_data, "camera", Utilities::JSON_TYPE_OBJECT );
    Utilities::validateJson( json_data, "collision_handling", Utilities::JSON_TYPE_OBJECT );

    // Configure the collision rules
    _theCollision.configure( json_data["collision_handling"] );

    // Configure the input mapping if one is required
    if ( Utilities::validateJson( json_data, "input_mapping", Utilities::JSON_TYPE_STRING, false ) )
    {
      _theInput.configure( json_data["input_mapping"].asString() );
    }


    // Pause behaviour
    if ( Utilities::validateJson( json_data, "pauseable", Utilities::JSON_TYPE_BOOLEAN, false ) )
    {
      _pauseable = json_data["pauseable"].asBool();
      DEBUG_STREAM << "  Context " << ( _pauseable ? "is" : "is not" ) << " pauseable";
    }


    DEBUG_LOG( "Context::configure : Building context layers" );
    Json::Value& layers = json_data["layers"];
    for ( Json::Value::iterator layer_it = layers.begin(); layer_it != layers.end(); ++layer_it )
    {
      Json::Value& layer_data = *layer_it;

      Utilities::validateJson( layer_data, "position", Utilities::JSON_TYPE_ARRAY );
      Utilities::validateJsonArray( layer_data["position"], 2, Utilities::JSON_TYPE_FLOAT );
      Utilities::validateJson( layer_data, "width", Utilities::JSON_TYPE_FLOAT );
      Utilities::validateJson( layer_data, "height", Utilities::JSON_TYPE_FLOAT );
      Utilities::validateJson( layer_data, "elements", Utilities::JSON_TYPE_ARRAY );
      Utilities::validateJson( layer_data, "movement_scale", Utilities::JSON_TYPE_ARRAY );
      Utilities::validateJsonArray( layer_data["movement_scale"], 2, Utilities::JSON_TYPE_FLOAT );

      std::string layer_name = layer_it.key().asString();

      INFO_STREAM << "Building context layer: " << layer_name;

      float x = layer_data["position"][0].asFloat();
      float y = layer_data["position"][1].asFloat();
      float dx = layer_data["movement_scale"][0].asFloat();
      float dy = layer_data["movement_scale"][1].asFloat();
      float w = layer_data["width"].asFloat();
      float h = layer_data["height"].asFloat();


      // Creates it if it doesn't already exist
      _layers[ layer_name ].configure( this, Vector( x, y ), Vector( dx, dy ), w, h );


      // Find and place all the requested elements
      Json::Value& element_data = layer_data["elements"];
      Json::ArrayIndex element_data_size = element_data.size();
      for ( Json::ArrayIndex j = 0; j != element_data_size; ++j )
      {
        Utilities::validateJson( element_data[j], "name", Utilities::JSON_TYPE_STRING );

        std::string object_name = element_data[j]["name"].asString();
        INFO_STREAM << "Adding game object into context layer: " << object_name;

        PhysicalObject* object;

        // If a global object is requested
        if ( Utilities::validateJson( element_data[j], "global", Utilities::JSON_TYPE_BOOLEAN, false ) && element_data[j]["global"].asBool() )
        {
          object = Manager::getInstance()->getContextManager().getGlobalContextGroup()->getPhysicalObject( object_name );
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

        Vector object_pos = placeInLayer( _layers[ layer_name ], object, element_data[j] );
        object->setPosition( object_pos );

        _layers[ layer_name ].layerGraph[ object->getCollisionTeam() ].push_back( object );
      }
    }


    /*
    // Configure the camera
    Json::Value camera_data = json_data["camera"];
    Utilities::validateJson( camera_data, "lower_limit", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJson( camera_data, "upper_limit", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJsonArray( camera_data["lower_limit"], 2, Utilities::JSON_TYPE_FLOAT );
    Utilities::validateJsonArray( camera_data["upper_limit"], 2, Utilities::JSON_TYPE_FLOAT );

    // Set the camera follow if provided
    if ( ( ! camera_data["follow"].isNull() ) && Utilities::validateJson( camera_data, "follow", Utilities::JSON_TYPE_STRING, false ) )
    {
      std::string camera_follow = camera_data["follow"].asString();

      if ( Utilities::validateJson( camera_data, "global", Utilities::JSON_TYPE_BOOLEAN, false ) && camera_data["global"].asBool() )
      {
        PhysicalObject* followee = Manager::getInstance()->getContextManager().getGlobalContextGroup()->getPhysicalObject( camera_follow );
        INFO_STREAM << "Setting camera to follow global object : " << camera_follow << " @ " << followee;
        _theCamera.followMe( followee );
      }
      else
      {
        PhysicalObject* followee = _owner->getPhysicalObject( camera_follow );
        INFO_STREAM << "Setting camera to follow : " << camera_follow << " @ " << followee;
        _theCamera.followMe( followee );
      }
    }
    */



//    // Let the focus handler register input actions
//    _theFocus.registerActions( _theInput );

    // Finall call the overriden function for the context
    this->registerActions( _theInput );
  }


////////////////////////////////////////////////////////////////////////////////////////////////////

  Vector placeInLayer( ContextLayer& layer, PhysicalObject* object, Json::Value& json_data )
  {
    DEBUG_LOG( "Context::placeInLayer : Placing in Layer" );
    Vector pos( 0.0 );
    Vector offset( 0.0 );

    if ( Utilities::validateJson( json_data, "position", Utilities::JSON_TYPE_ARRAY, false ) )
    {
      Utilities::validateJsonArray( json_data["position"], 2, Utilities::JSON_TYPE_FLOAT );

      float x = json_data["position"][0].asFloat();
      float y = json_data["position"][1].asFloat();
      pos.set( x, y );
    }

    if ( Utilities::validateJson( json_data, "alignment", Utilities::JSON_TYPE_ARRAY, false ) )
    {
      Utilities::validateJsonArray( json_data["alignment"], 2, Utilities::JSON_TYPE_STRING );

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
    }

    return pos+offset;
  }
}

