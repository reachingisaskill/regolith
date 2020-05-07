
#ifndef REGOLITH_NOISY_H_
#define REGOLITH_NOISY_H_

#include "Managers/Definitions.h"


namespace Regolith
{
  // Forward declarations
  class AudioHandler;

  /*
   * Defines the intferface for classes that can trigger sounds effects to be played.
   */
  class Noisy
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

      // Interface used to trigger the above function. Ensures the handler pointer is recorded
      void registerSounds( AudioHandler* h ) { _handler = h; this->registerSoundEffects( _handler ); }

      // Returns the pointer to the owning AudioHandler
      AudioHandler* audioHandler() { return _handler; }
  };

}

#endif // REGOLITH_NOISY_H_

