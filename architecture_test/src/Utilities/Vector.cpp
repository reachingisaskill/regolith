
#include "Regolith/Utilities/Vector.h"

#include <cmath>


namespace Regolith
{

  Vector::Vector() :
    _x(),
    _y()
  {
  }


  Vector::Vector( float val ) :
    _x( val ),
    _y( val )
  {
  }


  Vector::Vector( float x, float y ) :
    _x( x ),
    _y( y )
  {
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Lengths and norms

  double Vector::square() const
  {
    return _x*_x + _y*_y;
  }


  double Vector::mod() const
  {
    return std::sqrt(_x*_x + _y*_y);
  }


  Vector Vector::norm() const
  {
    float mod = this->mod();
    return Vector( _x/mod, _y/mod );
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Rotations

  Vector& Vector::rotate( float angle )
  {
    float cos = std::cos( angle );
    float sin = std::sin( angle );

    float new_x = _x*cos - _y*sin;
    this->_y = _x*sin + _y*cos;
    _x = new_x;

    return *this;
  }


  Vector Vector::getRotated( float angle ) const
  {
    float cos = std::cos( angle );
    float sin = std::sin( angle );

    float new_x = _x*cos - _y*sin;
    float new_y = _x*sin + _y*cos;

    return Vector( new_x, new_y );
  }


  Vector& Vector::rotateAbout( float angle, const Vector& vec )
  {
    float cos = std::cos( angle );
    float sin = std::sin( angle );

    float new_x = (_x-vec._x)*cos - (_y-vec._y)*sin;
    float new_y = (_x-vec._x)*sin + (_y-vec._y)*cos;
    _x = new_x + vec._x;
    _y = new_y + vec._y;

    return *this;
  }


  Vector Vector::getRotatedAbout( float angle, const Vector& vec ) const
  {
    float cos = std::cos( angle );
    float sin = std::sin( angle );

    float new_x = (_x-vec._x)*cos - (_y-vec._y)*sin;
    float new_y = (_x-vec._x)*sin + (_y-vec._y)*cos;

    return Vector( new_x+vec._x, new_y+vec._y );
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Operators

  Vector Vector::operator*( float f ) const // Multiply by constant
  {
    return Vector( _x*f, _y*f );
  }


  float Vector::operator*( const Vector& other ) const // Dot Product
  {
    return this->_x*other._x + this->_y*other._y;
  }


  float Vector::operator^( const Vector& other ) const // Cross Product
  {
    return this->_x*other._y - this->_y*other._x;
  }


  Vector Vector::operator%( const Vector& other ) const // Directional Dot Product
  {
    return Vector( this->_x*other._x, this->_y*other._y );
  }


  Vector Vector::operator+( const Vector& other ) const // Addition
  {
    return Vector( this->_x+other._x, this->_y+other._y );
  }


  Vector Vector::operator-( const Vector& other ) const // Subtraction
  {
    return Vector( this->_x-other._x, this->_y-other._y );
  }


  Vector Vector::operator-() const // Unary minus
  {
    return Vector( -_x, -_y );
  }


  Vector& Vector::operator+=( const Vector& other ) // Plus-assign
  {
    this->_x += other._x;
    this->_y += other._y;
    return *this;
  }


  Vector& Vector::operator-=( const Vector& other ) /// Minus-assign
  {
    this->_x -= other._x;
    this->_y -= other._y;
    return *this;
  }


  Vector& Vector::operator*=( float f )
  {
    this->_x *= f;
    this->_y *= f;
    return *this;
  }


  Vector& Vector::operator/=( float f )
  {
    this->_x /= f;
    this->_y /= f;
    return *this;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Friend Functions

  Vector operator*( float f, Vector v ) // Pre-Mulitply by constant
  {
    return Vector( f*v._x, f*v._y );
  }


  bool operator==( const Vector& v1, const Vector& v2 ) // Equal to
  {
    if ( std::fabs(v1._x - v2._x) > epsilon )
      return false;
    if ( std::fabs(v1._y - v2._y) > epsilon )
      return false;
    return true;
  }


  bool operator!=( const Vector& v1, const Vector& v2 ) // Equal to
  {
    if ( ( std::fabs(v1._x - v2._x) > epsilon ) && ( std::fabs(v1._y - v2._y) > epsilon ) )
      return true;
    return false;
  }


  bool operator>( const Vector& v1, const Vector& v2 ) // Greater than
  {
    return v1.square() - v2.square() > epsilon; // Skip the square root. Slow!
  }


  bool operator>=( const Vector& v1, const Vector& v2 ) // Greater, equal to
  {
    return v1.square() >= v2.square(); // Skip the square root. Slow!
  }


  bool operator<( const Vector& v1, const Vector& v2 ) // Less Than
  {
    return v2.square() - v1.square() > epsilon; // Skip the square root. Slow!
  }


  bool operator<=( const Vector& v1, const Vector& v2 ) // Less, equal to
  {
    return v1.square() <= v2.square(); // Skip the square root. Slow!
  }


  std::ostream& operator<<( std::ostream& os, Vector v )
  {
    os << "(" << v._x << ", " << v._y << ")";
    return os;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Related Functions

  float radians( float deg )
  {
    static float constant = pi / 180.0f;
    return constant * deg;
  }


  float degrees( float rad )
  {
    static float constant = 180.0f / pi;
    return constant * rad;
  }
}

