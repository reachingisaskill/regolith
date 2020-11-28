
#ifndef REGOLITH_UTILITIES_CONDITION_H_
#define REGOLITH_UTILITIES_CONDITION_H_

#include <mutex>
#include <condition_variable>


namespace Regolith
{
////////////////////////////////////////////////////////////////////////////////////////////////////
  // Condition structure for storing condition variables

  template < class DATA >
  struct Condition
  {
    mutable std::condition_variable variable;
    mutable std::mutex mutex;
    DATA data;

    Condition() {}
    explicit Condition( DATA d ) : data( d ) {}
  };

}

#endif // REGOLITH_UTILITIES_CONDITION_H_

