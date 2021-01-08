
#ifndef REGOLITH_LINKS_LINK_AUDIO_MANAGER_H_
#define REGOLITH_LINKS_LINK_AUDIO_MANAGER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Links/Link.h"
#include "Regolith/Managers/AudioManager.h"


namespace Regolith
{

  // Prevent partial specialisations from passing
  template < class REQUESTER >
  class Link< AudioManager, REQUESTER >
  {
    struct FalseType : public std::false_type {};
    static_assert( FalseType::value, "A link is not permitted between the requested class and the audio manager" );
  };


////////////////////////////////////////////////////////////////////////////////
  // Test Access
  template <>
  class Link< AudioManager, TestType >
  {
    private:

      AudioManager& _manager;

    public:

      Link( AudioManager& m ) : _manager( m ) {}

      void queueTrack( Music* m ) { _manager.queueTrack( m ); }
      void playTrack( Music* m ) { _manager.playTrack( m ); }
      void clearQueue() { _manager.clearQueue(); }
      void stopTrack() { _manager.stopTrack(); }
      void load( Playlist* pl ) { _manager.load( pl ); }
      void pauseTrack() { _manager.pauseTrack(); }
      void resumeTrack() { _manager.resumeTrack(); }
      void stopRepeatTrack() { _manager.stopRepeatTrack(); }
      void nextTrack() { _manager.nextTrack(); }
      void nextRepeatTrack() { _manager.nextRepeatTrack(); }
  };


////////////////////////////////////////////////////////////////////////////////
  // Context Group Access
  class ContextGroup;

  template <>
  class Link< AudioManager, ContextGroup >
  {
    private:

      AudioManager& _manager;

    public:

      Link( AudioManager& m ) : _manager( m ) {}

      void load( Playlist* pl ) { _manager.load( pl ); }
      void nextTrack() { _manager.nextTrack(); }
      void clearQueue() { _manager.clearQueue(); }
      void stopTrack() { _manager.stopTrack(); }
  };

}

#endif // REGOLITH_LINKS_LINK_AUDIO_MANAGER_H_

