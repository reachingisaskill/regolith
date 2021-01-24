
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
      int _tileWidth;
      int _tileHeight;

      // Whole tileset dimensions
      int _width;
      int _height;

      // Dimensions of the internal arrays
      size_t _numRows;
      size_t _numCols;


      // The number of cells that the texture is required to have.
      unsigned int _numCells;

      // Matrix of which texture goes where
      TileMatrix _matrix;


    protected:
      // Function handles the heavy lifting.
      void _configure();


    public:
      TileSet( std::string );

      ~TileSet();


      // Return the width of a tile
      float getTileWidth() const { return _tileWidth; }

      // Return the height of a tile
      float getTileHeight() const { return _tileHeight; }

      // Return the dimensions of the whole tiled structure.
      float getWidth() const { return _width; }
      float getHeight() const { return _height; }



      // Return the number of rows
      size_t getNumberRows() const { return _numRows; }

      // Return the number of columns
      size_t getNumberColumns() const { return _numCols; }


      // Return the value at the requested tile
      unsigned int operator()( size_t r, size_t c ) const { return _matrix[r][c]; }
      unsigned int get( size_t r, size_t c ) const { return _matrix[r][c]; }

  };

}

#endif // REGOLITH_GAME_PLAY_TILE_SET_H_

