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
FONT_EXTENSIONS = [ "ttf" ]

TEXTURE_KEYS = [ "path", "width", "height", "colour_key", "rows", "columns" ]
STRING_KEYS = [ "font", "text", "colour" ]
SOUND_KEYS = [ "path" ]
MUSIC_KEYS = [ "path" ]
FONT_KEYS = [ "path", "size" ]


def format_index( index_string ) :
  depth = -1
  new_string = ""
  indent = "  "
  close_flag = False
  eat_space = False

  for char in index_string :
    if char == '{' :
      depth += 1
      if depth == 0 :
        new_string += "{\n" + indent*(depth+1)
      elif depth > 1 :
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


def search( json_data, sub_key, value ) :
  for key in json_data.keys() :
    if sub_key in json_data[key] :
      if json_data[key][sub_key] == value :
        return key
  return None


if __name__ == "__main__" :

  sdlttf.TTF_Init()

  parser = argparse.ArgumentParser( description="Finds all media resources and creates an index.json file for regolith to use during runtime." )
  parser.add_argument( "directories", nargs="+", help="Directories in which to search for resources." )
  parser.add_argument( "--file", default="./regolith_index.json", help="Specify the name of the index file. If the file exists a backup copy will be made." )

  namespace = parser.parse_args()

  warnings = []
  new_textures = 0
  new_sounds = 0
  new_music = 0
  new_fonts = 0

  total_textures = 0
  total_sounds = 0
  total_music = 0
  total_fonts = 0
  total_strings = 0

  json_data = {}

  if os.path.isfile( namespace.file ) :
    backup_file = namespace.file+".backup"
    print( "Found an existing index file. Making a copy:", backup_file )
    print()
    shutil.copyfile( namespace.file, backup_file )

    with open( namespace.file, 'r' ) as json_file :
      json_data = json.load( json_file )


# Validate the top-level json objects
  if "fonts" not in json_data :
    json_data["fonts"] = {}
  if "sounds" not in json_data :
    json_data["sounds"] = {}
  if "music" not in json_data :
    json_data["music"] = {}
  if "strings" not in json_data :
    json_data["strings"] = {}
  if "textures" not in json_data :
    json_data["textures"] = {}
   

# Walk through the directtories and find all the files
  print( "Searching for files." )
  print()
  for directory in namespace.directories :
    for root, dirs, files in os.walk( directory ) :
      for filename in files :
        split_name = os.path.splitext( filename )
        name = split_name[0]
        extension = split_name[-1][1:]
        file_path = os.path.join( root, filename )


        if extension in IMAGE_EXTENSIONS :
          temp_surface = sdlext.load_image( file_path )
          width = temp_surface.w
          height = temp_surface.h
          sdl2.SDL_FreeSurface( temp_surface )

          if name in json_data["textures"] :
            if "path" not in json_data["textures"][name] :
              warnings.append( "WARNING: Texture \"" + name + "\" has entry in index, but no \"path\" value. Fixed!" )
              json_data["textures"][name]["path"] = file_path

            elif json_data["textures"][name]["path"] != file_path :
              warnings.append( "WARNING: Texture \"" + name + "\" has entry in index, with a different file path." )
              continue

            if "rows" not in json_data["textures"][name] :
              json_data["textures"][name]["rows"] = 1
            if "columns" not in json_data["textures"][name] :
              json_data["textures"][name]["columns"] = 1

            json_data["textures"][name]["width"] = width
            json_data["textures"][name]["height"] = height

          else :
            element = search( json_data["textures"], "path", file_path )
            if element != None :
              warnings.append( "WARNING: Texture path \""+file_path+"\" found associated with differently named texture: "+element+"." );

              if "rows" not in json_data["textures"][element] :
                json_data["textures"][element]["rows"] = 1
              if "columns" not in json_data["textures"][element] :
                json_data["textures"][element]["columns"] = 1

              if "width" in json_data["textures"][element] and json_data["textures"][element]["width"] != width :
                warnings.append( "WARNING: Texture \"" + name + "\" has entry in index, with a width: "+str( json_data["textures"][element]["width"] )+" != "+str(width )+"." )
              elif "height" in json_data["textures"][element] and json_data["textures"][element]["height"] != height :
                warnings.append( "WARNING: Texture \"" + name + "\" has entry in index, with a height: "+str( json_data["textures"][element]["height"] )+" != "+str(height )+"." )
              else :
                json_data["textures"][element]["width"] = width
                json_data["textures"][element]["height"] = height
            else :
              new_textures += 1
              json_data["textures"][name] = { "path" : file_path, "width" : width, "height" : height, "rows" : 1, "columns" : 1 }
          continue


        elif extension in MUSIC_EXTENSIONS :
          if name in json_data["music"] :
            if "path" not in json_data["music"][name] :
              warnings.append( "WARNING: Music \"" + name + "\" has entry in index, but no \"path\" value. Fixed!" )
              json_data["music"][name]["path"] = file_path

            elif json_data["music"][name]["path"] != file_path :
              warnings.append( "WARNING: Music \"" + name + "\" has entry in index, with a different file path." )

          else :
            element = search( json_data["music"], "path", file_path )
            if element != None :
              warnings.append( "WARNING: Music path \""+file_path+"\" found associated with differently named music: "+element+"." );
            else :
              new_music += 1
              json_data["music"][name] = { "path" : file_path }
          continue


        elif extension in SOUND_EXTENSIONS :
          if name in json_data["sounds"] :
            if "path" not in json_data["sounds"][name] :
              warnings.append( "WARNING: Sound \"" + name + "\" has entry in index, but no \"path\" value. Fixed!" )
              json_data["sounds"][name]["path"] = file_path
              
            elif json_data["sounds"][name]["path"] != file_path :
              warnings.append( "WARNING: Sound \"" + name + "\" has entry in index, with a different file path." )

          else :
            element = search( json_data["sounds"], "path", file_path )
            if element != None :
              warnings.append( "WARNING: Sound path \""+file_path+"\" found associated with differently named sound: "+element+"." );
            else :
              new_sounds += 1
              json_data["sounds"][name] = { "path" : file_path }
          continue


        elif extension in FONT_EXTENSIONS :
          if name in json_data["fonts"] :
            if "path" not in json_data["fonts"][name] :
              warnings.append( "WARNING: Font \"" + name + "\" has entry in index, but no \"path\" value. Fixed!" )
              json_data["fonts"][name]["path"] = file_path
              
            elif json_data["fonts"][name]["path"] != file_path :
              warnings.append( "WARNING: Font \"" + name + "\" has entry in index, with a different file path." )

          else :
            element = search( json_data["fonts"], "path", file_path )
            if element != None :
              warnings.append( "WARNING: Font path \""+file_path+"\" found associated with differently named font: "+element+"." );
            else :
              warnings.append( "WARNING: Font \"" + name + "\" added with path \""+file_path+"\". Please provide a size!" )
              new_fonts += 1
              json_data["fonts"][name] = { "path" : file_path }
          continue


#  Validate fonts, sounds, music

  for font_name in json_data["fonts"].keys() :
    font_data = json_data["fonts"][font_name]

    if "path" not in font_data :
      warnings.append( "WARNING: Font \"" + name + "\" has entry in index, but no \"path\" value and no associated file. Removed!" )
      del json_data["fonts"][font_name]

    if not os.path.isfile( font_data["path"] ) :
      warnings.append( "WARNING: Font \"" + name + "\" has entry in index, but file does not exist: "+font_data["path"]+". Removed!" )
      del json_data["fonts"][font_name]

    total_fonts += 1



  for music_name in json_data["music"].keys() :
    music_data = json_data["music"][music_name]

    if "path" not in music_data :
      warnings.append( "WARNING: Music \"" + name + "\" has entry in index, but no \"path\" value and no associated file. Removed!" )
      del json_data["music"][music_name]

    if not os.path.isfile( music_data["path"] ) :
      warnings.append( "WARNING: Music \"" + name + "\" has entry in index, but file does not exist: "+music_data["path"]+". Removed!" )
      del json_data["music"][music_name]

    total_music += 1



  for sound_name in json_data["sounds"].keys() :
    sound_data = json_data["sounds"][sound_name]

    if "path" not in sound_data :
      warnings.append( "WARNING: Sound \"" + name + "\" has entry in index, but no \"path\" value and no associated file. Removed!" )
      del json_data["sounds"][sound_name]

    if not os.path.isfile( sound_data["path"] ) :
      warnings.append( "WARNING: Sound \"" + name + "\" has entry in index, but file does not exist: "+sound_data["path"]+". Removed!" )
      del json_data["sounds"][sound_name]

    total_sounds += 1



  for texture_name in json_data["textures"].keys() :
    texture_data = json_data["textures"][texture_name]

    if "path" not in texture_data :
      warnings.append( "WARNING: Texture \"" + name + "\" has entry in index, but no \"path\" value and no associated file. Removed!" )
      del json_data["textures"][texture_name]

    if not os.path.isfile( texture_data["path"] ) :
      warnings.append( "WARNING: Texture \"" + name + "\" has entry in index, but file does not exist: "+texture_data["path"]+". Removed!" )
      del json_data["textures"][texture_name]

    elif "width" not in texture_data :
      warnings.append( "WARNING: Texture \"" + name + "\" has entry in index, but no \"width\" value and no associated file. Removed!" )
      del json_data["textures"][texture_name]

    elif "height" not in texture_data :
      warnings.append( "WARNING: Texture \"" + name + "\" has entry in index, but no \"height\" value and no associated file. Removed!" )
      del json_data["textures"][texture_name]

    total_textures += 1



  font_dict = json_data["fonts"]

  for string_name in json_data["strings"].keys() :
    string_data = json_data["strings"][string_name]

    if "font" not in string_data :
      warnings.append( "WARNING: String \"", string_name, "\" has no \"font\" value." )

    elif "text" not in string_data :
      warnings.append( "WARNING: String \"", string_name, "\" has no \"text\" value." )

    elif "colour" not in string_data :
      warnings.append( "WARNING: String \"", string_name, "\" has no \"colour\" value." )

    elif font_name not in font_dict :
      warnings.append( "WARNING: Font \"", font_name, "\" required by text string \"", string_name, "\" not found." )

    else :
      total_strings += 1


  print( "Found", len( warnings ), "warnings:" )
  for warning in warnings :
    print( warning )
  print()
  print( "Found", new_textures, "new texture files. Total =", total_textures )
  print( "Found", new_fonts, "new font files. Total =", total_fonts )
  print( "Found", new_sounds, "new sound files. Total =", total_sounds )
  print( "Found", new_music, "new music files. Total =", total_music )
  print( "Found", total_strings, "string configurations." )
  print()
  print( "Index saved to file:", namespace.file )

  with open( namespace.file, 'w' ) as json_file :
    json_file.write( format_index( json.dumps( json_data ) ) )
#  print( format_index( json.dumps( json_data ) ) )

  sdlttf.TTF_Quit()

