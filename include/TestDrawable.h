
#ifndef __REGOLITH__TEST_DRAWABLE_H__
#define __REGOLITH__TEST_DRAWABLE_H__

#include "Drawable.h"
#include "Collision.h"


namespace Regolith
{

  class TestDrawable : public Drawable
  {
    private:
      unsigned int _collisionCount;
      Collision _collision[2];

    public:
      TestDrawable();

      virtual bool hasCollision() const { return _collision != nullptr; }
      virtual bool hasInput() const { return false; }
      virtual bool hasAnimation() const { return this->isMovable(); }

      void render( Camera* ) {}

      void update( Uint32 ) {}

      void registerEvents( InputHandler* );
      
      unsigned int getCollision( Collision*& );

      Drawable* clone() const;
  };

}

#endif // __REGOLITH__TEST_DRAWABLE_H__

