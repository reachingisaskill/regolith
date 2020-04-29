
#ifndef __REGOLITH__DRAWABLE_H__
#define __REGOLITH__DRAWABLE_H__

#include "Definitions.h"
#include "Vector.h"
#include "Controllable.h"


namespace Regolith
{

  class Camera;
  class Collision;
  class InputHandler;

  /*
   * Defines the interface for all objects that can be drawn by the rendering functions.
   */
  class Drawable : public Controllable
  {

    private:
      SDL_Renderer* _theRenderer;
      Vector _position;
      Vector _velocity;
      Vector _force;
      Vector _inputForce;
      float _rotation;
      int _team;
      int _width;
      int _height;
      float _mass;
      float _inverseMass;
      bool _collisionActive;

    protected:
      Drawable( SDL_Renderer*, Vector, float );

      // Performs the movement integration step
      void step( float );

    public:
      Drawable();

      Drawable( int, int, float r = 0.0, float m = 0.0 );

      Drawable( const Drawable& ) = default;
      Drawable( Drawable&& ) = default;
      Drawable& operator=( const Drawable& ) = default;
      Drawable& operator=( Drawable&& ) = default;

      virtual ~Drawable();

      // Specify the properties of the object. (Moving, animated, collision, etc)
      virtual bool hasCollision() const = 0;
      virtual bool hasInput() const = 0;
      virtual bool hasAnimation() const = 0;


      // Returns the number of collision objects for the class.
      // The argument is a pointer which will be updated to point at the first object
      virtual unsigned int getCollision( Collision*& ) = 0;

      // Returns true if the object currently has active collision
      virtual bool collisionActive() const { return _collisionActive; }
      // Set whether thte collision objects are active
      virtual void setCollisionActive( bool active ) { _collisionActive = active; }


      // Perform the steps to call SDL_RenderCopy, etc
      virtual void render( Camera* ) = 0;

      // Update the objects behaviour based on the provided timestep
      virtual void update( Uint32 t ) { this->step( t ); }


      // Create a new copy and return the pointer to it. Transfers ownership of the memory
      virtual Drawable* clone() const = 0;
      // Same as above but specify the start position
      virtual Drawable* cloneAt( Vector ) const;


      // Get and Set the Renderer object
      void setRenderer( SDL_Renderer* rend ) { _theRenderer = rend; }
      SDL_Renderer* getRenderer() const { return _theRenderer; }


      // Position
      Vector getPosition() const { return _position; }
      void setPosition( Vector v ) { _position = v; }
      Vector& position() { return _position; }

      // Velocity
      Vector getVelocity() const { return _velocity; }
      void setVelocity( Vector v ) { _velocity = v; }
      void addVelocity( Vector v ) { _velocity += v; }
      Vector& velocity() { return _velocity; }

      // Force
      void applyForce( Vector f ) { _force += f; }
      void setInputForce( Vector f ) { _inputForce = f; }
      Vector& inputForce() { return _inputForce; }

      // Rotation
      float getRotation() const { return _rotation; }
      void setRotation( float f ) { _rotation = f; }
      float& rotation() { return _rotation; }

      // Incremental movement
      void move( Vector v ) { _position += v; }
      void rotate( float f ) { _rotation += f; }

      // Teams
      void setTeam( int t ) { _team = t; }
      int getTeam() const { return _team; }

      // Dimensions
      int getWidth() const { return _width; }
      void setWidth( int w ) { _width = w; }
      int getHeight() const { return _height; }
      void setHeight( int h ) { _height = h; }

      // Mass/inertia
      float getMass() const { return _mass; }
      float getInverseMass() const { return _inverseMass; }
      void setMass( float );
      bool isMovable() const { return _inverseMass >= epsilon; }


      // Prevent derived classes from using input manager. They do not persist outside the scene
      void registerEvents( InputManager* ) {}

      // Register actions with scenes input handler
      virtual void registerActions( InputHandler* ) = 0;

      // Interfaces for input
      virtual void eventAction( const InputEvent& ) {}
      virtual void booleanAction( const InputAction&, bool ) {}
      virtual void floatAction( const InputAction&, float ) {}
      virtual void vectorAction( const InputAction&, const Vector& ) {}
  };

}

#endif // __REGOLITH__DRAWABLE_H__

