
#include "Regolith/Collisions/TilesheetCollision.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Utilities/TileSet.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  TilesheetCollision::TilesheetCollision() :
    _hitboxes()
  {
  }


  void TilesheetCollision::configure( Json::Value& json_data )
  {
    validateJson( json_data, "tile_set", JsonType::STRING );
    validateJson( json_data, "collision_type", JsonType::STRING );

    // Load the tile set
    std::string tile_set = json_data["tile_set"].asString();
    TileSet the_tiles( tile_set );

    // Collision type for the hitboxes
    CollisionType collision_type = Manager::getInstance()->getCollisionType( json_data["collision_type"].asString() );

    // Build the hitboxes
    size_t num_rows = the_tiles.getNumberRows();
    size_t num_cols = the_tiles.getNumberColumns();

    // Tile dimensions
    float width = the_tiles.getTileWidth();
    float height = the_tiles.getTileHeight();


    for ( size_t row = 0; row < num_rows; ++row )
    {
      for ( size_t col = 0; col < num_cols; ++col )
      {
        if ( the_tiles( row, col ) > 0 )
        {
          HitBox hb;

          hb.collisionType = collision_type;
          hb.number = 4;
          hb.points.reserve( 4 );
          hb.normals.reserve( 4 );

          hb.points.push_back( Vector( col*width, row*height ) );
          hb.points.push_back( Vector( (col+1)*width, row*height ) );
          hb.points.push_back( Vector( (col+1)*width, (row+1)*height ) );
          hb.points.push_back( Vector( col*width, (row+1)*height ) );

          hb.normals.push_back( -unitVector_y );
          hb.normals.push_back(  unitVector_x );
          hb.normals.push_back(  unitVector_y );
          hb.normals.push_back( -unitVector_x );

          _hitboxes.push_back( hb );

          DEBUG_STREAM << "TilesheetCollision::configure : Added HitBox: " << Vector( col*width, row*height ) << ", " << Vector( (col+1)*width, row*height ) << ", " << Vector( (col+1)*width, (row+1)*height ) << ", " <<  Vector( col*width, (row+1)*height );
        } 
      }
    }

  }


}

