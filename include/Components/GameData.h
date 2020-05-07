
#ifndef REGOLITH_GAME_DATA_H_
#define REGOLITH_GAME_DATA_H_

#include "Managers/Definitions.h"

#include <functional>


namespace Regolith
{

  /*
   * Base class to define the interface for storing game data that is affected by events
   * during gameplay via call-back functions
   */
  class GameData
  {
    typedef std::map< std::string, unsigned int > EventMap;

    public :
      class CallBack
      {
        private:
          GameData* _owner;
          unsigned int _eventID;
          unsigned int _counter;

        public:
          CallBack( GameData*, unsigned int );

          void trigger();
      };

    private:
      EventMap _events;

    protected:
      void addEvent( const char*, unsigned int );

    public:
      GameData();

      virtual ~GameData();

      virtual void handleEvent( unsigned int ) = 0;

      CallBack registerEvent( unsigned int );
  };

}

#endif // REGOLITH_GAME_DATA_H_

