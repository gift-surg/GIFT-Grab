#!/usr/bin/env bash

# Convenience script providing a higher-level CLI
# for running tests selectively

THIS_SCRIPT="$(basename "$(test -L "${BASH_SOURCE[0]}" && readlink "$0" || echo "$0")")"

args_ok=true
if [ $# -lt 1 ]; then
    args_ok=false
elif [ "$1" = "encode" ] || [ "$1" = "decode" ];then
    if [ $# -ne "3" ]; then
        args_ok=false
    else
        echo $2 $3  # TODO
    fi
elif [ "$1" = "numpy" ]; then
    if [ $# -ne "2" ]; then
        args_ok=false
    else
        echo $2  # TODO
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