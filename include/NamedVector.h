
#ifndef _REGOLITH_NAMED_VECTOR_H_
#define _REGOLITH_NAMED_VECTOR_H_

#include "Exception.h"

#include "logtastic.h"

#include <vector>
#include <map>
#include <string>

namespace Regolith
{

  /*
   * Stores a vector of objects and a map of text names, identifying each opject.
   * This allows the "name" of each object to be given a unique identifier allowing 
   * fast access to the actual object list.
   * This is extremely useful for parsing Json data into enumerated lists of heap-allocated objects
   * This class owns all the stored memory and will delete it on destruction
   */
  template < class DATA, bool OWNS_MEMORY >
  class NamedVector
  {
    typedef std::map< std::string, size_t > NameMap;
    typedef std::vector< DATA* > DataVector;

    private :
      std::string _storageType;
      NameMap _names;
      DataVector _data;

    public :
      NamedVector( const char* );

      ~NamedVector();


      void clear();

      size_t addObject( DATA*, std::string );

      size_t addName( std::string );

      size_t getID( std::string ) const;

      DATA* getByName( std::string );

      DATA* get( size_t );

      DATA* operator[]( size_t i ) { return _data[ i ]; }

      size_t mapSize() const { return _names.size(); }
      size_t vectorSize() const { return _data.size(); }
      size_t size() const { return _data.size(); }
  };

////////////////////////////////////////////////////////////////////////////////////////////////////

  template < class DATA, bool OWNSMEMORY >
  NamedVector<DATA, OWNSMEMORY>::NamedVector( const char* type ) :
    _storageType( type ),
    _names(),
    _data()
  {
  }


  template < class DATA, bool OWNSMEMORY >
  NamedVector<DATA, OWNSMEMORY>::~NamedVector()
  {
  }


  template < class DATA, bool OWNSMEMORY >
  void NamedVector<DATA, OWNSMEMORY>::clear()
  {
    _names.clear();
    _data.clear();
  }


  template < class DATA, bool OWNSMEMORY >
  size_t NamedVector<DATA, OWNSMEMORY>::addObject( DATA* object, std::string objName )
  {
    size_t position = this->addName( objName );

    if ( (_data[position] != nullptr) )
    {
      WARN_STREAM << "Named Vector: " << _storageType << ". Overwriting object with name : " << objName;
    }

    _data[ position ] = object;
    return position;
  }


  template < class DATA, bool OWNSMEMORY >
  size_t NamedVector<DATA, OWNSMEMORY>::addName( std::string objName )
  {
    NameMap::iterator found = _names.find( objName );

    if ( found != _names.end() )
    {
      INFO_STREAM << "Named Vector: " << _storageType << ". Name already exists in map. Nothing to do.";
      return found->second;
    }

    size_t position = _names.size();
    _names[ objName ] = position;

    _data.push_back( nullptr );
    return position;
  }


  template < class DATA, bool OWNSMEMORY >
  size_t NamedVector<DATA, OWNSMEMORY>::getID( std::string objName ) const
  {
    NameMap::const_iterator found = _names.find( objName );

    if ( found == _names.end() )
    {
      ERROR_STREAM << "Named Vector: " << _storageType << ". Could not find \"" << objName << "\" in name map.";
      Exception ex( "NamedVector::getID()", "Could not find name in map", true );
      ex.addDetail( "Name", objName );
      throw ex;
    }

    return found->second;
  }


  template < class DATA, bool OWNSMEMORY >
  DATA* NamedVector<DATA, OWNSMEMORY>::getByName( std::string name )
  {
    size_t id = this->getID( name );
    return _data[ id ];
  }


  template < class DATA, bool OWNSMEMORY >
  DATA* NamedVector<DATA, OWNSMEMORY>::get( size_t i )
  {
    if ( i >= _names.size() )
    {
      ERROR_STREAM << "Named Vector: " << _storageType << ". Could index, " << i << ", outside of map size, " << _names.size();
      Exception ex( "NamedVector::getID()", "Index outside of map size", true );
      ex.addDetail( "Index", i );
      ex.addDetail( "Size", _names.size() );
      throw ex;
    }

    return _data[i];
  }

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Partial Specialisation for Owned memory

  template < class DATA >
  class NamedVector< DATA, true >
  {
    typedef std::map< std::string, size_t > NameMap;
    typedef std::vector< DATA* > DataVector;

    private :
      std::string _storageType;
      NameMap _names;
      DataVector _data;

    public :
      NamedVector( const char* );

      ~NamedVector();


      void clear();

      size_t addObject( DATA*, std::string );

      size_t addName( std::string );

      size_t getID( std::string ) const;

      DATA* getByName( std::string );

      DATA* get( size_t );

      DATA* operator[]( size_t i ) { return _data[ i ]; }

      size_t mapSize() const { return _names.size(); }
      size_t vectorSize() const { return _data.size(); }
      size_t size() const { return _data.size(); }
  };


  template < class DATA >
  NamedVector<DATA, true>::NamedVector( const char* type ) :
    _storageType( type ),
    _names(),
    _data()
  {
  }


  template < class DATA >
  NamedVector<DATA, true>::~NamedVector()
  {
  }


  template < class DATA >
  void NamedVector<DATA, true>::clear()
  {
    INFO_STREAM << "Destroying Named Vector of: " << _storageType;

    // Clear the name maps
    _names.clear();

    // Delete all the pointers
    for ( size_t i = 0; i < _data.size(); ++i )
    {
      delete _data[i];
      _data[i] = nullptr;
    }
    _data.clear();
  }


  template < class DATA >
  size_t NamedVector<DATA, true>::addObject( DATA* object, std::string objName )
  {
    size_t position = this->addName( objName );

    if ( (_data[position] != nullptr) )
    {
      WARN_STREAM << "Named Vector: " << _storageType << ". Deleting and overwriting object with name : " << objName;
      delete _data[ position ];
    }

    _data[ position ] = object;
    return position;
  }


  template < class DATA >
  size_t NamedVector<DATA, true>::addName( std::string objName )
  {
    NameMap::iterator found = _names.find( objName );

    if ( found != _names.end() )
    {
      INFO_STREAM << "Named Vector: " << _storageType << ". Name already exists in map. Nothing to do.";
      return found->second;
    }

    size_t position = _names.size();
    _names[ objName ] = position;

    _data.push_back( nullptr );
    return position;
  }


  template < class DATA >
  size_t NamedVector<DATA, true>::getID( std::string objName ) const
  {
    NameMap::const_iterator found = _names.find( objName );

    if ( found == _names.end() )
    {
      ERROR_STREAM << "Named Vector: " << _storageType << ". Could not find \"" << objName << "\" in name map.";
      Exception ex( "NamedVector::getID()", "Could not find name in map", true );
      ex.addDetail( "Name", objName );
      throw ex;
    }

    return found->second;
  }


  template < class DATA >
  DATA* NamedVector<DATA, true>::getByName( std::string name )
  {
    size_t id = this->getID( name );
    return _data[ id ];
  }


  template < class DATA >
  DATA* NamedVector<DATA, true>::get( size_t i )
  {
    if ( i >= _names.size() )
    {
      ERROR_STREAM << "Named Vector: " << _storageType << ". Could index, " << i << ", outside of map size, " << _names.size();
      Exception ex( "NamedVector::getID()", "Index outside of map size", true );
      ex.addDetail( "Index", i );
      ex.addDetail( "Size", _names.size() );
      throw ex;
    }

    return _data[i];
  }


}

#endif // _REGOLITH_NAMED_VECTOR_H_

