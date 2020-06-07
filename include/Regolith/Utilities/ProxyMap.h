
#ifndef REGOLITH_UTILITIES_PROXY_MAP_H_
#define REGOLITH_UTILITIES_PROXY_MAP_H_

#include <map>
#include <string>

#include "logtastic.h"


namespace Regolith
{

  template< DATA >
  class Proxy
  {
    private:
      DATA* data;

    public:
      Proxy( DATA* d = nullptr ) : data( d ) {}

      DATA& operator->() { return *data; }
      const DATA& operator->() const { return *data; }

      DATA& operator*() { return *data; }
      const DATA& operator*() const { return *data; }

      template < OTHER >
      operator Proxy<OTHER>() const { return Proxy( dynamic_cast<OTHER>( data ) ); }

      operator bool() const { return data != nullptr; }
  };


  template< DATA >
  class ProxyMap
  {
    typedef typename Proxy<DATA> ObjectProxy;
    typedef typename std::map< std::string, DATA > MapType;

    public:
      typedef MapType::iterator iterator;
      typedef MapType::const_iterator const_iterator;

    private:
      std::string _name;

      MapType _dataMap;

    public:
      ProxyMap( const char* );

      ~ProxyMap();

      ObjectProxy request( std::string );

      DATA& get( std::string );
      const DATA& get( std::string ) const;

      void set( std::string, DATA& );

      bool exists( std::string );

      size_t size() const { return _dataMap.size(); }


      iterator find( std::string name ) { return _dataMap.find( name ); }
      const_iterator find( std::string name ) const { return _dataMap.find( name ); }

      iterator begin() { return _dataMap.begin(); }
      const_iterator begin() const { return _dataMap.begin(); }

      iterator end() { return _dataMap.end(); }
      const_iterator end() const { return _dataMap.end(); }
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Template member function definitions

  template < DATA >
  ProxyMap<DATA>::ProxyMap( const char* name ) :
    _name( name ),
    _dataMap()
  {
  }


  template < DATA >
  ProxyMap<DATA>::~ProxyMap()
  {
    INFO_STREAM << "Delete Proxy Map: " << _name;
    _dataMap.clear();
  }


  template < DATA >
  ObjectProxy ProxyMap<DATA>::request( std::string name )
  {
    MapType::iterator found = _dataMap.find( name );
    if ( found == _dataMap.end() )
    {
      found = _dataMap.insert( std::make_pair( name, DATA() ) ).first;
    }

    return ObjectProxy( &(*found) );
  }


  template < DATA >
  DATA& ProxyMap::set( std::string name, DATA& obj )
  {
    DATA& datum = _dataMap[name];
    datum = obj;
    return datum;
  }


  template < DATA >
  DATA& ProxyMap::get( std::string name )
  {
    MapType::iterator found = _dataMap.find( name );
    if ( found == _dataMap.end() )
    {
      found = _dataMap.insert( std::make_pair( name, DATA() ) ).first;
    }

    return *found;
  }


  template < DATA >
  const DATA& ProxyMap::get( std::string name ) const
  {
    MapType::const_iterator found = _dataMap.find( name );
    if ( found == _dataMap.end() )
    {
      Exception ex( "ProxyMap::get()", "Object not found in map." );
      ex.addDetail( "Map Name", _name );
      ex.addDetail( "Object Name", name );
      throw ex;
    }

    return *found;
  }


  template < DATA >
  bool ProxyMap::exists( std::string name ) const
  {
    MapType::const_iterator found = _dataMap.find( name );
    if ( found == _dataMap.end() )
      return false;
    else
      return true;
  }

}

#endif // REGOLITH_UTILITIES_PROXY_MAP_H_

