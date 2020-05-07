
#ifndef REGOLITH_TRIGGER_H_
#define REGOLITH_TRIGGER_H_

#include "Managers/Definitions.h"
#include "Architecture/Drawable.h"


namespace Regolith
{

  // Forward declaration
  class Collision;
  class InputHandler;

  /*
   * This class defines an object that cannot be drawn to the window.
   * Rather the collision attached to this object is used to trigger events
   * e.g. kill-box, scene loading and ending, enemy spawn, etc.
   */
  class Trigger : public Drawable
  {
    private :
      Collision** _collision;
      ContextEvent _eventType;
      unsigned int _size;
      unsigned int _current;

    public :
      Trigger( unsigned int, ContextEvent );

      Trigger( const Trigger& );

      ~Trigger();

      // Set the properties
      virtual bool hasCollision() const { return true; }
      virtual bool hasInput() const { return false; }
      virtual bool hasAnimation() const { return false; }

      virtual ContextEvent getTriggerType()  { return _eventType; }


      // Returns the number of collision objects for the class.
      // The argument is a pointer which will be updated to point at the first object
      virtual unsigned int getCollision( Collision*& );

      // Function to add a collision object to the class. 
      // How the collision objects are stored is up to the derived class
      virtual void addCollision( Collision* );


      // Perform the steps to call SDL_RenderCopy, etc
      virtual void render( Camera* ) {}

      // Update the objects behaviour based on the provided timestep
      virtual void update( Uint32 ) {}


      // Create a new copy and return the pointer to it. Transfers ownership of the memory
      virtual Drawable* clone() const;


      void registerActions( InputHandler* ) {}

      // Interfaces for input
      virtual void eventAction( const RegolithEvent& ) {}
      virtual void booleanAction( const InputAction&, bool ) {}
      virtual void floatAction( const InputAction&, float ) {}
      virtual void vectorAction( const InputAction&, const Vector& ) {}
  };

}

#endif // REGOLITH_TRIGGER_H_

