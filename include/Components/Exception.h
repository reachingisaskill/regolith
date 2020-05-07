
#ifndef REGOLITH_EXCEPTION_H_
#define REGOLITH_EXCEPTION_H_

#include <vector>
#include <string>
#include <sstream>
#include <exception>


namespace Regolith
{

  class Exception : public std::exception
  {
    private:
      std::string _error;
      std::vector< std::string > _details;
      std::string _elucidate;
      bool _recoverable;

    public:
      // Function name, error, details
      Exception( std::string, std::string, bool recoverable = true );
      Exception( const Exception& );
      Exception( const Exception&& );

//      template < class T >
//      void addDetail( std::string, T& ); // Add variable names and values

      template < class T >
      void addDetail( std::string, T ); // Add variable names and values

      size_t numberDetails() const { return _details.size(); }

      bool isRecoverable() const { return _recoverable; }

      virtual ~Exception() {}

      Exception& operator=( const Exception& ) noexcept;
      Exception& operator=( const Exception&& ) noexcept;

      virtual const char* what() const noexcept;

      virtual const char* elucidate();
  };

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Template Functions

//  template< class T >
//  void Exception::addDetail( std::string variable, T& value )
//  {
//    std::stringstream stream;
//    stream << variable << " = " << value;
//
//    _details.push_back( stream.str() );
//  }

  template< class T >
  void Exception::addDetail( std::string variable, T value )
  {
    std::stringstream stream;
    stream << variable << " = " << value;

    _details.push_back( stream.str() );
  }

}

#endif // REGOLITH_EXCEPTION_H_
