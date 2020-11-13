
#ifndef REGOLITH_ARCHITECTURE_NOISY_INTERFACE_H_
#define REGOLITH_ARCHITECTURE_NOISY_INTERFACE_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/GameObject.h"
#include "Regolith/Managers/AudioHandler.h"


namespace Regolith
{
  // Forward declarations
  class AudioHandler;

  /*
   * Defines the intferface for classes that can trigger sounds effects to be played.
   */
  class NoisyInterface : virtual public GameObject
  {
    private:
      AudioHandler* _handler;

    protected :
      // For derived classes to implement and register the requested sound effects with the audio handler
      virtual void registerSoundEffects( AudioHandler* ) = 0;

    public:
      // Simple contructor
      NoisyInterface();

      // Make the destructor virtual
      virtual ~NoisyInterface() {}


      // Interface used to trigger the above function. Ensures the handler pointer is recorded
      void registerSounds( AudioHandler* h );


      // Play a registered sound
      void playSound( RawSound* s ) const { _handler->playSound( s ); }

  };

}

#endif // REGOLITH_NOISY_INTERFACE_H_

