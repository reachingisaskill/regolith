
#ifndef __REGOLITH__BUTTON_H__
#define __REGOLITH__BUTTON_H__

#include "Definitions.h"

#include "Texture.h"
#include "Collision.h"


namespace Regolith
{

  class Button : public Drawable
  {
    public:
      enum State
      {
        STATE_NORMAL,
        STATE_FOCUSSED,
        STATE_ACTIVATED,
        STATE_INACTIVE,
        STATE_TOTAL
      };

    private :
      Texture _textures[STATE_TOTAL];
      Collision* _collision;

      SDL_Rect _destination;

      State _state;

      std::string _actionName;
      ContextEvent _optionNumber;


    public :
      // Create in new button with the default texture
      Button( Texture, Collision* );

      // Copy the button and clone the collision
      Button( const Button& );

      // Destroy and clear the texture set
      virtual ~Button();

      // Set the texture for a specific state
      void setTexture( Texture, State );

      // Returns the current state
      State getState() const { return _state; }


      // Return the object properties
      virtual bool hasCollision() const { return _collision != nullptr; }
      virtual bool hasInput() const { return false; }
      virtual bool hasAnimation() const;
      virtual bool hasInteraction() const { return true; }


      // For derived classes to update every frame
      virtual void update( Uint32 );

      // Render with the current renderer object
      virtual void render( Camera* );

      // Handle events
      virtual void registerEvents( InputManager* ) {}

      // Handle events
      virtual void registerActions( InputHandler* ) {}

      // Returns the collision object for the class;
      virtual unsigned int getCollision( Collision*& );

      // Return a new clone of the current object. Transfers ownership of memory
      virtual Drawable* clone() const;


      // Set the action name
      void setActionName( std::string name ) { _actionName = name; }

      // Set the action name
      std::string getActionName() { return _actionName; }


      // Set the enumerated option number
      void setOption( ContextEvent e ) { _optionNumber = e; }

      // Get the enumerated option number
      ContextEvent getOption() { return _optionNumber; }
      

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

