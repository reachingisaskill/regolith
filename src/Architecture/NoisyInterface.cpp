
#include "Regolith/Architecture/NoisyInterface.h"

#include "Regolith/Managers/AudioHandler.h"


namespace Regolith
{

  NoisyInterface::NoisyInterface() :
    _handler( nullptr )
  {
  }


  void NoisyInterface::registerSounds( AudioHandler* h )
  {
    if ( h == nullptr )
    {
      Exception ex( "NoisyInterface::registerSounds()", "Could not register sounds. AudioHandler is a nullptr." );
      throw ex;
    }
    else
    {
      _handler = h;
      this->registerSoundEffects( _handler );
    }
  }


  void NoisyInterface::playSound( Sound& sound ) const
  {
    _handler->playSound( sound );
  }

}

