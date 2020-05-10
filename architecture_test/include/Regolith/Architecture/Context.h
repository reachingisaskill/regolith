
#ifndef REGOLITH_ARCHITECTURE_CONTEXT_H_
#define REGOLITH_ARCHITECTURE_CONTEXT_H_

#include "Global/Global.h"
#include "Architecture/ControllableInterface.h"
#include "Architecture/Component.h"
#include "Architecture/Animated.h"
#include "Architecture/PhysicalObject.h"
#include "Architecture/Layer.h"
#include "Managers/InputManager.h"
#include "Managers/AudioManager.h"

#include <list>


namespace Regolith
{

  namespace Interfaces
  {

    /*
     * Defines a context interface.
     * This is any object that can hold the focus of the user input.
     * This interface class exposes functions to force the handling of specific
     * events within the current context.
     * Examples of derived classes include all scenes and dialog boxes.
     * The engine class has no knowledge of contexts! They are only found at scene-level and below.
     * The standard rendering interface is still to use the current scene, and let the scene
     * call the rendering function for the current context.
     */
    class Context : public ControllableInterface, public Component
    {
      // Contexts should not be copyable
      Context( const Context& ) = delete;
      Context& operator=( const Context& ) = delete;

      // Useful typedefs
      typedef std::list< Animated* > AnimatedList;
      typedef std::list< PhysicalObject* > PhysicalObjectList

      private:
        // Context-local audio and input handlers
        InputHandler _theInput;
        AudioHandler _theAudio;

        // Location and size with respect to the window
        Vector _position;
        float _width;
        float _height;

        // Flag to indicate whether the context is advancing
        bool _paused;

        // Flag to indicate whether the context should be paused when it loses focus
        bool _pauseable;

        // Named vector containing all the contexts that can be loaded from this one
        NamedVector< Context, true > _childContexts;

        // Named vector of all the layers owned by the current context
        NamedVector< Layer, true > _layers;

        // Owned memory of all the objects that are spawned within this context
        PhysicalObjectList _spawnedObjects;

        // Cache of all objects that are animated
        AnimatedList _animatedObjects;


      protected:
        // Set the pauseable flag
        void setPauseable( bool p ) { _pauseable = p; }

        // Hooks for context state changes
        virtual void onStart() = 0;
        virtual void onStop() = 0;
        virtual void onPause() = 0;
        virtual void onResume() = 0;


        // Return the input handler for this context
        InputHandler* inputHandler() { return &_theInput; }

        // Return the audio handler for this context
        AudioHandler* audioHandler() { return &_theAudio; }


      public:
        // Basic Constructor: position, width, height
        Context( Vector, float, float );

        // Destroctor
        virtual ~Context();


        // Return the state of the pause flag
        bool isPaused() const { return _paused; }

        // Public interface to context states
        void startContext() { _theAudio.play(); this->onStart(); }
        void stopContext() { _theAudio.stop(); this->onStop(); }
        void pauseContext() { if ( ! _paused && _pauseable ) { _paused = true; _theAudio.pause(); this->onPause(); } }
        void resumeContext() { if ( _paused ) { _paused = false; _theAudio.play(); this->onResume(); } }


        // Returns true to pause and ignore the parent context. This prevents it from being rendered, updated, stepped, anything!
        // Use this for when child contexts/menus/etc required the full screen and comply obscure the parent
        virtual bool overridesPreviousContext() const = 0;



        // Update the animated objects
        void update( Uint32 );


        // Update the moveable objects
        void step( Uint32 );


        // Render all the objects
        void render();


        // Render all the objects
        void resolveCollisions();



        // Opens a new child context with the specified id
        void openChildContext( unsigned int );
        // Return the ID of a child context so that it may be opened later
        unsigned int getContextID( std::string name ) const { return _childContexts.getID( name ); }

        // Return the ID of a layer so that it may be referenced when spawining objects
        unsigned int getLayerID( std::string name ) const { return _layers.getID( name ); }
        


        // Spawn an object with a given ID and place it in the provided layer, at the provided position
        void spawn( unsigned int, unsigned int, const Vector& );

        // Insert a spawned object (memory owned by another, probably derived class) and cache it in the provided layer
        void addSpawnedObject( PhysicalObject*, unsigned int, const Vector& );

        // Cache an object, this will NOT add objects to any layers, but will ensure any update functions are called.
        virtual void cacheObject( GameObject* );

    };

  }
}

#endif // REGOLITH_ARCHITECTURE_CONTEXT_H_
