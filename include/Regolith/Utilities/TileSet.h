
#ifndef REGOLITH_UTILITIES_TILE_SET_H_
#define REGOLITH_UTILITIES_TILE_SET_H_

#include "Regolith/Global/Global.h"

#include <vector>


namespace Regolith
{

  /*
   *  Helper class for reading tile set files and configuring textures and collision objects
   */
  class TileSet
  {
    typedef std::vector< unsigned int > TileRow;
    typedef std::vector< TileRow > TileMatrix;

    private:
      // File containing the tiling info
      std::string _filename;

      // Flag to indicate that neighbouring cells can be joined when building hitboxes
      bool _optimize;

      // Tile dimensions
      float _tileWidth;
      float _tileHeight;

      // Whole tileset dimensions
      float _width;
      float _height;

      // The number of cells that the texture is required to have.
      unsigned int _numCells;

      // Matrix of which texture goes where
      TileMatrix _matrix;


    protected:
      // Function handles the heavy lifting.
      void _configure();


    public:
      TileSet( std::string, RawTexture* );

      ~TileSet();

      SDL_Surface* buildSurface( RawTexture* );

      // void buildHitBoxes( HitBoxVector& );
  };

}

#endif // REGOLITH_GAME_PLAY_TILE_SET_H_

