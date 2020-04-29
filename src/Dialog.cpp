
#include "Dialog.h"


namespace Regolith
{

  // Dialog Window

  DialogWindow::DialogWindow( Scene* scene, Camera* camera, std::string file ) :
    _filename( file ),
    _scene( scene ),
    _camera( camera ),
    _dialogTree( nullptr ),
    _currentDialog( nullptr )
  {
  }


  void DialogWindow::loadFromJson()
  {
  }


  void DialogWindow::update( Uint32 time )
  {
    _currentDialog->update( time );
  }


  void DialogWindow::render()
  {
    _currentDialog->render();
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Dialog Class

  Dialog::Dialog( Scene* scene, Camera* camera, Json::Value& json_data ) :
    _owner( scene ),
    _HUDCamera( camera ),
    _subDialogs("sub_dialogs"),
    _elements(),
    _background(),
    _buttons()
  {
  }


  void Dialog::update( Uint32 time )
  {
  }


  void Dialog::render()
  {
  }

}

