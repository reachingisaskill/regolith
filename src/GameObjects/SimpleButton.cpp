#define LOGTASTIC_DEBUG_OFF

#include "Regolith/GameObjects/SimpleButton.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/GamePlay/Camera.h"

#include "logtastic.h"


namespace Regolith
{
  
  SimpleButton::SimpleButton() :
    Drawable(),
    Interactable(),
    Clickable(),
    _textures(),
    _destination( { 0, 0, 0, 0 } ),
    _currentTexture( nullptr )
  {
  }


  SimpleButton::~SimpleButton()
  {
  }


  void SimpleButton::onStateChange( Clickable::State state )
  {
    _currentTexture = &_textures[state];
  }


  PhysicalObject* SimpleButton::clone( const Vector& pos ) const
  {
    SimpleButton* temp = new SimpleButton( *this );
    temp->setPosition( pos );
    return temp;
  }


  void SimpleButton::render( SDL_Renderer* renderer, const Camera& camera ) const
  {
    // Place the object
    _destination.x = position().x();
    _destination.y = position().y();
    _destination.w = getWidth();
    _destination.h = getHeight();
    
    // Move into the camera reference frame
    SDL_Rect destination = camera.place( _destination );
    DEBUG_STREAM << "BUTTON RENDER: Original: " << _destination.x << ", " << _destination.y << ", " <<  _destination.w << ", " << _destination.h << ", NEW: " << destination.x << ", " << destination.y << ", " << destination.w << ", " << destination.h;

    _currentTexture->draw( renderer, &destination );
  }


  void SimpleButton::configure( Json::Value& json_data, ContextGroup& cg, DataHandler& handler )
  {
    Drawable::configure( json_data, cg, handler );
    Interactable::configure( json_data, cg, handler );
    Clickable::configure( json_data, cg, handler );


    Utilities::validateJson( json_data, "textures", Utilities::JSON_TYPE_OBJECT );
    Json::Value& textures = json_data["textures"];

    // Require the normal texture - this is the default
    Utilities::validateJson( textures, "normal", Utilities::JSON_TYPE_OBJECT );
    _textures[Clickable::NORMAL].configure( textures["normal"], handler );

    if ( Utilities::validateJson( textures, "down", Utilities::JSON_TYPE_OBJECT, false ) )
    {
      _textures[Clickable::DOWN].configure( textures["down"], handler );
    }
    else
    {
      _textures[Clickable::DOWN].configure( textures["normal"], handler );
    }

    if ( Utilities::validateJson( textures, "focus", Utilities::JSON_TYPE_OBJECT, false ) )
    {
      _textures[Clickable::FOCUSSED].configure( textures["focus"], handler );
    }
    else
    {
      _textures[Clickable::FOCUSSED].configure( textures["normal"], handler );
    }

    if ( Utilities::validateJson( textures, "inactive", Utilities::JSON_TYPE_OBJECT, false ) )
    {
      _textures[Clickable::INACTIVE].configure( textures["inactive"], handler );
    }
    else
    {
      _textures[Clickable::INACTIVE].configure( textures["normal"], handler );
    }

    onStateChange( Clickable::NORMAL );
  }

}

