
#ifndef REGOLITH_UTILITIES_WRAPPER_MAP_H_
#define REGOLITH_UTILITIES_WRAPPER_MAP_H_

#include <map>
#include <string>

#include "logtastic.h"


namespace Regolith
{

  template< DATA >
  class Wrapper
  {
    private:
      DATA* data;

    public:
      Wrapper( DATA* d = nullptr ) : data( d ) {}

      DATA& operator->() { return *data; }
      const DATA& operator->() const { return *data; }

      DATA& operator*() { return *data; }
      const DATA& operator*() const { return *data; }

      template < OTHER >
      operator Wrapper<OTHER>() const { return Wrapper( dynamic_cast<OTHER>( data ) ); }

      operator bool() const { return data != nullptr; }
  };


  template< DATA >
  class WrapperMap
  {
    typedef typename Wrapper<DATA> ObjectWrapper;
    typedef typename std::map< std::string, DATA > MapType;

    public:
      typedef MapType::iterator iterator;
      typedef MapType::const_iterator const_iterator;

    private:
      std::string _name;

      MapType _dataMap;

    public:
      WrapperMap( const char* );

      ~WrapperMap();

      ObjectWrapper request( std::string );

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
  WrapperMap<DATA>::WrapperMap( const char* name ) :
    _name( name ),
    _dataMap()
  {
  }


  template < DATA >
  WrapperMap<DATA>::~WrapperMap()
  {
    INFO_STREAM << "Delete Wrapper Map: " << _name;
    _dataMap.clear();
  }


  template < DATA >
  ObjectWrapper WrapperMap<DATA>::request( std::string name )
  {
    MapType::iterator found = _dataMap.find( name );
    if ( found == _dataMap.end() )
    {
      found = _dataMap.insert( std::make_pair( name, DATA() ) ).first;
    }

    return ObjectWrapper( &(*found) );
  }


  template < DATA >
  DATA& WrapperMap::set( std::string name, DATA& obj )
  {
    DATA& datum = _dataMap[name];
    datum = obj;
    return datum;
  }


  template < DATA >
  DATA& WrapperMap::get( std::string name )
  {
    MapType::iterator found = _dataMap.find( name );
    if ( found == _dataMap.end() )
    {
      found = _dataMap.insert( std::make_pair( name, DATA() ) ).first;
    }

    return *found;
  }


  template < DATA >
  const DATA& WrapperMap::get( std::string name ) const
  {
    MapType::const_iterator found = _dataMap.find( name );
    if ( found == _dataMap.end() )
    {
      Exception ex( "WrapperMap::get()", "Object not found in map." );
      ex.addDetail( "Map Name", _name );
      ex.addDetail( "Object Name", name );
      throw ex;
    }

    return *found;
  }


  template < DATA >
  bool WrapperMap::exists( std::string name ) const
  {
    MapType::const_iterator found = _dataMap.find( name );
    if ( found == _dataMap.end() )
      return false;
    else
      return true;
  }

}

#endif // REGOLITH_UTILITIES_WRAPPER_MAP_H_

