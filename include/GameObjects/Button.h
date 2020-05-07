
#ifndef REGOLITH_BUTTON_H_
#define REGOLITH_BUTTON_H_

#include "Managers/Definitions.h"
#include "Components/Texture.h"
#include "Components/Collision.h"


namespace Regolith
{
  class Signal;

  class Button : public Drawable
  {
    public:
      enum State
      {
        STATE_NORMAL,
        STATE_FOCUSSED,
        STATE_DOWN,
        STATE_INACTIVE,
        STATE_TOTAL
      };

    private :
      Texture _textures[STATE_TOTAL];
      Collision* _collision;

      SDL_Rect _destination;

      State _state;

      std::vector< Signal* > _actions;


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


      // Add an action to the button
      void addAction( Signal* s ) { _actions.push_back( s ); }
      

      // Sets the current state to focussed if button is active
      void giveFocus();

      // Sets the current state to normal if button is active
      void takeFocus();

      // Returns false if the button is inactive
      void down(); 

      // Returns false if the button is inactive
      void up(); 

      // Allows the button to be clickable
      void activate();

      // Deactivates the button
      void deactivate();

      // Trigger the actions
      void click();
  };

}

#endif // REGOLITH_BUTTON_H_

