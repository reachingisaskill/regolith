
#ifndef REGOLITH_ARCHITECTURE_BUTTON_INTERFACE_H_
#define REGOLITH_ARCHITECTURE_BUTTON_INTERFACE_H_


namespace Regolith
{
  // Forward declare the focus handler
  class FocusHandler;

  /*
   * The class that defines an interface for anything that could act like a button.
   */
  class ButtonInterface
  {
    private:
      FocusHandler* _handler;

    public:
      ButtonInterface() : _handler( nullptr ) {}

      virtual ~ButtonInterface() {}


      // Register this object with the focus handler
      void registerFocus( FocusHandler* );


      // Focus is given to this object by the focus handler
      virtual void giveFocus() = 0;

      // Focus is taken from this object by the focus handler
      virtual void takeFocus() = 0;

      // Forcibly requests focus from the focus handler
      void requestFocus();
  };

}

#endif // REGOLITH_ARCHITECTURE_BUTTON_INTERFACE_H_

