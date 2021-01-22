
#include "Regolith/Utilities/TileSet.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  TileSet::TileSet( std::string file ) :
    _filename( file ),
    _optimize( false ),
    _tileWidth( 0.0 ),
    _tileHight( 0.0 ),
    _width( 0.0 ),
    _height( 0.0 ),
    _numCells( 0 ),
    _matrix()
  {
    this->_configure();
  }


  TileSet::~TileSet()
  {
  }


  void TileSet::_configure()
  {
    // Load the tile set file
    Json::Value json_data;
    loadJsonData( json_data, _filename );

    // Validate the data
    validateJson( json_data, "tile_width", JsonType::FLOAT );
    validateJson( json_data, "tile_height", JsonType::FLOAT );
    validateJson( json_data, "tile_matrix", JsonType::ARRAY );
    validateJsonArray( json_data["tile_matrix"], 0, JsonType::ARRAY );

    _tileWidth = json_data["tile_width"].asFloat();
    _tileHeight = json_data["tile_height"].asFloat();

    Json::Value& matrix_data = json_data["tile_matrix"];
    unsigned int num_rows = matrix_data.size();
    unsigned int num_cols = matrix_data[0].size();

    _matrix.reserve( num_rows );

    // Iterate through the rows
    for ( Json::ArrayIndex i = 0; i < matrix_data.size(); ++i )
    {
      Json::Value& row_data = matrix_data[i];

      // Validate the row_data
      validateJsonArray( row_data, 0, JsonType::INTEGER );

      // Make sure everything's the same size
      if ( row_data.size() != num_cols )
      {
        Exception ex( "TileSet::_configure()", "Number of columns in tile matrix is not consistent." );
        ex.addDetail( "First Row", num_cols );
        ex.addDetail( "Row Number", i );
        ex.addDetail( "Found Columns", row_data.size() );
        throw ex;
      }
      
      // Push a fresh row_data and reserve space
      _matrix.push_back( TileRow( num_cols, 0 ) );

      // Iterate through the columns
      for ( Json::ArrayIndex j = 0; j < row_data.size(); ++j )
      {
        int value = row_data[j].asInt();

        // Make sure its a positive integer.
        if ( value < 0 )
        {
          WARN_STREAM << "TileSet::_configure : Tile Set file: " << _filename << ", matrix entries must be unsigned integers: " << value;
        }
        value = std::abs( value );

        // Update the number of required cells if needed.
        if ( value > _numCells ) _numCells = value;

        _matrix[i].push_back( value );
      }
    }

  }


  SDL_Surface* TileSet::buildSurface( RawTexture* texture )
  {
    if ( texture->cells < _numCells )
    {
      Exception ex( "TileSet::buldSurface()", "Provided texture does not have enough cells." );
      ex.addDetail( "Number Provided", texture->cells );
      ex.addDetail( "Number Required", _numCells );
      throw ex;
    }



  }


}

