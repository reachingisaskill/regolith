
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

      int getProperties() const { return OBJECT_SIMPLE; }

      void render( Camera* ) {}

      void update( Uint32 ) {}

      void handleEvent( SDL_Event& ) {}
      
      unsigned int getCollision( Collision*& );

      Drawable* clone() const;
  };

}

#endif // __REGOLITH__TEST_DRAWABLE_H__

