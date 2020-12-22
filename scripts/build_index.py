#!/bin/env python3

import shutil
import os
import argparse
import json

import sdl2
import sdl2.ext as sdlext
import sdl2.sdlttf as sdlttf

IMAGE_EXTENSIONS = sdlext.get_image_formats()
AUDIO_EXTENSIONS = [ "mp3", "wav" ]
FONT_EXTENSIONS = [ "ttf" ]
TEXT_EXTENSIONS = [ "txt" ]

IMAGE_KEYS = [ "path", "width", "height", "colour_key", "rows", "columns" ]
TEXT_KEYS = [ "path" ]
AUDIO_KEYS = [ "path" ]
FONTS_KEYS = [ "path" ]


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
  new_images = 0
  new_audio = 0
  new_fonts = 0
  new_text = 0

  total_images = 0
  total_audio = 0
  total_fonts = 0
  total_text = 0

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
  if "audio" not in json_data :
    json_data["audio"] = {}
  if "text" not in json_data :
    json_data["text"] = {}
  if "images" not in json_data :
    json_data["images"] = {}
   

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

          if name in json_data["images"] :
            if "path" not in json_data["images"][name] :
              warnings.append( "WARNING: Image \"" + name + "\" has entry in index, but no \"path\" value. Fixed!" )
              json_data["images"][name]["path"] = file_path

            elif json_data["images"][name]["path"] != file_path :
              warnings.append( "WARNING: Images \"" + name + "\" has entry in index, with a different file path." )
              continue

            if "rows" not in json_data["images"][name] :
              json_data["images"][name]["rows"] = 1
            if "columns" not in json_data["images"][name] :
              json_data["images"][name]["columns"] = 1

            json_data["images"][name]["width"] = width
            json_data["images"][name]["height"] = height

          else :
            element = search( json_data["images"], "path", file_path )
            if element != None :
              warnings.append( "WARNING: Image path \""+file_path+"\" found associated with differently named texture: "+element+"." );

              if "rows" not in json_data["images"][element] :
                json_data["images"][element]["rows"] = 1
              if "columns" not in json_data["images"][element] :
                json_data["images"][element]["columns"] = 1

              if "width" in json_data["images"][element] and json_data["images"][element]["width"] != width :
                warnings.append( "WARNING: Image \"" + name + "\" has entry in index, with a width: "+str( json_data["images"][element]["width"] )+" != "+str(width )+"." )
              elif "height" in json_data["images"][element] and json_data["images"][element]["height"] != height :
                warnings.append( "WARNING: Image \"" + name + "\" has entry in index, with a height: "+str( json_data["images"][element]["height"] )+" != "+str(height )+"." )
              else :
                json_data["images"][element]["width"] = width
                json_data["images"][element]["height"] = height
            else :
              new_images += 1
              json_data["images"][name] = { "path" : file_path, "width" : width, "height" : height, "rows" : 1, "columns" : 1 }
          continue


        elif extension in AUDIO_EXTENSIONS :
          if name in json_data["audio"] :
            if "path" not in json_data["audio"][name] :
              warnings.append( "WARNING: Audio \"" + name + "\" has entry in index, but no \"path\" value. Fixed!" )
              json_data["audio"][name]["path"] = file_path

            elif json_data["audio"][name]["path"] != file_path :
              warnings.append( "WARNING: Audio \"" + name + "\" has entry in index, with a different file path." )

          else :
            element = search( json_data["audio"], "path", file_path )
            if element != None :
              warnings.append( "WARNING: Audio path \""+file_path+"\" found associated with differently named audio: "+element+"." );
            else :
              new_audio += 1
              json_data["audio"][name] = { "path" : file_path }
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
              new_fonts += 1
              json_data["fonts"][name] = { "path" : file_path }
          continue


        elif extension in TEXT_EXTENSIONS :
          if name in json_data["text"] :
            if "path" not in json_data["text"][name] :
              warnings.append( "WARNING: Text \"" + name + "\" has entry in index, but no \"path\" value. Fixed!" )
              json_data["text"][name]["path"] = file_path
              
            elif json_data["text"][name]["path"] != file_path :
              warnings.append( "WARNING: Text \"" + name + "\" has entry in index, with a different file path." )

          else :
            element = search( json_data["text"], "path", file_path )
            if element != None :
              warnings.append( "WARNING: Text path \""+file_path+"\" found associated with differently named text: "+element+"." );
            else :
              new_text += 1
              json_data["text"][name] = { "path" : file_path }
          continue


#  Validate fonts, sounds, music, text

  for font_name in json_data["fonts"].keys() :
    font_data = json_data["fonts"][font_name]

    if "path" not in font_data :
      warnings.append( "WARNING: Font \"" + font_name + "\" has entry in index, but no \"path\" value and no associated file. Removed!" )
      del json_data["fonts"][font_name]

    if not os.path.isfile( font_data["path"] ) :
      warnings.append( "WARNING: Font \"" + font_name + "\" has entry in index, but file does not exist: "+font_data["path"]+". Removed!" )
      del json_data["fonts"][font_name]

    total_fonts += 1



  for audio_name in json_data["audio"].keys() :
    audio_data = json_data["audio"][audio_name]

    if "path" not in audio_data :
      warnings.append( "WARNING: Audio \"" + audio_name + "\" has entry in index, but no \"path\" value and no associated file. Removed!" )
      del json_data["audio"][audio_name]

    if not os.path.isfile( audio_data["path"] ) :
      warnings.append( "WARNING: Audio \"" + audio_name + "\" has entry in index, but file does not exist: "+audio_data["path"]+". Removed!" )
      del json_data["audio"][audio_name]

    total_audio += 1



  for image_name in json_data["images"].keys() :
    image_data = json_data["images"][image_name]

    if "path" not in image_data :
      warnings.append( "WARNING: Image \"" + image_name + "\" has entry in index, but no \"path\" value and no associated file. Removed!" )
      del json_data["images"][image_name]

    if not os.path.isfile( image_data["path"] ) :
      warnings.append( "WARNING: Image \"" + image_name + "\" has entry in index, but file does not exist: "+image_data["path"]+". Removed!" )
      del json_data["images"][image_name]

    elif "width" not in image_data :
      warnings.append( "WARNING: Image \"" + image_name + "\" has entry in index, but no \"width\" value and no associated file. Removed!" )
      del json_data["images"][image_name]

    elif "height" not in image_data :
      warnings.append( "WARNING: Image \"" + image_name + "\" has entry in index, but no \"height\" value and no associated file. Removed!" )
      del json_data["images"][image_name]

    total_images += 1



  for text_name in json_data["text"].keys() :
    text_data = json_data["text"][string_name]

    if "path" not in text_data :
      warnings.append( "WARNING: Text \"" + text_name + "\" has entry in index, but no \"path\" value and no associated file. Removed!" )
      del json_data["text"][text_name]

    if not os.path.isfile( text_data["path"] ) :
      warnings.append( "WARNING: Text \"" + text_name + "\" has entry in index, but file does not exist: "+text_data["path"]+". Removed!" )
      del json_data["text"][text_name]

    else :
      total_text += 1


  print( "Found", len( warnings ), "warnings:" )
  for warning in warnings :
    print( warning )
  print()
  print( "Found", new_images, "new image files. Total =", total_images )
  print( "Found", new_fonts, "new font files. Total =", total_fonts )
  print( "Found", new_audio, "new audio files. Total =", total_audio )
  print( "Found", new_text, "new text files. Total =", total_text )
  print()
  print( "Index saved to file:", namespace.file )

  with open( namespace.file, 'w' ) as json_file :
    json_file.write( format_index( json.dumps( json_data ) ) )
#  print( format_index( json.dumps( json_data ) ) )

  sdlttf.TTF_Quit()

