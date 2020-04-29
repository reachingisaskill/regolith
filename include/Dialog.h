
#ifndef __REGOLITH__DIALOG_H__
#define __REGOLITH__DIALOG_H__

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

  class DialogWindow : public Context
  {
    private:
      Scene* _scene;
      Camera* _camera;

      // DialogWindow owns this memory
      Dialog* _dialogTree;

      // This is just a pointer to the currently visible dialog - NOT unique and deletable memory.
      Dialog* _currentDialog;


    public:
      // Scene pointer, HUD camera, json file
      DialogWindow( Scene*, Camera*, Json::Value& );

      // Load the Dialog data from the file
      void loadFromJson( Json::Value& );

      // Update all animations, sprite changes, etc
      void update( Uint32 );

      // Render the dialog using the stored camera
      void render();

      // Raise an event in the current context
      void raiseContextEvent( ContextEvent );
  };


////////////////////////////////////////////////////////////////////////////////////////////////////


  class Dialog
  {
    typedef std::vector< Drawable* > ElementList;

    Dialog( const Dialog& ) = delete;
    Dialog& operator=( const Dialog& ) = delete;

    private:
      Scene* _owner;
      Camera* _HUDCamera;
      NamedVector<Dialog> _subDialogs;

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
      Dialog( Scene*, Camera*, Json::Value& );

      // Destruct all the owned memory
      virtual ~Dialog();

      // Update all animations, sprite changes, etc
      void update( Uint32 );

      // Render the dialog using the stored camera
      void render();


      // Go to the corresponding sub dialog
      Dialog* selectDialog( unsigned int );

  };

}

#endif // __REGOLITH__DIALOG_H__

