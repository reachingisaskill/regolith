
#ifndef REGOLITH_GAMEPLAY_MUSIC_TRACK_H_
#define REGOLITH_GAMEPLAY_MUSIC_TRACK_H_

#include "Regolith/Architecture/GameObject.h"
#include "Regolith/GamePlay/Noises.h"


namespace Regolith
{

  class MusicTrack : public GameObject
  {
    private:
      RawMusic* _track;

    public:
      // Constructor
      MusicTrack();

      // Destructor
      ~MusicTrack();

      // Configure the music track
      void configure( Json::Value&, ContextGroup&, DataHandler& );

      // Return the track pointer
      RawMusic* getTrack() const { return _track; }
  };

}

#endif // REGOLITH_GAMEPLAY_MUSIC_TRACK_H_

