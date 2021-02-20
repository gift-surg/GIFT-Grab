cmake_minimum_required(VERSION 3.13.0)

add_library(giftgrab_core)

target_sources(giftgrab_core
  PRIVATE
  src/broadcastdaemon.cpp
  src/except.cpp
  src/iobservable.cpp
  src/maskframe.cpp
  src/videoframe.cpp
  src/videosourcefactory.cpp
  src/videotargetfactory.cpp
  src/videoframe.cpp
  
  
  PUBLIC
  include/giftgrab/broadcastdaemon.h
  include/giftgrab/codec.h
  include/giftgrab/device.h
  include/giftgrab/except.h
  include/giftgrab/macros.h
  include/giftgrab/iobservable.h
  include/giftgrab/iobserver.h
  include/giftgrab/ivideosource.h
  include/giftgrab/ivideotarget.h
  include/giftgrab/maskframe.h
  include/giftgrab/videoframe.h
  include/giftgrab/videosourcefactory.h
  include/giftgrab/videotargetfactory.h
  
)

target_include_directories(giftgrab_core PUBLIC include PRIVATE src)
set_property(TARGET giftgrab_core PROPERTY CXX_STANDARD 11)

# include(GenerateExportHeader)
# generate_export_header(giftgrab)