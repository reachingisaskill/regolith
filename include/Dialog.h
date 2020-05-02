
#ifndef REGOLITH_DIALOG_H_
#define REGOLITH_DIALOG_H_

#include "Definitions.h"

#include "Context.h"
#include "NamedVector.h"
#include "ClickableSet.h"

#include <string>
#include <json/json.h>


namespace Regolith
{

  class Scene;
  class Camera;
  class Dialog;

  /*
   * Classes for defining dialog-style interaction boxes in a tree structure.
   * Support multiple pages and branching options.
   * Useful for interactions with NPCs and interactable objects,
   * but also things like map overlays and inventory/pause menus
   *
   * Designed to be built after the scene - uses the Scenes resources and textures
   * no point in re-implementing the memory handling
   */

  class Dialog : public Context
  {
    typedef std::vector< Drawable* > ElementList;

    Dialog( const Dialog& ) = delete;
    Dialog& operator=( const Dialog& ) = delete;

    private:
      Camera* _cameraHUD;
      std::string _name;
      NamedVector<Dialog, true> _subDialogs;

      // Options
      bool _canCancel;

      // List of the spawned elements
      // Dialog owns this memory - all elements in the dialog
      ElementList _elements;

      // Separate the background sprite
      Drawable* _background;

      // All the elements that can be clicked on/have focus - No memory ownership associated
      ClickableSet _buttons;

      // All the elements that are animated - No memory ownership associated
      ElementList _animated;

    public:
      // Scene pointer, camera pointer, json object containing this dialog
      Dialog( Camera*, Json::Value& );

      // Destruct all the owned memory
      virtual ~Dialog();

      // Load the dialog
      void loadFromJson( Json::Value& );


      // Return the set of all clickable objects
      ClickableSet& getSet() { return _buttons; }

      // Go to the corresponding sub dialog
      Dialog* selectDialog( unsigned int );


      // Update all animations, sprite changes, etc
      void update( Uint32 );

      // Render the dialog using the stored camera
      void render();

      // Raise an event in the current context
      void raiseContextEvent( ContextEvent );

      // Return true if animated
      virtual bool isAnimated() const { return true; }

      // Return true if visible
      virtual bool isVisible() const { return true; }

      // No child contexts - this function isn't needed
      void returnFocus() {}



      // Enable the controllable interface
      // Don't allow derived classes to access game-wide events
      void registerEvents( InputManager* ) {}

      // Register context-wide actions
      virtual void registerActions( InputHandler* );

      // Interfaces for input
      virtual void eventAction( const RegolithEvent&, const SDL_Event& ) {}
      virtual void booleanAction( const InputAction&, bool );
      virtual void floatAction( const InputAction&, float ) {}
      virtual void vectorAction( const InputAction&, const Vector& );
  };

}

#endif // REGOLITH_DIALOG_H_

