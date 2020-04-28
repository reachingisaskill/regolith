
#ifndef __REGOLITH__BUTTON_H__
#define __REGOLITH__BUTTON_H__

#include "Definitions.h"

#include "Texture.h"


namespace Regolith
{

  class Button : public Drawable
  {
    typedef std::vector< Texture > TextureVector;

    public:
      enum State
      {
        STATE_NORMAL,
        STATE_FOCUSSED,
        STATE_ACTIVATED,
        STATE_INACTIVE
      };

    private :
      TextureVector _textures;

      Texture* _normal;
      Texture* _focussed;

      State _state;


    public :
      // Create in new button with the default texture
      Button( Texture );

      // Destroy and clear the texture set
      virtual ~Button();

      // Set the texture for a specific state
      void setTexture( Texture, State );

      // Sets the current state to focussed if button is active
      void giveFocus();

      // Sets the current state to normal if button is active
      void takeFocus();

      // Returns false if the button is inactive
      bool tryClick() const; 

      // Allows the button to be clickable
      void activate();

      // Deactivates the button
      void deactivate();
  };

}

#endif // __REGOLITH__BUTTON_H__

