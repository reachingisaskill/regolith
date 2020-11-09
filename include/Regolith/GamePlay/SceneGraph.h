
#ifndef REGOLITH_ARCHITECTURE_SCENE_GRAPH_H_
#define REGOLITH_ARCHITECTURE_SCENE_GRAPH_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/Architecture/Drawable.h"
#include "Regolith/Architecture/Moveable.h"
#include "Regolith/Architecture/Collidable.h"
#include "Regolith/Architecture/Clickable.h"

#include <list>
#include <set>


namespace Regolith
{

  class SceneGraph
  {
    typedef std::list< SceneGraph* > NodeList;
    friend class SceneGraph::SceneIterator;

////////////////////////////////////////////////////////////////////////////////
    private:
      // List of the sub-nodes of the scene graph (recursive data structure)
      // This class owns the memory of its children
      NodeList _children;

      Vector _position; // Can be considered as the offset wrt to the camera
      Vector _movementScale; // Movement wrt the camera position
      float _width;
      float _height;


      // List of all drawable objects
      DrawableList _drawables;

      // List of all moveable objects
      MoveableList _moveables;

      // Map of all teams.
      TeamMap _teams;

      // Set of all the objects that are "clickable"
      ClickableSet _clickables;

      // Cache of all objects that are animated
      AnimatedList _animated;


////////////////////////////////////////////////////////////////////////////////
    public:
      // Constuct
      SceneGraph();
      // Clear all the caches
      ~SceneGraph();

      // Configure with position, movement scale, width and height
      void configure( Context*, Json::Value& );


////////////////////////////////////////////////////////////////////////////////
      // Node details

      // Return the position of the node
      const Vector& getPosition() const { return _position; }

      // Return movement speed wrt camera
      const Vector& getMovementScale() const { return _movementScale; }

      // Return width & height
      const float& getWidth() const { return _width; }
      const float& getHeight() const { return _height; }


////////////////////////////////////////////////////////////////////////////////
      // Object spawning and caching

      // Spawn a copy of the provided object at the requested position 
      void spawn( PhysicalObject*, const Vector& );

      // Cache an object in this node
      virtual void cacheObject( GameObject* );


////////////////////////////////////////////////////////////////////////////////////////////////////
      // Iterator sub class for iterating through the nodes

      class SceneIterator
      {
        private:
          SceneGraph* _currentNode;

        public:
          SceneIterator();

          DrawableList& drawables() const;
          MoveableList& moveables() const;
          TeamMap& teams() const;
          ClickableSet& clickables() const;
          AnimatedList& animated() const;
      };

  };

}

#endif // REGOLITH_ARCHITECTURE_SCENE_GRAPH_H_

