#!/bin/env python3

import shutil
import os
import argparse
import json
from PIL import Image

IMAGE_EXTENSIONS = [ "jpg", "jpeg", "png", "bmp" ]
MUSIC_EXTENSIONS = [ "mp3" ]
SOUND_EXTENSIONS = [ "wav" ]


if __name__ == "__main__" :
  parser = argparse.ArgumentParser( description="Finds all media resources and creates an index.json file for regolith to use during runtime." )
  parser.add_argument( "directories", nargs="+", help="Directories in which to search for resources." )
  parser.add_argument( "--file", default="./regolith_index.json", help="Specify the name of the index file. If the file exists a backup copy will be made." )

  namespace = parser.parse_args()

  json_data = {}

  if os.path.isfile( namespace.file ) :
    backup_file = namespace.file+".backup"
    print( "Found an existing index file. Making a copy:", backup_file )
    shutil.copyfile( namespace.file, backup_file )

    with open( namespace.file, 'r' ) as json_file :
      json_data = json.load( json_file )
   

  for directory in namespace.directories :
    for root, dirs, files in os.walk( directory ) :
      for filename in files :
        split_name = os.path.splitext( filename )
        
        name = root.split( os.sep )[-1] + "_" + split_name[0]
        extension = split_name[-1][1:]

        if extension in IMAGE_EXTENSIONS :
          temp_image = Image.open( os.path.join( root, filename ) )
          width, height = temp_image.size
          print( "Found image :", os.path.join( root, filename ), " w =", width, " h =", height )
          continue

        if extension in MUSIC_EXTENSIONS :
          print( "Found music :", os.path.join( root, filename ) )
          continue

        if extension in SOUND_EXTENSIONS :
          print( "Found sound :", os.path.join( root, filename ) )
          continue



#  with open( namespace.file, 'w' ) as json_file :
#    json.dump( json_data, json_file, indent="  " )
  print( json_data )

