
#include "Regolith/Test/JukeboxContext.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  JukeboxContext::JukeboxContext() :
    _playlist(),
    _status( nullptr ),
    _skipTimer1(),
    _skipTimer2(),
    _pauseTimer(),
    _resumeTimer(),
    _stopTimer(),
    _startTimer(),
    _stopRepeatTimer(),
    _closeTimer()
  {
  }


  JukeboxContext::~JukeboxContext()
  {
  }


  void JukeboxContext::configure( Json::Value& json_data, ContextGroup& cg )
  {
    // Configure the base class
    Context::configure( json_data, cg );

    Utilities::validateJson( json_data, "music", Utilities::JSON_TYPE_OBJECT );
    _playlist.configure( json_data["music"], cg.getDataHandler() );


    Utilities::validateJson( json_data, "status_string", Utilities::JSON_TYPE_STRING );
    PhysicalObject* status_phys = cg.getPhysicalObject( json_data["status_string"].asString() );
    _status = dynamic_cast<StatusString*>( status_phys );

    if ( _status == nullptr )
    {
      Exception ex( "JukeboxContext::configure()", "Requested status string object is not of type StatusString" );
      ex.addDetail( "Object Name", json_data["status_string"].asString() );
      throw ex;
    }

    _skipTimer1.configure(   2000, 1 );
    _skipTimer2.configure(   4000, 1 );
    _pauseTimer.configure(  20000, 1 );
    _resumeTimer.configure( 21000, 1 );
    _stopTimer.configure(   23000, 1 );
    _startTimer.configure(  24000, 1 );
    _stopRepeatTimer.configure(   30000, 1 );
    _closeTimer.configure(  45000, 1 );

    _status->setStatus( "Loading" );
  }


   void JukeboxContext::onStart()
   {
     _playlist.play();

     this->setClosed( false );
   }


  void JukeboxContext::updateContext( float time )
  {
    static AudioManager& manager = Manager::getInstance()->getAudioManager();

    if ( _skipTimer1.trigger( time ) )
    {
      _status->setStatus( "Skip" );
      manager.nextTrack();
    }

    if ( _skipTimer2.trigger( time ) )
    {
      _status->setStatus( "Skip Repeat" );
      manager.nextRepeatTrack();
    }

    if ( _pauseTimer.trigger( time ) )
    {
      _status->setStatus( "Paused" );
      manager.pauseTrack();
    }

    if ( _resumeTimer.trigger( time ) )
    {
      _status->setStatus( "Resumed" );
      manager.resumeTrack();
    }

    if ( _stopTimer.trigger( time ) )
    {
      _status->setStatus( "Stopped" );
      manager.stopTrack();
    }

    if ( _startTimer.trigger( time ) )
    {
      _status->setStatus( "Started" );
      manager.resumeTrack();
    }

    if ( _stopRepeatTimer.trigger( time ) )
    {
      _status->setStatus( "Stopping Repeat" );
      manager.stopRepeatTrack();
    }

    if ( _closeTimer.trigger( time ) )
    {
      _status->setStatus( "Adios!" );
      this->stopContext();
    }

  }

}

