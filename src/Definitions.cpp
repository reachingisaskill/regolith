
#include "Definitions.h"

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

}

