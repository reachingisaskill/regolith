
#ifndef REGOLITH_ARCHITECTURE_CONTEXT_LAYER_H_
#define REGOLITH_ARCHITECTURE_CONTEXT_LAYER_H_

#include "Global/Global.h"
#include "Achitecture/PhysicalObject.h"
#include "Achitecture/Drawable.h"
#include "Achitecture/Movable.h"
#include "Achitecture/Collidable.h"

#include <list>


namespace Regolith
{
  namespace Interfaces
  {

    typedef std::list< Drawable* > DrawableList;
    typedef std::list< Moveable* > MoveableList;
    typedef std::list< Collidable* > CollidableList;
    typedef std::map< int, CollidableList > TeamMap;

    class ContextLayer
    {

      private:
        Vector _position;
        float _width;
        float _height;

      public:
        // Constuct with position, width and height
        ContextLayer( Vector, float, float );
        // Clear all the caches
        ~ContextLayer();

        // List of all drawable objects
        DrawableList drawables;

        // List of all moveable objects
        MoveableList movables;

        // Map of all teams. Note a team cannot collide with it's own members!
        TeamMap teams;

    };

  }
}

#endif // REGOLITH_ARCHITECTURE_CONTEXT_LAYER_H_

