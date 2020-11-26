
#ifndef REGOLITH_ARCHITECTURE_GAME_OBJECT_H_
#define REGOLITH_ARCHITECTURE_GAME_OBJECT_H_

#include "Regolith/Architecture/MassProduceable.h"

namespace Regolith
{
  class ContextGroup;
  class DataHandler;

  /*
   * The base class for all objects that can appear (event without a drawable interface) within the game)
   * The Virtual functions must be overriden correctionly to directly indicate where or not the objects pointer can be dynamically cast to the corresponding interface class pointer:
   *
   * Audio => Noisy
   * Input => Controllable
   * Button => Focusable
   * Animation => Animated
   * Texture => Drawable
   * Movement => Movable
   */
  class GameObject : public MassProduceable< ContextGroup& >
  {
    public:
      virtual ~GameObject() {}

      virtual bool hasAudio() const { return false; }
      virtual bool hasInput() const { return false; }
      virtual bool hasButton() const { return false; }
      virtual bool hasAnimation() const { return false; }
      virtual bool hasTexture() const { return false; }
      virtual bool hasMovement() const { return false; }
      virtual bool isPhysical() const { return false; }
  };

}

#endif // REGOLITH_ARCHITECTURE_GAME_OBJECT_H_

