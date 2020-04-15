
#ifndef __REGOLITH__MANAGER_H__
#define __REGOLITH__MANAGER_H__

#include "Singleton.h"

namespace Regolith
{

  class Manager : protected Singleton< Manager >
  {
    friend class Singleton< Manager >; // Required to enable the singleton pattern

    private:

    public:

  };

}

#endif // __REGOLITH__MANAGER_H__

