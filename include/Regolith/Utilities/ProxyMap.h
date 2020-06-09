
#ifndef REGOLITH_UTILITIES_PROXY_MAP_H_
#define REGOLITH_UTILITIES_PROXY_MAP_H_

#include <map>
#include <string>

#include "logtastic.h"


namespace Regolith
{

  template< class DATA >
  class Proxy
  {
    private:
      DATA* data;
      const std::string* name;

    public:
      Proxy() : data( nullptr ), name( nullptr ) {}
      Proxy( DATA* d, const std::string* n ) : data( d ), name( n ) {}

      DATA* getData() { return data; }

      const std::string& getName() const { return *name; }

      DATA& operator->() { return *data; }
      const DATA& operator->() const { return *data; }

      DATA& operator*() { return *data; }
      const DATA& operator*() const { return *data; }

      template < class OTHER >
      operator Proxy<OTHER>() const { return Proxy<OTHER>( dynamic_cast<OTHER*>( data ), name ); }

      template < class OTHER >
      operator Proxy<OTHER*>() const { return Proxy<OTHER*>( reinterpret_cast<OTHER**>( data ), name ); }

      operator bool() const { return data != nullptr; }
  };

////////////////////////////////////////////////////////////////////////////////////////////////////

  template< class DATA >
  class ProxyMap
  {
    typedef Proxy<DATA> ProxyType;
    typedef std::map< std::string, DATA > MapType;

    public:
      typedef typename MapType::iterator iterator;
      typedef typename MapType::const_iterator const_iterator;

    private:
      std::string _name;

      MapType _dataMap;

    public:
      ProxyMap( const char* );

      ~ProxyMap();

      ProxyType request( std::string );

      DATA& get( std::string );
      const DATA& get( std::string ) const;

      DATA& set( std::string, DATA& );

      DATA& create( std::string, DATA& );

      DATA& create( std::string );

      DATA& createIfMissing( std::string );

      bool exists( std::string ) const;

      size_t size() const { return _dataMap.size(); }

      void clear() { _dataMap.clear(); }


      iterator find( std::string name ) { return _dataMap.find( name ); }
      const_iterator find( std::string name ) const { return _dataMap.find( name ); }

      iterator begin() { return _dataMap.begin(); }
      const_iterator begin() const { return _dataMap.begin(); }

      iterator end() { return _dataMap.end(); }
      const_iterator end() const { return _dataMap.end(); }
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Template member function definitions

  template < class DATA >
  ProxyMap<DATA>::ProxyMap( const char* name ) :
    _name( name ),
    _dataMap()
  {
  }


  template < class DATA >
  ProxyMap<DATA>::~ProxyMap()
  {
    INFO_STREAM << "Delete Proxy Map: " << _name;
    _dataMap.clear();
  }


  template < class DATA >
  typename ProxyMap<DATA>::ProxyType ProxyMap<DATA>::request( std::string name )
  {
    typename MapType::iterator found = _dataMap.find( name );
    if ( found == _dataMap.end() )
    {
      found = _dataMap.insert( std::make_pair( name, DATA() ) ).first;
    }

    return ProxyType( &(found->second), &(found->first) );
  }


  template < class DATA >
  DATA& ProxyMap<DATA>::set( std::string name, DATA& obj )
  {
    DATA& datum = _dataMap[name];
    datum = obj;
    return datum;
  }


  template < class DATA >
  DATA& ProxyMap<DATA>::create( std::string name, DATA& data )
  {
    std::pair<typename MapType::iterator, bool> result = _dataMap.insert( std::make_pair( name, data ) );
    if ( ! result.second )
    {
      WARN_STREAM << "ProxyMap: " << _name << ". Object (" << name << ") already exists. Recreating.";
      _dataMap[name] = data;
    }

    return *(result.first);
  }


  template < class DATA >
  DATA& ProxyMap<DATA>::create( std::string name )
  {
    std::pair<typename MapType::iterator, bool> result = _dataMap.insert( std::make_pair( name, DATA() ) );
    if ( ! result.second )
    {
      WARN_STREAM << "ProxyMap: " << _name << ". Object (" << name << ") already exists. Recreating.";
      _dataMap[name] = DATA();
    }

    return result.first->second;
  }


  template < class DATA >
  DATA& ProxyMap<DATA>::createIfMissing( std::string name )
  {
    typename MapType::iterator found = _dataMap.find( name );
    if ( found == _dataMap.end() )
    {
      found = _dataMap.insert( std::make_pair( name, DATA() ) ).first;
    }

    return found->second;
  }


  template < class DATA >
  DATA& ProxyMap<DATA>::get( std::string name )
  {
    typename MapType::iterator found = _dataMap.find( name );
    if ( found == _dataMap.end() )
    {
      Exception ex( "ProxyMap::get()", "Object not found in map." );
      ex.addDetail( "Map Name", _name );
      ex.addDetail( "Object Name", name );
      throw ex;
    }

    return found->second;
  }


  template < class DATA >
  const DATA& ProxyMap<DATA>::get( std::string name ) const
  {
    typename MapType::const_iterator found = _dataMap.find( name );
    if ( found == _dataMap.end() )
    {
      Exception ex( "ProxyMap::get()", "Object not found in map." );
      ex.addDetail( "Map Name", _name );
      ex.addDetail( "Object Name", name );
      throw ex;
    }

    return found->second;
  }


  template < class DATA >
  bool ProxyMap<DATA>::exists( std::string name ) const
  {
    typename MapType::const_iterator found = _dataMap.find( name );
    if ( found == _dataMap.end() )
      return false;
    else
      return true;
  }

}

#endif // REGOLITH_UTILITIES_PROXY_MAP_H_

