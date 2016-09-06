* [x265](http://x265.org/)

x265
----

Installation instructions:

1. `hg clone https://bitbucket.org/multicoreware/x265`
1. `cd x265`
1. `hg checkout 1.9`
1. `cd ..` and `mkdir x265-build` and `cd x265-build`
1. `cmake -D ENABLE_SHARED:bool=on ../x265/source/`
1. `make -j` and `make install`

FFmpeg
------

1. If using x265: `../FFmpeg/configure --enable-shared --enable-avresample --enable-libx265 --enable-gpl --enable-muxer=mp4`
