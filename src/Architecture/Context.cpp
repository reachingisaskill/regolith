
#include "Regolith/Architecture/Context.h"
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

  Vector placeInLayer( ContextLayer*, PhysicalObject*, Json::Value& );

////////////////////////////////////////////////////////////////////////////////////////////////////

  Context::Context() :
    MassProduceable(),
    ControllableInterface(),
    Component(),
    _theInput(),
    _theAudio(),
    _theFocus(),
    _theCollision(),
    _theData(),
    _theCamera(),
    _paused( false ),
    _pauseable( false ),
    _layers( "Context Layers" ),
    _spawnedObjects(),
    _animatedObjects()
  {
  }


  Context::~Context()
  {
    INFO_LOG( "Clearing animated cache" );
    _animatedObjects.clear();

    INFO_LOG( "Clearing spawned objects" );
    for ( PhysicalObjectList::iterator it = _spawnedObjects.begin(); it != _spawnedObjects.end(); ++it )
    {
      delete (*it);
    }
    _spawnedObjects.clear();

    INFO_LOG( "Clearing layers" );
    _layers.clear();
  }


  void Context::update( float time )
  {
    DEBUG_LOG( "Context Update" );

    // Update the camera first
    _theCamera.update( time );

    // Update all the animated objects
    AnimatedList::iterator it = _animatedObjects.begin();
    AnimatedList::iterator end =  _animatedObjects.end();
    while ( it != end )
    {
      (*it)->update( time );
      ++it;
    }

    updateContext( time );
  }


  void Context::step( float time )
  {
    DEBUG_LOG( "Context Step" );
    NamedVector< ContextLayer, true >::iterator layers_end = _layers.end();
    for ( NamedVector<ContextLayer, true>::iterator layer_it = _layers.begin(); layer_it != layers_end; ++layer_it )
    {
      MoveableList& moveables = (*layer_it)->moveables;
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
    NamedVector< ContextLayer, true >::iterator layers_end = _layers.end();
    for ( NamedVector<ContextLayer, true>::iterator layer_it = _layers.begin(); layer_it != layers_end; ++layer_it )
    {
      _theCamera.setLayer( *layer_it );

      DEBUG_STREAM << " Rendering layer. " << (*layer_it)->drawables.size() << " Elements";

      DrawableList& drawables = (*layer_it)->drawables;
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

    NamedVector< ContextLayer, true >::iterator layers_end = _layers.end();
    for ( NamedVector<ContextLayer, true>::iterator layer_it = _layers.begin(); layer_it != layers_end; ++layer_it )
    {
      DEBUG_STREAM << " Starting Layer Collision: " << (*layer_it)->teams.size();

      // Colliding objects
      CollisionHandler::iterator end = _theCollision.collisionEnd();
      for ( CollisionHandler::iterator it = _theCollision.collisionBegin(); it != end; ++it )
      {
        CollidableList& team1 = (*layer_it)->teams[ it->first ];
        CollidableList& team2 = (*layer_it)->teams[ it->second ];

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
        CollidableList& team1 = (*layer_it)->teams[ it->first ];
        CollidableList& team2 = (*layer_it)->teams[ it->second ];

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


  void Context::spawn( unsigned int id, unsigned int layer_num, const Vector& position )
  {
    PhysicalObject* object = _theData.spawn( id, position );
    addSpawnedObject( object, layer_num );
    _spawnedObjects.push_back( object );
  }


  void Context::addSpawnedObject( PhysicalObject* object, unsigned int layer_num )
  {
    if ( object->hasInput() )
    {
      dynamic_cast<Controllable*>( object )->registerActions( _theInput );
    }
    if ( object->hasAudio() )
    {
      dynamic_cast<Noisy*>( object )->registerSounds( &_theAudio );
    }
    if ( object->hasAnimation() )
    {
      _animatedObjects.push_back( dynamic_cast<Animated*>( object ) );
    }
    if ( object->hasClick() )
    {
      _theFocus.addObject( dynamic_cast<Clickable*>( object ) );
    }
//    if ( object->hasInteraction() )
//    {
//       // Nothing to do for this one
//    }


    ContextLayer* layer = _layers[layer_num];

    if ( object->hasTexture() )
    {
      layer->drawables.push_back( dynamic_cast<Drawable*>( object ) );
    }
    if ( object->hasMovement() )
    {
      layer->moveables.push_back( dynamic_cast<Moveable*>( object ) );
    }
    if ( object->hasCollision() )
    {
      Collidable* temp = dynamic_cast<Collidable*>( object );
      layer->teams[ temp->getTeam() ].push_back( temp );
    }
  }


  void Context::cacheObject( GameObject* object )
  {
    if ( object->hasInput() )
    {
      dynamic_cast<Controllable*>( object )->registerActions( _theInput );
    }
    if ( object->hasAudio() )
    {
      dynamic_cast<Noisy*>( object )->registerSounds( &_theAudio );
    }
    if ( object->hasAnimation() )
    {
      _animatedObjects.push_back( dynamic_cast<Animated*>( object ) );
    }
    if ( object->hasClick() )
    {
      _theFocus.addObject( dynamic_cast<Clickable*>( object ) );
    }
//    if ( object->hasInteraction() )
//    {
//       // Nothing to do for this one
//    }
  }


//////////////////////////////////////////////////////////////////////////////////////////////////// 
  // Context configuration

  void Context::configure( Json::Value& json_data, ContextGroup& handler )
  {
    INFO_LOG( "Configuring Context" );

    Utilities::validateJson( json_data, "input_mapping", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "data_handler", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "layers", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJson( json_data, "contexts", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJson( json_data, "camera", Utilities::JSON_TYPE_OBJECT );
    Utilities::validateJson( json_data, "collision_handling", Utilities::JSON_TYPE_OBJECT );


    // Configure the data handler
    handler.configureDataHandler( _theData, json_data["data_handler"].asString() );
    // Configure the collision rules
    _theCollision.configure( json_data["collision_handling"] );
    // Configure the input mapping
    _theInput.configure( json_data["input_mapping"].asString() );

    if ( Utilities::validateJson( json_data, "pauseable", Utilities::JSON_TYPE_BOOLEAN, false ) )
    {
      _pauseable = json_data["pauseable"].asBool();
      DEBUG_STREAM << "  Context " << ( _pauseable ? "is" : "is not" ) << " pauseable";
    }

    DEBUG_LOG( "  Building context layers" );
    Json::Value& layer_data = json_data["layers"];
    Json::ArrayIndex layer_data_size = layer_data.size();
    for ( Json::ArrayIndex i = 0; i != layer_data_size; ++i )
    {
      Utilities::validateJson( layer_data[i], "name", Utilities::JSON_TYPE_STRING );
      Utilities::validateJson( layer_data[i], "position", Utilities::JSON_TYPE_ARRAY );
      Utilities::validateJsonArray( layer_data[i]["position"], 2, Utilities::JSON_TYPE_FLOAT );
      Utilities::validateJson( layer_data[i], "width", Utilities::JSON_TYPE_FLOAT );
      Utilities::validateJson( layer_data[i], "height", Utilities::JSON_TYPE_FLOAT );
      Utilities::validateJson( layer_data[i], "elements", Utilities::JSON_TYPE_ARRAY );
      Utilities::validateJson( layer_data[i], "movement_scale", Utilities::JSON_TYPE_ARRAY );
      Utilities::validateJsonArray( layer_data[i]["movement_scale"], 2, Utilities::JSON_TYPE_FLOAT );

      float x = layer_data[i]["position"][0].asFloat();
      float y = layer_data[i]["position"][1].asFloat();
      float dx = layer_data[i]["movement_scale"][0].asFloat();
      float dy = layer_data[i]["movement_scale"][1].asFloat();
      float w = layer_data[i]["width"].asFloat();
      float h = layer_data[i]["height"].asFloat();

      ContextLayer* newLayer = new ContextLayer( Vector( x, y ), Vector( dx, dy ), w, h );
      unsigned int layer_number = _layers.addObject( newLayer, layer_data[i]["name"].asString() );


      Json::Value& element_data = layer_data[i]["elements"];
      Json::ArrayIndex element_data_size = element_data.size();
      for ( Json::ArrayIndex j = 0; j != element_data_size; ++j )
      {
        Utilities::validateJson( element_data[j], "name", Utilities::JSON_TYPE_STRING );

        std::string name = element_data[j]["name"].asString();
        INFO_STREAM << "Loading element into context layer: " << name;

        if ( Utilities::validateJson( element_data[j], "is_global", Utilities::JSON_TYPE_BOOLEAN, false ) && element_data[j]["is_global"].asBool() )
        {
          GameObject* element = _theData.getGameObject( name );
          if ( element->isPhysical() )
          {
            INFO_LOG( "Element is physical. Placing global object within layer." );

            PhysicalObject* phys_element = dynamic_cast<PhysicalObject*>( element );
            addSpawnedObject( phys_element, layer_number );

            if ( Utilities::validateJson( layer_data[i], "position", Utilities::JSON_TYPE_ARRAY, false ) ) // Position values are optional for global elements
            {
              Vector pos = placeInLayer( newLayer, phys_element, element_data[j] );
              phys_element->setPosition( pos );
            }
          }
          else
          {
            INFO_LOG( "Element is not physical. Caching object." );
            cacheObject( element );
          }
        }
        else
        {
          INFO_LOG( "Spawning object in layer" );
          Vector pos = placeInLayer( newLayer, _theData.getPhysicalObject( name ), element_data[j] );

          PhysicalObject* element = _theData.spawn( name, pos );
          addSpawnedObject( element, layer_number );
          _spawnedObjects.push_back( element );
        }
      }
    }


    DEBUG_LOG( "Building child contexts" );
    Json::Value& context_data = json_data["contexts"];
    Json::ArrayIndex context_data_size = context_data.size();
    for ( Json::ArrayIndex i = 0; i != context_data_size; ++i )
    {
      handler.buildContext( context_data[i] );
    }


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

      PhysicalObject* followee =  _theData.getPhysicalObject( camera_follow );
      INFO_STREAM << "Setting camera to follow : " << camera_follow << " @ " << followee;
      _theCamera.followMe( followee );
    }


    _theFocus.registerActions( _theInput );

    // Finall call the overriden function for the context
    this->registerActions( _theInput );
  }


////////////////////////////////////////////////////////////////////////////////////////////////////

  Vector placeInLayer( ContextLayer* layer, PhysicalObject* object, Json::Value& json_data )
  {
    DEBUG_LOG( "Placing in Layer" );
    Utilities::validateJson( json_data, "position", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJsonArray( json_data["position"], 2, Utilities::JSON_TYPE_FLOAT );

    float x = json_data["position"][0].asFloat();
    float y = json_data["position"][1].asFloat();
    Vector pos( x, y );
    Vector offset( 0.0, 0.0 );

    if ( Utilities::validateJson( json_data, "alignment", Utilities::JSON_TYPE_ARRAY, false ) )
    {
      Utilities::validateJsonArray( json_data["alignment"], 2, Utilities::JSON_TYPE_STRING );

      if ( json_data["alignment"][0].asString() == "center" )
      {
        offset.x() = layer->getPosition().x() + 0.5*( layer->getWidth() - object->getWidth() );
      }
      else if ( json_data["alignment"][0].asString() == "right" )
      {
        offset.x() = layer->getPosition().x() + ( layer->getWidth() - object->getWidth() );
      }

      if ( json_data["alignment"][1].asString() == "center" )
      {
        offset.y() = layer->getPosition().y() + 0.5*( layer->getHeight() - object->getHeight() );
      }
      else if ( json_data["alignment"][1].asString() == "bottom" )
      {
        offset.y() = layer->getPosition().y() + ( layer->getHeight() - object->getHeight() );
      }
    }

    return pos+offset;
  }
}

