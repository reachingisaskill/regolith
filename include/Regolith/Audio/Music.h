
#ifndef REGOLITH_AUDIO_MUSIC_H_
#define REGOLITH_AUDIO_MUSIC_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Assets/RawMusic.h"


namespace Regolith
{

  class Music
  {
    friend class AudioHandler;

    private:
      RawMusic* _rawMusic;
      unsigned int _playCount;

    public:
      // Trivial construction
      Music();

      // Virtual destructor
      virtual ~Music() {}

      // To configure the sound
      void configure( Json::Value&, DataHandler& );


      // Return the sound to play
      virtual Mix_Music* getMIXMusic() { return _rawMusic->music; }

      // Return the number of times to play the music (zero = infinite loop)
      virtual unsigned int getPlayCount() { return _playCount; }

  };

}

#endif // REGOLITH_AUDIO_MUSIC_H_

