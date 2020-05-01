
#ifndef REGOLITH_TEST_DRAWABLE_H_
#define REGOLITH_TEST_DRAWABLE_H_

#include "Drawable.h"
#include "Collision.h"


namespace Regolith
{

  class TestDrawable : public Drawable
  {
    private:
      unsigned int _collisionCount;
      Collision _collision[2];

      float _moveDistance;
      bool _jump;
      bool _attack;

    public:
      TestDrawable();

      virtual bool hasCollision() const { return _collision != nullptr; }
      virtual bool hasInput() const { return false; }
      virtual bool hasAnimation() const { return this->isMovable(); }

      void render( Camera* ) {}

      void update( Uint32 ) {}

      void registerActions( InputHandler* );
      
      unsigned int getCollision( Collision*& );

      void addCollision( Collision* );

      Drawable* clone() const;

      void booleanAction( const InputAction&, bool );
      void floatAction( const InputAction&, float );
      void vectorAction( const InputAction&, const Vector& );


      bool getJump() { return _jump; }
      bool getAttack() { return _attack; }
  };

}

#endif // REGOLITH_TEST_DRAWABLE_H_

