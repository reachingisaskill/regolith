
#include "Regolith/Architecture/Noisy.h"

namespace Regolith
{

  Noisy::Noisy() :
    _handler( nullptr )
  {
  }


  void Noisy::registerSounds( AudioHandler* h )
  {
    if ( h == nullptr )
    {
      Exception ex( "Noisy::registerSounds()", "Could not register sounds. AudioHandler is a nullptr." );
      throw ex;
    }
    else
    {
      _handler = h;
      this->registerSoundEffects( _handler );
    }
  }

}

