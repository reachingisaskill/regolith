
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
      bool _silence;

    public:
      // Trivial construction
      Music();

      // Virtual destructor
      virtual ~Music() {}

      // To configure the sound
      void configure( Json::Value&, DataHandler& );


      // Flag to indicate no music
      virtual bool isSilent() const { return _silence; }


      // Return the sound to play
      virtual Mix_Music* getMIXMusic() { return _rawMusic->music; }

  };

}

#endif // REGOLITH_AUDIO_MUSIC_H_

