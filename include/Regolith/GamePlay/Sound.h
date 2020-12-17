
#ifndef REGOLITH_GAME_PLAY_SOUND_H_
#define REGOLITH_GAME_PLAY_SOUND_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Assets/RawSound.h"


namespace Regolith
{

  class Sound
  {
    friend class AudioHandler;

    private:
      RawSound* _rawSound;
      int _channel;

    protected:

      // Return the sound to play
      virtual Mix_Chunk* getMIXSound() { return _rawSound->sound; }

      // Return the channel to play it on
      virtual int getChannel() { return _channel; }

      // Register the specific channel for this sound
      virtual void registerChannel( int c ) { _channel = c; }

    public:
      // Trivial construction
      Sound();

      // Virtual destructor
      virtual ~Sound() {}

      // To configure the sound
      void configure( Json::Value&, DataHandler& );
  };

}

#endif // REGOLITH_GAME_PLAY_SOUND_H_

