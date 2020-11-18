
#ifndef REGOLITH_GAMEP_PLAY_SCENE_ITERATOR_H_
#define REGOLITH_GAMEP_PLAY_SCENE_ITERATOR_H_

#include "Regolith/Global/Global.h"


namespace Regolith
{

  /*
   * This class defines a 2D tree iterator for traversing the scene graph.
   * It updates the positions based on the current object such that global positions and rotations
   * are always accessible to the caller.
   */
  class SceneIterator
  {
    private:
      // Global position of the current object
      Vector _position;
      // Global rotation of the current object
      float _rotation;

      // Pointer to the current object
      PhysicalObject* _current;

    public:
      SceneIterator( PhysicalObject* );

  };

}

#endif // REGOLITH_GAMEP_PLAY_SCENE_ITERATOR_H_

