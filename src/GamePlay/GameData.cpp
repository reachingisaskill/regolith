
#include "Regolith/GamePlay/GameData.h"


namespace Regolith
{

  GameData::CallBack::CallBack( GameData* owner, unsigned int id ) :
    _owner( owner ),
    _eventID( id ),
    _counter( 0 )
  {
  }


  void GameData::CallBack::trigger()
  {
    ++_counter;
    _owner->handleEvent( _eventID );
  }


//////////////////////////////////////////////////

  GameData::GameData() :
    _events()
  {
  }


  GameData::~GameData()
  {
  }


  void GameData::addEvent( const char* name, unsigned int id )
  {
    _events[name] = id;
  }


  GameData::CallBack GameData::registerEvent( unsigned int id )
  {
    return GameData::CallBack( this, id );
  }
  
}

