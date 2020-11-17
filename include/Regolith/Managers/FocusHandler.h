
#ifndef REGOLITH_MANAGERS_FOCUS_HANDLER_H_
#define REGOLITH_MANAGERS_FOCUS_HANDLER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/ButtonInterface.h"

#include <vector>
#include <map>


namespace Regolith
{

  /*
   * Class provides the logic for a context to handle the focus of buttons/objects.
   * It Handles the mouse clicks and movements to handle different clickable object state changes
   * In the future it will probably have a binary seach tree rather than a list, but we'll get there.
   */
  class FocusHandler
  {
    
    typedef std::vector< ButtonInterface* > ButtonVector;
    typedef std::map< ButtonInterface*, size_t > ButtonMap;

    private:
      ButtonVector _buttons;
      ButtonMap _numberLookup;
      size_t _currentFocus;

    public:
      FocusHandler();

      virtual ~FocusHandler();

      // Add an object to the handler
      void addObject( ButtonInterface* );

      // Pass focus to the next object
      void next();

      // Pass focus to the previous object
      void previous();

      // Forcibly request focus
      void requestFocus( ButtonInterface* );

  };

}

#endif // REGOLITH_MANAGERS_FOCUS_HANDLER_H_

