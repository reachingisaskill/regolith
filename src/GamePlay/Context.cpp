
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
    _theCamera(),
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


  void Context::update( float time )
  {
    DEBUG_LOG( "Context::update : Context Update" );

    // Update the camera first
    _theCamera.update( time );

    // Update all the animated objects
    ContextLayerMap::iterator layer_end = _layers.end();
    for ( ContextLayerMap::iterator layer_it = _layers.begin(); layer_it != layer_end; ++layer_it )
    {
      layer_it->second.update( time );
    }

    updateContext( time );
  }


  void Context::render( Camera& camera )
  {
    DEBUG_LOG( "Context::render : Context Render" );
    ContextLayerMap::iterator layer_end = _layers.end();
    for ( ContextLayerMap::iterator layer_it = _layers.begin(); layer_it != layer_end; ++layer_it )
    {
      layer_it->second.render( camera );
    }
  }


  void Context::followMe( PhysicalObject* object )
  {
    _theObject = object;
    _offset.x() = 0.5*Manager::getInstance()->getWindow().getResolutionWidth() - 0.5*object->getWidth();
    _offset.y() = 0.5*Manager::getInstance()->getWindow().getResolutionHeight() - 0.5*object->getHeight();
  }


  ContextLayer& Context::getLayer( std::string name )
  {
    ContextLayerMap::iterator found = _layers.find( name );
    if ( found == _layers.end() )
    {
    }
    return found->second;
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
      ContextLayer& current_layer = _layers.create( layer_name );
      current_layer.configure( this, Vector( x, y ), Vector( dx, dy ), w, h );


      // Find and place all the requested elements
      Json::Value& element_data = layer_data["elements"];
      Json::ArrayIndex element_data_size = element_data.size();
      for ( Json::ArrayIndex j = 0; j != element_data_size; ++j )
      {
        Utilities::validateJson( element_data[j], "name", Utilities::JSON_TYPE_STRING );

        std::string object_name = element_data[j]["name"].asString();
        INFO_STREAM << "Adding game object into context layer: " << object_name;

        GameObject* object;

        // If a global object is requested
        if ( Utilities::validateJson( element_data[j], "global", Utilities::JSON_TYPE_BOOLEAN, false ) && element_data[j]["global"].asBool() )
        {
          object = Manager::getInstance()->getContextManager().getGlobalContextGroup()->getGameObject( object_name );
        }
        else
        {
          object = _owner->getGameObject( object_name );
        }

        if ( object == nullptr )
        {
          Exception ex( "Context::configure()", "Object not found." );
          ex.addDetail( "Object Name", object_name );
          ex.addDetail( "Layer Name", layer_name );
          throw ex;
        }

        // If a copy is required (make sure its physical)
        if ( Utilities::validateJson( element_data[j], "spawn", Utilities::JSON_TYPE_BOOLEAN, false ) && element_data[j]["spawn"].asBool() )
        {
          if ( ! object->isPhysical() )
          {
            Exception ex( "Context::configure()", "A non-physical object is requesting spawning behaviour." );
            ex.addDetail( "Object Name", object_name );
            ex.addDetail( "Layer Name", layer_name );
            throw ex;
          }

          PhysicalObject* phys_obj = dynamic_cast<PhysicalObject*>( object );
          Vector object_pos = placeInLayer( current_layer, phys_obj, element_data[j] );

          current_layer.spawn( phys_obj, object_pos );
        }
        else
        {
          if ( object->isPhysical() )
          {
            PhysicalObject* phys_obj = dynamic_cast<PhysicalObject*>( object );
            Vector object_pos = placeInLayer( current_layer, phys_obj, element_data[j] );
            phys_obj->setPosition( object_pos );
          }

          current_layer.cacheObject( object );
        }
      }
    }


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

