
#include "Regolith/Architecture/Context.h"
#include "Regolith/Architecture/ContextLayer.h"
#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/Architecture/Noisy.h"
#include "Regolith/Architecture/Interactable.h"
#include "Regolith/Architecture/Controllable.h"
#include "Regolith/Architecture/Drawable.h"
#include "Regolith/Architecture/Moveable.h"
#include "Regolith/Architecture/Collidable.h"
#include "Regolith/Architecture/Animated.h"
#include "Regolith/Managers/Manager.h"

#include "logtastic.h"


namespace Regolith
{

  // Local function declarations

  Vector placeInLayer( ContextLayer&, PhysicalObject*, Json::Value& );

////////////////////////////////////////////////////////////////////////////////////////////////////

  Context::Context() :
    MassProduceable(),
    ControllableInterface(),
    Component(),
    _owner( nullptr ),
    _theInput(),
    _theFocus(),
    _theCollision(),
    _theCamera(),
    _paused( false ),
    _pauseable( false ),
    _layers( "Context Layers" )
  {
  }


  Context::~Context()
  {
    INFO_LOG( "Destroying Context" );
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
    DEBUG_LOG( "Context Update" );

    // Update the camera first
    _theCamera.update( time );

    // Update all the animated objects
    ProxyMap<ContextLayer>::iterator layer_end = _layers.end();
    for ( ProxyMap<ContextLayer>::iterator layer_it = _layers.begin(); layer_it != layer_end; ++layer_it )
    {
      AnimatedList::iterator anim_it = layer_it->second.animated.begin();
      AnimatedList::iterator anim_end = layer_it->second.animated.end();
      while ( anim_it != anim_end )
      {
        (*anim_it)->update( time );
        ++anim_it;
      }
    }

    updateContext( time );
  }


  void Context::step( float time )
  {
    DEBUG_LOG( "Context Step" );
    ProxyMap<ContextLayer>::iterator layer_end = _layers.end();
    for ( ProxyMap<ContextLayer>::iterator layer_it = _layers.begin(); layer_it != layer_end; ++layer_it )
    {
      MoveableList& moveables = layer_it->second.moveables;
      DEBUG_STREAM << " Stepping : " << moveables.size();
      MoveableList::iterator move_it = moveables.begin();
      MoveableList::iterator move_end = moveables.end();
      while ( move_it != move_end )
      {
        if ( (*move_it)->isDestroyed() )
        {
          moveables.erase( move_it++ );
        }
        else
        {
          (*move_it)->step( time );
          ++move_it;
        }
      }
    }
  }


  void Context::render()
  {
    DEBUG_LOG( "Context Render" );
    ProxyMap<ContextLayer>::iterator layer_end = _layers.end();
    for ( ProxyMap<ContextLayer>::iterator layer_it = _layers.begin(); layer_it != layer_end; ++layer_it )
    {
      _theCamera.setLayer( &(layer_it->second) );

      DEBUG_STREAM << " Rendering layer. " << layer_it->second.drawables.size() << " Elements";

      DrawableList& drawables = layer_it->second.drawables;
      DrawableList::iterator draw_it = drawables.begin();
      DrawableList::iterator draw_end = drawables.end();
      while ( draw_it != draw_end )
      {
        if ( (*draw_it)->isDestroyed() )
        {
          drawables.erase( draw_it++ );
        }
        else
        {
          (*draw_it)->render( _theCamera );
          ++draw_it;
        }
      }
    }
  }


  void Context::resolveCollisions()
  {
    DEBUG_LOG( "Context Collisions" );

    ProxyMap<ContextLayer>::iterator layer_end = _layers.end();
    for ( ProxyMap<ContextLayer>::iterator layer_it = _layers.begin(); layer_it != layer_end; ++layer_it )
    {
      DEBUG_STREAM << " Starting Layer Collision: " << layer_it->second.teams.size();

      // Colliding objects
      CollisionHandler::iterator end = _theCollision.collisionEnd();
      for ( CollisionHandler::iterator it = _theCollision.collisionBegin(); it != end; ++it )
      {
        CollidableList& team1 = layer_it->second.teams[ it->first ];
        CollidableList& team2 = layer_it->second.teams[ it->second ];

        CollidableList::iterator end1 = team1.end();
        CollidableList::iterator end2 = team2.end();

        for ( CollidableList::iterator it1 = team1.begin(); it1 != end1; ++it1 )
        {
          if ( ! (*it1)->collisionActive() ) continue;
          for ( CollidableList::iterator it2 = team2.begin(); it2 != end2; ++it2 )
          {
            if ( ! (*it2)->collisionActive() ) continue;

            collides( (*it1), (*it2) );
          }
        }
      }

      DEBUG_LOG( " Starting Layer Containment" );
      // Containing Objects
      end = _theCollision.containerEnd();
      for ( CollisionHandler::iterator it = _theCollision.containerBegin(); it != end; ++it )
      {
        CollidableList& team1 = layer_it->second.teams[ it->first ];
        CollidableList& team2 = layer_it->second.teams[ it->second ];

        CollidableList::iterator end1 = team1.end();
        CollidableList::iterator end2 = team2.end();

        for ( CollidableList::iterator it1 = team1.begin(); it1 != end1; ++it1 )
        {
          if ( ! (*it1)->collisionActive() ) continue;
          for ( CollidableList::iterator it2 = team2.begin(); it2 != end2; ++it2 )
          {
            if ( ! (*it2)->collisionActive() ) continue;

            contains( (*it1), (*it2) );
          }
        }
      }
    }

    DEBUG_LOG( "Context Collisions Resolved" );
  }


  Proxy<ContextLayer> Context::requestLayer( std::string name )
  {
    return _layers.request( name );
  }


  ContextLayer& Context::getLayer( std::string name )
  {
    return _layers.get( name );
  }


//////////////////////////////////////////////////////////////////////////////////////////////////// 
  // Context configuration

  void Context::configure( Json::Value& json_data, ContextGroup& handler )
  {
    INFO_LOG( "Configuring Context" );
    _owner = &handler;

    Utilities::validateJson( json_data, "input_mapping", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "layers", Utilities::JSON_TYPE_OBJECT );
    Utilities::validateJson( json_data, "camera", Utilities::JSON_TYPE_OBJECT );
    Utilities::validateJson( json_data, "collision_handling", Utilities::JSON_TYPE_OBJECT );

    // Configure the collision rules
    _theCollision.configure( json_data["collision_handling"] );

    // Configure the input mapping
    _theInput.configure( json_data["input_mapping"].asString() );


    // Pause behaviour
    if ( Utilities::validateJson( json_data, "pauseable", Utilities::JSON_TYPE_BOOLEAN, false ) )
    {
      _pauseable = json_data["pauseable"].asBool();
      DEBUG_STREAM << "  Context " << ( _pauseable ? "is" : "is not" ) << " pauseable";
    }


    DEBUG_LOG( "Building context layers" );
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

    _theCamera.configure( Vector( camera_data["lower_limit"][0].asFloat(), camera_data["lower_limit"][1].asFloat() ),
                          Vector( camera_data["upper_limit"][0].asFloat(), camera_data["upper_limit"][1].asFloat() ) );

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

    // Let the focus handler register input actions
    _theFocus.registerActions( _theInput );

    // Finall call the overriden function for the context
    this->registerActions( _theInput );
  }


////////////////////////////////////////////////////////////////////////////////////////////////////

  Vector placeInLayer( ContextLayer& layer, PhysicalObject* object, Json::Value& json_data )
  {
    DEBUG_LOG( "Placing in Layer" );
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

