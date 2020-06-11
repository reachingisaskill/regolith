#!/bin/env python3

import shutil
import os
import argparse
import json

import sdl2
import sdl2.ext as sdlext
import sdl2.sdlttf as sdlttf

IMAGE_EXTENSIONS = sdlext.get_image_formats()
MUSIC_EXTENSIONS = [ "mp3" ]
SOUND_EXTENSIONS = [ "wav" ]


def format_index( index_string ) :
  depth = -1
  new_string = ""
  indent = "  "
  close_flag = False
  eat_space = False

  for char in index_string :
    if char == '{' :
      depth += 1
      if depth > 1 :
        new_string += "{ "
      else :
        new_string += "\n" + indent*depth + "{\n" + indent*(depth+1)
    elif char == '}' :
      if depth > 1 :
        new_string += " }"
      else :
        new_string += "\n" + indent*depth + "}"
      close_flag = True
      depth -= 1
    elif char == ',' and close_flag :
      if depth < 3 :
        new_string += char + "\n" + indent*(depth+1)
        eat_space = True
      else :
        new_string += char
      close_flag = False
    elif char == ' ' and eat_space :
      eat_space = False
      continue
    else :
      new_string += char
  return new_string


if __name__ == "__main__" :

  sdlttf.TTF_Init()

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
          print( os.path.join( root, filename ) )
          temp_surface = sdlext.load_image( os.path.join( root, filename ) )
          print( "Found image :", os.path.join( root, filename ), " w =", temp_surface.w, " h =", temp_surface.h )

          del temp_surface
          continue

        if extension in MUSIC_EXTENSIONS :
          print( "Found music :", os.path.join( root, filename ) )
          continue

        if extension in SOUND_EXTENSIONS :
          print( "Found sound :", os.path.join( root, filename ) )
          continue


  print()

  font_dict = json_data["fonts"]

# Check the text textures by hand
  for texture_name in json_data["textures"].keys() :
    texture_data = json_data["textures"][texture_name]

    if texture_data["type"] == "text" :
      font_name = texture_data["font"]

      if font_name not in font_dict :
        print( "Error : Font \"", font_name, "\" required by text texture \"", texture_name, "\" not found" )
      else :

        font_path = font_dict[ font_name ]["path"]
        font_size = font_dict[ font_name ]["size"]
        temp_font = sdlttf.TTF_OpenFont( font_path.encode(), font_size )

        temp_surface = sdlttf.TTF_RenderText_Solid( temp_font, texture_data["text"].encode(), sdl2.SDL_Color( 255, 255, 255 ) ).contents

        print( "Found text texture: ", texture_data["text"], " --- w =", temp_surface.w, " h =", temp_surface.h )

        del temp_surface
        sdlttf.TTF_CloseFont( temp_font )


      





#  with open( namespace.file, 'w' ) as json_file :
#    json.dump( json_data, json_file, indent="  " )
  print( format_index( json.dumps( json_data ) ) )

  sdlttf.TTF_Quit()

