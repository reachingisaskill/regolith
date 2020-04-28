
#ifndef __REGOLITH__DIALOG_H__
#define __REGOLITH__DIALOG_H__

#include "Definitions.h"

#include "Controllable.h"
#include "NamedVector.h"
#include "ClickableSet.h"

#include <string>


namespace Regolith
{

  class Scene;
  class Camera;

  /*
   * Classes for defining dialog-style interaction boxes in a tree structure.
   * Support multiple pages and branching options.
   * Useful for interactions with NPCs and interactable objects,
   * but also things like map overlays and inventory/pause menus
   *
   * Designed to be built after the scene - uses the Scenes resources and textures
   * no point in re-implementing the memory handling
   */

  class Dialog;

  class DialogWindow
  {
    private:
      // DialogWindow owns this memory
      Dialog* _dialogTree;

      // This is just a pointer to the currently visible dialog- NOT unique and deletable memory.
      Dialog* _currentDialog;


    public:
      // Scene pointer, HUD camera, json file
      DialogWindow( Scene*, Camera*, std::string );

      // Update all animations, sprite changes, etc
      void update( Uint32 );

      // Render the dialog using the stored camera
      void render();
  };


  class Dialog
  {
    typedef std::vector< Drawable* > ElementList;

    private:
      Scene* _owner;
      Camera* _HUDCamera;
      std::string _dialogFile;
      NamedVector<Dialog> _subDialogs;

      // List of the spawned elements
      // Dialog owns this memory
      ElementList _elements;

      // Separate the background sprite
      Drawable* _background;

      // All the elements that can be clicked on/have focus
      ClickableSet _buttons;

    public:
      // Scene pointer, camera pointer, json object containing this dialog
      Dialog( Scene*, Camera*, Json::Value& );

      // Update all animations, sprite changes, etc
      void update( Uint32 );

      // Render the dialog using the stored camera
      void render();

  };

}

#endif // __REGOLITH__DIALOG_H__
