
#include "Regolith/Utilities/CircularSemaphore.h"

#include "logtastic.h"


namespace Regolith
{
  namespace Utilities
  {

////////////////////////////////////////////////////////////////////////////////////////////////////
    // CircularSemaphore::ListItem Member function definitions

    CircularSemaphore::ListItem::ListItem( std::thread::id id, ListItem* n, AtomicThreadID& thread ) :
      _id( id ),
      _next( n ),
      _currentThread( thread )
    {
    }


    CircularSemaphore::ListItem::~ListItem()
    {
      // Hopefully the user isn't an idiot...
    }


    CircularSemaphore::ListItem* CircularSemaphore::ListItem::append( ListItem* li )
    {
      _next = li;
      return _next;
    }


    void CircularSemaphore::ListItem::aquire()
    {
      // Wait until it's our turn
      while ( _currentThread.load( std::memory_order_relaxed ) != _id );
    }


    void CircularSemaphore::ListItem::pass()
    {
      // Store the next thread id in the current thread variable
      _currentThread.store( _next->_id, std::memory_order_relaxed );
    }


////////////////////////////////////////////////////////////////////////////////////////////////////
    // CircularSemaphore::Handle Member function definitions

    CircularSemaphore::Handle::Handle( ListItem& li ) :
      _listItem( li ),
      _isLocked( false )
    {
    }


    CircularSemaphore::Handle::Handle( Handle&& h ) :
      _listItem( h._listItem ),
      _isLocked( std::move( h._isLocked ) )
    {
    }


    CircularSemaphore::Handle::~Handle()
    {
    }


    void CircularSemaphore::Handle::lock()
    {
      _listItem.aquire();
      _isLocked = true;
    }


    void CircularSemaphore::Handle::unlock()
    {
      _listItem.pass();
      _isLocked = false;
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
    // CircularSemaphore Member function definitions

    CircularSemaphore::CircularSemaphore( unsigned int N ) :
      _currentThread(),
      _listMutex(),
      _totalHandles( N ),
      _listEnd( nullptr ),
      _startCondition(),
      _currentNumberHandles( 0 )
    {
    }


    CircularSemaphore::~CircularSemaphore()
    {
      if ( _listEnd != nullptr )
      {
        ListItem* current = _listEnd;
        do
        {
          ListItem* next = current->next();
          delete current;
          current = next;
        }
        while ( current != _listEnd );
      }
    }


    CircularSemaphore::Handle CircularSemaphore::requestHandle( unsigned int handleNumber )
    {
      DEBUG_STREAM << "WAITING HANDLE: " << handleNumber;

      // Lock access to the list
      std::unique_lock< std::mutex > lock( _listMutex );

      // Wait until the number of handles equals the requested handleNumber
      _startCondition.wait( lock, [this, handleNumber]()->bool{ return ( this->_currentNumberHandles == handleNumber ); } );

      DEBUG_STREAM << "CONFIGURING HANDLE: " << handleNumber;


      // Unique thread identifier
      std::thread::id id = std::this_thread::get_id();

      // Create and append the new list item
      if ( _listEnd == nullptr )
      {
        ListItem* newItem = new ListItem( id, nullptr, _currentThread );
        _listEnd = newItem->append( newItem );
        _currentThread = id;
      }
      else
      {
        ListItem* newItem = new ListItem( id, _listEnd->next(), _currentThread );
        _listEnd = _listEnd->append( newItem );
      }

      // Create a handle for the item
      Handle handle( *_listEnd );

      // Increment the counter
      _currentNumberHandles += 1;

      // Notify the update
      _startCondition.notify_all();

      // If that's all, we notify the condition variable, otherwise we wait on it.
      _startCondition.wait( lock, [this]() -> bool { return (this->_currentNumberHandles == this->_totalHandles); } );
      lock.unlock();

      // Return the handle
      return handle; 
    }

  }
}

