
#ifndef REGOLITH_UTILITIES_NAMED_REFERENCE_VECTOR_H_
#define REGOLITH_UTILITIES_NAMED_REFERENCE_VECTOR_H_

#include "Regolith/Utilities/Exception.h"

#include "logtastic.h"

#include <vector>
#include <map>
#include <string>


namespace Regolith
{

  // Sligthly different version - if the OWNS_MEMORY Flag is not present - do no store as pointers!
  template < class DATA >
  class NamedReferenceVector
  {
    typedef std::map< std::string, size_t > NameMap;
    typedef std::vector< DATA > DataVector;

    public:
      typedef typename std::vector< DATA >::iterator iterator;
      typedef typename std::vector< DATA >::const_iterator const_iterator;

    private :
      std::string _storageType;
      NameMap _names;
      DataVector _data;

    public :
      NamedReferenceVector( const char* );

      ~NamedReferenceVector();


      void clear();

      size_t addObject( DATA, std::string );

      size_t addName( std::string );

      size_t getID( std::string ) const;

      std::string getName( size_t ) const;

      DATA& get( std::string );

      DATA& get( size_t );

      void set( size_t, DATA );

      void set( std::string, DATA );

      DATA& operator[]( size_t i ) { return _data[ i ]; }

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

  template < class DATA >
  NamedReferenceVector<DATA>::NamedReferenceVector( const char* type ) :
    _storageType( type ),
    _names(),
    _data()
  {
  }


  template < class DATA >
  NamedReferenceVector<DATA>::~NamedReferenceVector()
  {
  }


  template < class DATA >
  void NamedReferenceVector<DATA>::clear()
  {
    _names.clear();
    _data.clear();
  }


  template < class DATA >
  size_t NamedReferenceVector<DATA>::addObject( DATA object, std::string objName )
  {
    if ( exists( objName ) )
    {
      WARN_STREAM << "Named Vector: " << _storageType << ". Overwriting object with name : " << objName;
    }

    size_t position = this->addName( objName );

    _data[ position ] = object;
    return position;
  }


  template < class DATA >
  size_t NamedReferenceVector<DATA>::addName( std::string objName )
  {
    NameMap::iterator found = _names.find( objName );

    if ( found != _names.end() )
    {
      return found->second;
    }

    size_t position = _names.size();
    _names[ objName ] = position;

    _data.push_back( DATA() );
    return position;
  }


  template < class DATA >
  size_t NamedReferenceVector<DATA>::getID( std::string objName ) const
  {
    NameMap::const_iterator found = _names.find( objName );

    if ( found == _names.end() )
    {
      Exception ex( "NamedReferenceVector::getID()", "Could not find name in map", true );
      ex.addDetail( "Storage Type", _storageType );
      ex.addDetail( "Name", objName );
      throw ex;
    }

    return found->second;
  }


  template < class DATA >
  std::string NamedReferenceVector<DATA>::getName( size_t id ) const
  {
    for ( NameMap::const_iterator it = _names.begin(); it != _names.end(); ++it )
    {
      if ( it->second == id )
        return it->first;
    }

    Exception ex( "NamedReferenceVector::getName()", "Could not find id in map" );
    ex.addDetail( "Storage Type", _storageType );
    ex.addDetail( "ID", id );
    throw ex;
  }


  template < class DATA >
  DATA& NamedReferenceVector<DATA>::get( std::string name )
  {
    size_t id = this->getID( name );
    return _data[ id ];
  }


  template < class DATA >
  DATA& NamedReferenceVector<DATA>::get( size_t i )
  {
    if ( i >= _names.size() )
    {
      Exception ex( "NamedReferenceVector::get()", "Index outside of map size", true );
      ex.addDetail( "Storage Type", _storageType );
      ex.addDetail( "Index", i );
      ex.addDetail( "Size", _names.size() );
      throw ex;
    }

    return _data[i];
  }


  template < class DATA >
  void NamedReferenceVector<DATA>::set( size_t i, DATA data )
  {
    if ( i >= _names.size() )
    {
      Exception ex( "NamedReferenceVector::get()", "Index outside of map size", true );
      ex.addDetail( "Storage Type", _storageType );
      ex.addDetail( "Index", i );
      ex.addDetail( "Size", _names.size() );
      throw ex;
    }

    _data[i] = data;
  }


  template < class DATA >
  void NamedReferenceVector<DATA>::set( std::string name, DATA data )
  {
    if ( ! exists( name ) )
    {
      Exception ex( "NamedReferenceVector::get()", "Index outside of map size", true );
      ex.addDetail( "Storage Type", _storageType );
      ex.addDetail( "Name", name );
      ex.addDetail( "Size", _names.size() );
      throw ex;
    }

    size_t id = this->getID( name );
    _data[id] = data;
  }


  template < class DATA >
  bool NamedReferenceVector<DATA>::exists( std::string objName ) const
  {
    NameMap::const_iterator found = _names.find( objName );
    if ( found == _names.end() ) return false;
    else return true;
  }


  template < class DATA >
  bool NamedReferenceVector<DATA>::exists( size_t i ) const
  {
    if ( i >= _names.size() ) return false;
    else return true;
  }

}

#endif // REGOLITH_UTILITIES_NAMED_REFERENCE_VECTOR_H_
