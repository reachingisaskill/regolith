
#ifndef REGOLITH_DIALOG_H_
#define REGOLITH_DIALOG_H_

#include "Managers/Definitions.h"

#include "Architecture/Context.h"
#include "Components/NamedVector.h"
#include "Components/ClickableSet.h"

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
    friend class DialogFactory;
    typedef std::vector< Drawable* > ElementList;

    Dialog( const Dialog& ) = delete;
    Dialog& operator=( const Dialog& ) = delete;

    private:
      Camera* _cameraHUD;

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

    protected:
      virtual void _render( Camera* ) = 0;

      virtual void _update( Uint32 ) = 0;

    public:
      // Scene pointer, camera pointer, json object containing this dialog
      Dialog( Camera*, std::string );

      // Destruct all the owned memory
      virtual ~Dialog();


      // Return the set of all clickable objects
      ClickableSet& getSet() { return _buttons; }



      // Enable to context interface
      // Update all animations, sprite changes, etc
      virtual void update( Uint32 ) override;

      // Render the dialog using the stored camera
      virtual void render() override;

      // Return true if animated
      virtual bool isAnimated() const override { return true; }

      // Return true if visible
      virtual bool isVisible() const override { return true; }

      // No child contexts - this function isn't needed
      virtual void returnFocus() override {}


      // Raise an event in the current context
//      virtual void raiseContextEvent( ContextEvent ) override = 0;



      // Enable the controllable interface
      // No game-wide events
      virtual void registerEvents( InputManager* ) override {}

      // Register context-wide actions
      virtual void registerActions( InputHandler* ) override;

      // Interfaces for input
//      virtual void eventAction( const RegolithEvent&, const SDL_Event& ) override;
      virtual void booleanAction( const InputAction&, bool ) override;
//      virtual void floatAction( const InputAction&, float ) override;
      virtual void vectorAction( const InputAction&, const Vector& ) override;
      virtual void mouseAction( const InputAction&, bool, const Vector& ) override;
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Menu Dialog - The most common of dialogs

  class MenuDialog : public Dialog
  {
    friend class MenuDialogFactory;

    private:
      NamedVector<Dialog, true> _subDialogs;

    protected :
      void _render( Camera* ) override {}

      void _update( Uint32 ) override {}

    public :
      MenuDialog( Camera*, std::string );

      virtual ~MenuDialog();


      // Go to the corresponding sub dialog
      Dialog* selectDialog( unsigned int );


      // Raise an event in the current context
      // In this context it selects a child dialog from the subDialogs member
      virtual void raiseContextEvent( ContextEvent ) override;

//      virtual void eventAction( const RegolithEvent&, const SDL_Event& ) override;
//      virtual void booleanAction( const InputAction&, bool ) override;
//      virtual void floatAction( const InputAction&, float ) override;
//      virtual void vectorAction( const InputAction&, const Vector& ) override;
//      virtual void mouseAction( const InputAction&, bool, const Vector& ) override;
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Book Dialog. Has pages of text & choices

//  class BookDialog : public Dialog
//  {
//  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Shop Dialog. Buy and sell items to/from various inventory objects (TBD)

//  class ShopDialog : public Dialog
//  {
//  };

}

#endif // REGOLITH_DIALOG_H_

