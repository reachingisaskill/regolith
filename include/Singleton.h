
#ifndef __REGOLITH__SINGLETON_H__
#define __REGOLITH__SINGLETON_H__


template < class T >
class Singleton
{
    Singleton( const Singleton< T >& ) = delete; // Non-copy-constructable
    Singleton( const Singleton< T >&& ) = delete; // Non-move-constructable
    Singleton< T >& operator=( const Singleton< T >& ) = delete; // Non-copy-assignable
    Singleton< T >& operator=( const Singleton< T >&& ) = delete; // Non-move-assignable

  private:
    static T* _theInstance;

  protected:
    Singleton() { }
    virtual ~Singleton() { }

  public:
    static T* createInstance() { if ( _theInstance ) delete _theInstance; _theInstance = new T(); return _theInstance; }
    static T* getInstance() { return _theInstance; }
    static void killInstance() { if ( _theInstance ) delete _theInstance; _theInstance = nullptr; }

    virtual operator bool () const { return ( _theInstance != nullptr ); }
    bool isAlive() const { return ( _theInstance != nullptr ); }
};

template < class T >
T* Singleton< T >::_theInstance = nullptr;


#endif // __REGOLITH__SINGLETON_H__
