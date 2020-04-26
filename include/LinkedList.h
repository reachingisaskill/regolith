
#ifndef __REGOLITH__LINKED_LIST_H__
#define __REGOLITH__LINKED_LIST_H__

#include <mutex>


namespace Regolith
{
  typedef std::unique_lock< std::mutex > UniqueLock;
  typedef std::lock_guard< std::mutex > GuardLock;

  /*
   * A singly-linked list implementation where each element is mutexed.
   * this allows parallel reads AND writes AND deletions to happen.
   * In order to facilitate this it MUST be singly linked and iterators
   * can only travel forward!
   */

  template < class TYPE >
  class MutexedList
  {

    class Element
    {
      public :
        std::mutex mutex;

        TYPE data;
        Element* next;

        Element() : data(), next( nullptr ) {}

        Element( TYPE d, Element* n = nullptr ) : data( d ), next( n ) {}
    };

    private:
      std::mutex _mutexStart;
      std::mutex _mutexEnd;
      Element* _start;
      Element* _end;

    public:
      MutexedList();

      ~MutexedList();

      // Push onto back of list
      void push( TYPE );

      // Read/pop from front of list
      void pop();
      TYPE& front();
      const TYPE& front() const;

//      iterator begin();
//      iterator end();

      void erase( Element* );


      size_t size() const;

  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Template member function definitions

  template < class TYPE >
  MutexedList< TYPE >::MutexedList() :
    _start( nullptr ),
    _end( nullptr )
  {
  }


  template < class TYPE >
  MutexedList< TYPE >::~MutexedList()
  {
    // Lock the ends of the list - No one else can start iterating
    GuardLock startGuard( _mutexStart );
    GuardLock endGuard( _mutexEnd );

    MutexedList::Element* current = _start;
    // No more entry to list
    _start = nullptr;
    _end = nullptr;

    UniqueLock lock_current;
    while ( current != nullptr )
    {
      // Aquire the current element mutex
      lock_current = UniqueLock( current->mutex );

      if ( current->next == nullptr )
      {
        // Delete the current element
        delete current;
        current = nullptr;
      }
      else
      {
//        UniqueLock lock_next( current->next->mutex );
        lock_current = UniqueLock( current->next->mutex );

        // Find the next element
        Element* next = current->next;

        // Release the current lock - no else can obtain it anyway
//        lock_current = lock_next; // Move the current lock to next elemnt

        // Delete the current element
        delete current;
        current = next;
      }
    }
  }


  template < class TYPE >
  void MutexedList< TYPE >::push( TYPE val )
  {
    MutexedList::Element* element = new MutexedList::Element( val, _end );

    // Take ownership of the last element - must do this BEFORE the end pointer
    GuardLock finalGuard( _end->mutex );

    // Lock the end marker
    GuardLock endGuard( _mutexEnd );

    // Update the chain
    _end->next = element;
    _end = element;

    // If list was initially empty, update the start marker too
    if ( _start == nullptr )
    {
      GuardLock startGuard( _start );
      _start = element;
    }
  }


  template < class TYPE >
  void MutexedList< TYPE >::pop()
  {
    // Lock the start marker
    GuardLock startGuard( _mutexStart );

    // Check that we can own the first element
    UniqueLock firstLock( _start->mutex );
    firstLock.unlock();

    // Find the next element
    MutexedList::Element* next = _start->next;

    // Delete the start element and update the pointer.
    delete _start;
    _start = next;
  }
  

  template < class TYPE >
  TYPE& MutexedList< TYPE >::front()
  {
    // Lock the start marker
    GuardLock startGuard( _mutexStart );

    // Lock the first element
    GuardLock firstGuard( _start->mutex );

    // Return the stored value
    return _start->data;
  }


}

#endif // __REGOLITH__LINKED_LIST_H__

