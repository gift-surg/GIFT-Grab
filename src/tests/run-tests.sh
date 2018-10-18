#!/usr/bin/env bash

# Convenience script providing a higher-level CLI
# for running tests selectively

args_ok=true
this_colour=""
this_codec=""
this_file_extension=""

function parse_colour()
{
    if [ "$1" = "bgra" ] || [ "$1" = "i420" ] || [ "$1" = "uyvy" ]; then
        this_colour=$1
    else
        args_ok=false
    fi
}

function parse_codec()
{
    if [ "$1" = "hevc" ]; then
        this_file_extension="mp4"
    elif [ "$1" = "xvid" ]; then
        this_file_extension="avi"
    elif [ "$1" = "vp9" ]; then
        this_file_extension="webm"
    else
        args_ok=false
    fi

    if [ "$args_ok" = true ]; then
        this_codec=$1
    fi
}

THIS_SCRIPT="$(basename "$(test -L "${BASH_SOURCE[0]}" && readlink "$0" || echo "$0")")"

if [ $# -lt 1 ]; then
    args_ok=false
elif [ "$1" = "encode" ] || [ "$1" = "decode" ];then
    if [ $# -ne "3" ]; then
        args_ok=false
    else
        parse_codec $2
        parse_colour $3
    fi
elif [ "$1" = "numpy" ]; then
    if [ $# -ne "2" ]; then
        args_ok=false
    else
        parse_colour $2
    fi
elif [ "$1" = "epiphan-dvi2pcieduo" ]; then
    printf "$1 option not implemented yet\n"  # TODO
elif [ "$1" = "network" ]; then
    printf "$1 option not implemented yet\n"  # TODO
elif [ "$1" = "blackmagic-decklinksdi4k" ]; then
    printf "$1 option not implemented yet\n"  # TODO
else
    args_ok=false
fi

if [ "$args_ok" != true ];
then
    printf "Usage:\t${THIS_SCRIPT} encode | decode CODEC COLOUR\n"
    printf "\tCODEC should be one of hevc, xvid, or vp9\n"
    printf " or:\tCOLOUR should be one of bgra, i420, or uyvy\n"
    printf " or:\t${THIS_SCRIPT} numpy bgra | i420\n"
    printf " or:\t${THIS_SCRIPT} epiphan-dvi2pcieduo bgra | i420\n"
    printf " or:\t${THIS_SCRIPT} network\n"
    printf " or:\t${THIS_SCRIPT} blackmagic-decklinksdi4k\n"
    exit 1
fi