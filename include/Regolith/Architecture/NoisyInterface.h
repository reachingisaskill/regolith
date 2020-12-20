
#ifndef REGOLITH_ARCHITECTURE_NOISY_INTERFACE_H_
#define REGOLITH_ARCHITECTURE_NOISY_INTERFACE_H_


namespace Regolith
{
  // Forward declarations
  class AudioHandler;
  class Sound;


  class NoisyInterface
  {
    private:
      // Store an audio handler pointer to actually play the sounds
      AudioHandler* _handler;

    protected :
      // For derived classes to implement and register the requested sound effects with the audio handler
      virtual void registerSoundEffects( AudioHandler* ) = 0;

    public:

      //Constructor
      NoisyInterface();

      // Destructor
      virtual ~NoisyInterface() {}


      // Interface used to trigger the above function. Ensures the handler pointer is recorded
      void registerSounds( AudioHandler* h );


      // Play a registered sound
      void playSound( Sound& s ) const;

  };

}

#endif // REGOLITH_ARCHITECTURE_NOISY_INTERFACE_H_

