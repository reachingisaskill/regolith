
#ifndef REGOLITH_ARCHITECTURE_NOISY_H_
#define REGOLITH_ARCHITECTURE_NOISY_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/GameObject.h"


namespace Regolith
{
  // Forward declarations
  class AudioHandler;

  namespace Architecture
  {

    /*
     * Defines the intferface for classes that can trigger sounds effects to be played.
     */
    class Noisy : virtual public GameObject
    {
      AudioHandler* _handler;

      protected :
        // For derived classes to implement and register the requested sound effects with the audio handler
        virtual void registerSoundEffects( AudioHandler* ) = 0;

      public:
        // Simple contructor
        Noisy() : _handler( nullptr ) {}

        // Make the destructor virtual
        virtual ~Noisy() {}


        // Tells the context that this class can be Noisy
        virtual bool hasAudio() const override { return true; }


        // Interface used to trigger the above function. Ensures the handler pointer is recorded
        void registerSounds( AudioHandler* h ) { _handler = h; this->registerSoundEffects( _handler ); }


        // Play a registered sound
        void playSound( unsigned int id ) const { _handler->triggerEffect( id ); }

    };

  }
}

#endif // REGOLITH_NOISY_H_

