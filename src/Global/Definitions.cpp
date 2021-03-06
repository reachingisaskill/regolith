
#include "Regolith/Global/SDL.h"
#include "Regolith/Global/Definitions.h"

#include <string>


namespace Regolith
{

  InputAction getActionID( std::string name )
  {
    for ( unsigned int i = 0; i < INPUT_ACTION_TOTAL; ++i )
    {
      if ( name == InputActionStrings[i] )
        return (InputAction) i;
    }

    return INPUT_ACTION_NULL;
  }

  SDL_Scancode getScancodeID( std::string name )
  {
    int value = 0;
    std::map< std::string, int >::const_iterator found = ScancodeStrings.find( name );

    if ( found == ScancodeStrings.end() )
      value = 0;
    else
      value = found->second;

    return (SDL_Scancode)value;
  }

  RegolithEvent getRegolithEventID( std::string name )
  {
    for ( unsigned int i = 0; i < REGOLITH_EVENT_TOTAL; ++i )
    {
      if ( name == RegolithEventStrings[i] )
        return (RegolithEvent) i;
    }

    return REGOLITH_EVENT_NULL;
  }


  MouseButton getMouseButtonID( std::string name )
  {
    for ( unsigned int i = 0; i < MOUSE_BUTTON_TOTAL; ++i )
    {
      if ( name == MouseButtonStrings[i] )
        return (MouseButton) i;
    }

    return MOUSE_BUTTON_NULL;
  }

  SDL_GameControllerButton getButtonID( std::string name )
  {
    for ( unsigned int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; ++i )
    {
      if ( name == ControllerButtonStrings[i] )
        return (SDL_GameControllerButton) i;
    }

    return SDL_CONTROLLER_BUTTON_INVALID;
  }

  SDL_GameControllerAxis getAxisID( std::string name )
  {
    for ( unsigned int i = 0; i < SDL_CONTROLLER_AXIS_MAX; ++i )
    {
      if ( name == ControllerAxisStrings[i] )
        return (SDL_GameControllerAxis) i;
    }

    return SDL_CONTROLLER_AXIS_INVALID;
  }


}

