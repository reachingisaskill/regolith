
#ifndef REGOLITH_UTILITIES_NAMED_VECTOR_H_
#define REGOLITH_UTILITIES_NAMED_VECTOR_H_

#include "Regolith/Global/Global.h"

#include "Regolith/Utilities/Exception.h"

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
   * This class is configured to own the stored memory using a boolean template type.
   */
  template < class DATA, bool OWNS_MEMORY >
  class NamedVector
  {
    typedef std::map< std::string, size_t > NameMap;
    typedef std::vector< DATA* > DataVector;

    public:
      typedef typename std::vector< DATA* >::iterator iterator;
      typedef typename std::vector< DATA* >::const_iterator const_iterator;

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

      std::string getName( size_t ) const;

      DATA* get( std::string );

      DATA* get( size_t );

      void set( size_t, DATA* );

      void set( std::string, DATA* );

      DATA* operator[]( size_t i ) { return _data[ i ]; }
      
      const DATA* operator[]( size_t i ) const { return _data[ i ]; }

      bool exists( std::string ) const;

      bool exists( size_t ) const;


      iterator begin() { return _data.begin(); }
      const_iterator begin() const { return _data.begin(); }

      iterator end() { return _data.end(); }
      const_iterator end() const { return _data.end(); }

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
      Exception ex( "NamedVector::getID()", "Could not find name in map", true );
      ex.addDetail( "Storage Type", _storageType );
      ex.addDetail( "Name", objName );
      throw ex;
    }

    return found->second;
  }


  template < class DATA, bool OWNSMEMORY>
  std::string NamedVector<DATA, OWNSMEMORY>::getName( size_t id ) const
  {
    for ( NameMap::const_iterator it = _names.begin(); it != _names.end(); ++it )
    {
      if ( it->second == id )
        return it->first;
    }

    Exception ex( "NamedVector::getName()", "Could not find id in map" );
    ex.addDetail( "Storage Type", _storageType );
    ex.addDetail( "ID", id );
    throw ex;
  }


  template < class DATA, bool OWNSMEMORY >
  DATA* NamedVector<DATA, OWNSMEMORY>::get( std::string name )
  {
    size_t id = this->getID( name );
    return _data[ id ];
  }


  template < class DATA, bool OWNSMEMORY >
  DATA* NamedVector<DATA, OWNSMEMORY>::get( size_t i )
  {
    if ( i >= _names.size() )
    {
      Exception ex( "NamedVector::get()", "Index outside of map size", true );
      ex.addDetail( "Storage Type", _storageType );
      ex.addDetail( "Index", i );
      ex.addDetail( "Size", _names.size() );
      throw ex;
    }

    return _data[i];
  }


  template < class DATA, bool OWNSMEMORY >
  void NamedVector<DATA, OWNSMEMORY>::set( size_t i, DATA* data )
  {
    if ( i >= _names.size() )
    {
      Exception ex( "NamedVector::set()", "Index outside of map size", true );
      ex.addDetail( "Storage Type", _storageType );
      ex.addDetail( "Index", i );
      ex.addDetail( "Size", _names.size() );
      throw ex;
    }

    _data[i] = data;
  }


  template < class DATA, bool OWNSMEMORY >
  void NamedVector<DATA, OWNSMEMORY>::set( std::string name, DATA* data )
  {
    if ( ! exists( name ) )
    {
      Exception ex( "NamedVector::set()", "Index outside of map size", true );
      ex.addDetail( "Storage Type", _storageType );
      ex.addDetail( "Name", name );
      ex.addDetail( "Size", _names.size() );
      throw ex;
    }

    size_t id = this->getID( name );
    _data[id] = data;
  }


  template < class DATA, bool OWNSMEMORY >
  bool NamedVector<DATA, OWNSMEMORY>::exists( std::string objName ) const
  {
    NameMap::const_iterator found = _names.find( objName );
    if ( found == _names.end() ) return false;
    else return true;
  }


  template < class DATA, bool OWNSMEMORY >
  bool NamedVector<DATA, OWNSMEMORY>::exists( size_t i ) const
  {
    if ( i >= _names.size() ) return false;
    else return true;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Partial Specialisation for Owned memory

  template < class DATA >
  class NamedVector< DATA, true >
  {
    typedef std::map< std::string, size_t > NameMap;
    typedef std::vector< DATA* > DataVector;

    public:
      typedef typename std::vector< DATA* >::iterator iterator;
      typedef typename std::vector< DATA* >::const_iterator const_iterator;

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

      std::string getName( size_t ) const;

      DATA* get( std::string );

      DATA* get( size_t );

      void set( size_t, DATA* );

      void set( std::string, DATA* );

      DATA* operator[]( size_t i ) { return _data[ i ]; }
      
      const DATA* operator[]( size_t i ) const { return _data[ i ]; }

      bool exists( std::string ) const;

      bool exists( size_t ) const;


      iterator begin() { return _data.begin(); }
      const_iterator begin() const { return _data.begin(); }

      iterator end() { return _data.end(); }
      const_iterator end() const { return _data.end(); }

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
      Exception ex( "NamedVector::getID()", "Could not find name in map", true );
      ex.addDetail( "Storage Type", _storageType );
      ex.addDetail( "Name", objName );
      throw ex;
    }

    return found->second;
  }


  template < class DATA >
  std::string NamedVector<DATA, true>::getName( size_t id ) const
  {
    for ( NameMap::const_iterator it = _names.begin(); it != _names.end(); ++it )
    {
      if ( it->second == id )
        return it->first;
    }

    Exception ex( "NamedVector::getName()", "Could not find id in map" );
    ex.addDetail( "Storage Type", _storageType );
    ex.addDetail( "ID", id );
    throw ex;
  }


  template < class DATA >
  DATA* NamedVector<DATA, true>::get( std::string name )
  {
    size_t id = this->getID( name );
    return _data[ id ];
  }


  template < class DATA >
  DATA* NamedVector<DATA, true>::get( size_t i )
  {
    if ( i >= _names.size() )
    {
      Exception ex( "NamedVector::get()", "Index outside of map size", true );
      ex.addDetail( "Storage Type", _storageType );
      ex.addDetail( "Index", i );
      ex.addDetail( "Size", _names.size() );
      throw ex;
    }

    return _data[i];
  }


  template < class DATA >
  void NamedVector<DATA, true>::set( size_t i, DATA* data )
  {
    if ( i >= _names.size() )
    {
      Exception ex( "NamedVector::set()", "Index outside of map size", true );
      ex.addDetail( "Storage Type", _storageType );
      ex.addDetail( "Index", i );
      ex.addDetail( "Size", _names.size() );
      throw ex;
    }

    _data[i] = data;
  }


  template < class DATA >
  void NamedVector<DATA, true>::set( std::string name, DATA* data )
  {
    if ( ! exists( name ) )
    {
      Exception ex( "NamedVector::set()", "Index outside of map size", true );
      ex.addDetail( "Storage Type", _storageType );
      ex.addDetail( "Name", name );
      ex.addDetail( "Size", _names.size() );
      throw ex;
    }

    size_t id = this->getID( name );
    _data[id] = data;
  }


  template < class DATA >
  bool NamedVector<DATA, true>::exists( std::string objName ) const
  {
    NameMap::const_iterator found = _names.find( objName );
    if ( found == _names.end() ) return false;
    else return true;
  }


  template < class DATA >
  bool NamedVector<DATA, true>::exists( size_t i ) const
  {
    if ( i >= _names.size() ) return false;
    else return true;
  }

}

#endif // REGOLITH_UTILITIES_NAMED_VECTOR_H_

