
#ifndef REGOLITH_UTILITIES_VECTOR_H_
#define REGOLITH_UTILITIES_VECTOR_H_

#include "Regolith/Global/Global.h"

#include <ostream>


namespace Regolith
{

  class Vector
  {
    private:
      float _x;
      float _y;

    public:
      explicit Vector();
      explicit Vector( float );
      Vector( float, float );

      // Use the Compilers default versions for the trival functions.
//      Vector( const Vector& ); // Copy Construction
//      Vector& operator= ( const Vector& ); // Copy assignment
//      Vector& operator= ( const float& );
//      Vector( const Vector&& ); // Move Construction
//      Vector& operator= ( const Vector&& ); // Move assignment
//      ~Vector(); // Destructor

      void set( float x, float y ) { _x = x; _y = y; }
      void zero() { _x = 0.0; _y = 0.0; }

      float& x() { return _x; }
      const float& x() const { return _x; }
      float& y() { return _y; }
      const float& y() const { return _y; }


      double square() const;
      double mod() const;
      Vector norm() const;
      Vector fabs() const;


      Vector& rotate( float ); // Rotate about the origin
      Vector getRotated( float ) const;
      Vector& rotateAbout( float, const Vector& ); // Rotate about a specified point
      Vector getRotatedAbout( float, const Vector& ) const;


      // Standard operator overloads
      Vector operator*( float ) const; // Multiply by constant
      Vector operator/( float ) const; // Divide by constant
      float operator*( const Vector& ) const; // Dot Product
      float operator^( const Vector& ) const; // Cross Product
      Vector operator%( const Vector& ) const; // Directional Dot Product
      Vector operator+( const Vector& ) const; // Addition
      Vector operator-( const Vector& ) const; // Subtraction
      Vector operator-() const; // Unary minus

      // Standard update operators
      Vector& operator+=( const Vector& ); // Plus-assign
      Vector& operator-=( const Vector& ); /// Minus-assign
      Vector& operator*=( float ); // Multiply-assign
      Vector& operator/=( float ); // Divide-assign


      // Friend function operators
      friend Vector operator* ( float, Vector ); // Pre-Mulitply by constant
      friend bool operator==( const Vector&, const Vector& ); // Equal to
      friend bool operator!=( const Vector& v1, const Vector& v2 ); // Not equal to
      friend bool operator>( const Vector&, const Vector& ); // Greater than
      friend bool operator>=( const Vector&, const Vector& ); // Greater, equal to
      friend bool operator<( const Vector&, const Vector& ); // Less Than
      friend bool operator<=( const Vector&, const Vector& ); // Less, equal to

      // Overload the streaming operator for ease
      friend std::ostream& operator<<( std::ostream&, Vector );
  };

  // Friend functions
  Vector operator* ( float, Vector ); // Pre-Mulitply by constant
  bool operator==( const Vector&, const Vector& ); // Equal to
  bool operator!=( const Vector&, const Vector& ); // Not equal to
  bool operator>( const Vector&, const Vector& ); // Greater than
  bool operator>=( const Vector&, const Vector& ); // Greater, equal to
  bool operator<( const Vector&, const Vector& ); // Less Than
  bool operator<=( const Vector&, const Vector& ); // Less, equal to


  // Angle Conversions
  float radians( float );
  float degrees( float );


  // Useful Definitions
  const Vector unitVector_x( 1.0f, 0.0f );
  const Vector unitVector_y( 0.0f, 1.0f );
}

#endif // REGOLITH_UTILITIES_VECTOR_H_

