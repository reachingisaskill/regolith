
#ifndef REGOLITH_ARCHITECTURE_GAME_OBJECT_H_
#define REGOLITH_ARCHITECTURE_GAME_OBJECT_H_

#include "Regolith/Architecture/MassProduceable.h"

namespace Regolith
{

  namespace Architecture : public MassProduceable
  {

    /*
     * The base class for all objects that can appear (event without a drawable interface) within the game)
     * The Virtual functions must be overriden correctionly to directly indicate where or not the objects pointer can be dynamically cast to the corresponding interface class pointer:
     *
     * Audio => Noisy
     * Input => Controllable
     * Animation => Animated
     * Collision => Collidable
     * Texture => Drawable
     * Interaction => Interactable
     * Movement => Movable
     */
    class GameObject
    {
      public:
        virtual ~GameObject() {}

        bool hasAudio() const { return false; }
        bool hasInput() const { return false; }
        bool hasAnimation() const { return false; }
        bool hasCollision() const { return false; }
        bool hasTexture() const { return false; }
        bool hasInteraction() const { return false; }
        bool hasMovement() const { return false; }
        bool isPhysical() const override { return false; }
    };

  }
}

#endif // REGOLITH_ARCHITECTURE_GAME_OBJECT_H_

