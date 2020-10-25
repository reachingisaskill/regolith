
#include "Regolith/Utilities/Exception.h"


typedef std::vector< std::string > DetailVector;
      

namespace Regolith
{

  Exception::Exception( std::string function, std::string name, bool recoverable ) :
    _error( name + " in " + function ),
    _details(),
    _recoverable( recoverable )
  {
    if ( _recoverable )
    {
      ERROR_STREAM << "Recoverable exception has been thrown from \"" << function << "\" with error: " << name;
    }
    else
    {
      FAILURE_STREAM << "Non-recoverable exception has been thrown from \"" << function << "\" with error: " << name;
    }
  }


  Exception::Exception( const Exception& ex ) :
    _error( ex._error ),
    _details( ex._details ),
    _recoverable( ex._recoverable )
  {
  }


  Exception::Exception( const Exception&& ex ) :
    _error( std::move(ex._error) ),
    _details( std::move(ex._details) ),
    _recoverable( std::move(ex._recoverable) )
  {
  }


  Exception& Exception::operator=( const Exception& ex ) noexcept
  {
    this->_error = ex._error;
    this->_details = ex._details;

    return *this;
  }


  Exception& Exception::operator=( const Exception&& ex ) noexcept
  {
    this->_error = ex._error;
    this->_details = ex._details;

    return *this;
  }


  const char* Exception::what() const noexcept
  {
    return _error.c_str();
  }


  std::string Exception::elucidate() const
  {
    std::stringstream whatStream;
    whatStream << ( _recoverable ? "Recoverable" : "Non-recoverable" ) << " exception occured: " <<  _error;

    DetailVector::const_iterator end = _details.end();
    for ( DetailVector::const_iterator it = _details.begin(); it != end; ++it )
    {
      whatStream << "\n   + " << (*it);
    }
    whatStream << '\n';

    return whatStream.str();
  }
}

