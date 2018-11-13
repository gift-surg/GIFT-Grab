#!/usr/bin/env bash

# Convenience script providing a higher-level CLI
# for running tests selectively

THIS_SCRIPT="$(basename "$(test -L "${BASH_SOURCE[0]}" && readlink "$0" || echo "$0")")"

args_ok=true
test_colour_space=""
test_port=""
test_codec=""
test_file_extension=""
test_cmd="pytest"
test_dir="$GiftGrab_SOURCE_DIR/tests"
test_file_frame_rate=30
test_file_frame_count=15
test_file_frame_width=128
test_file_frame_height=64


# Function for parsing passed CLI argument for colour space
function parse_colour()
{
    if [ "$1" = "bgra" ] || [ "$1" = "i420" ] || [ "$1" = "uyvy" ]; then
        test_colour_space=$1
    else
        args_ok=false
    fi
}


# Function for parsing passed CLI argument for Epiphan device port
function parse_port()
{
    if [ "$1" = "dvi" ] || [ "$1" = "sdi" ]; then
        test_port=$1
    else
        args_ok=false
    fi
}


# Function for parsing passed CLI argument for codec
function parse_codec()
{
    if [ "$1" = "hevc" ]; then
        test_file_extension="mp4"
    elif [ "$1" = "xvid" ]; then
        test_file_extension="avi"
    elif [ "$1" = "vp9" ]; then
        test_file_extension="webm"
    else
        args_ok=false
    fi

    if [ "$args_ok" = true ]; then
        test_codec=$1
    fi
}


# Compose actual command to run based on CLI arguments
if [ $# -lt 1 ]; then
    args_ok=false
elif [ "$1" = "encode" ] || [ "$1" = "decode" ];then
    if [ $# -ne "3" ]; then
        args_ok=false
    else
        parse_codec $2
        parse_colour $3
        if [ "$args_ok" = true ]; then
            test_cmd="$test_cmd --colour-space=$test_colour_space"
            if [ "$1" = "encode" ]; then
                test_cmd="$test_cmd --codec=$test_codec $test_dir/target"
            else  # decode
                test_file="$test_dir/files/data"
                test_file="$test_file/video_${test_file_frame_count}frames"
                test_file="${test_file}_${test_file_frame_rate}fps"
                test_file="${test_file}.${test_file_extension}"
                test_cmd="$test_cmd --filepath=$test_file"
                test_cmd="$test_cmd --frame-rate=$test_file_frame_rate"
                test_cmd="$test_cmd --frame-count=$test_file_frame_count"
                test_cmd="$test_cmd --frame-width=$test_file_frame_width"
                test_cmd="$test_cmd --frame-height=$test_file_frame_height"
                test_cmd="$test_cmd $test_dir/files -m observer_pattern"
            fi
        fi
    fi
elif [ "$1" = "numpy" ]; then
    if [ $# -ne "2" ]; then
        args_ok=false
    else
        parse_colour $2
        if [ "$test_colour_space" = "uyvy" ]; then
            args_ok=false
        else
            test_cmd="$test_cmd --colour-space=$test_colour_space"
            test_cmd="$test_cmd $test_dir/videoframe -m numpy_compatibility"
        fi
    fi
elif [ "$1" = "epiphan-dvi2pcieduo" ]; then
    test_device=$1
    test_device=${test_device:8}
    if [ $# -ne "3" ]; then
        args_ok=false
    else
        parse_colour $2
        parse_port $3
        test_cmd="$test_cmd --colour-space=$test_colour_space"
        test_cmd="$test_cmd --port=$test_port"
        test_cmd_working_dir="$test_dir/epiphan/dvi2pcieduo"
        test_cmd_unit="$test_cmd $test_cmd_working_dir -m unit"
        test_cmd_observer="$test_cmd --frame-rate=27 --observers=3"
        test_cmd_observer="$test_cmd_observer $test_cmd_working_dir -m observer_pattern"
        test_cmd="$test_cmd_unit && $test_cmd_observer"
    fi
elif [ "$1" = "network" ]; then
    printf "$1 option not implemented yet\n"  # TODO
    args_ok=false
elif [ "$1" = "blackmagic-decklinksdi4k" ] || [ "$1" = "blackmagic-decklink4kextreme12g" ]; then
    test_device=$1
    test_device=${test_device:11}
    if [ $# -ne "2" ]; then
        args_ok=false
    else
        if [ "$1" = "blackmagic-decklinksdi4k" ]; then
            frame_rate=27
        elif [ "$1" = "blackmagic-decklink4kextreme12g" ]; then
            frame_rate=24
        fi
        parse_colour $2
        test_cmd="$test_cmd --device=$test_device"
        test_cmd="$test_cmd --colour-space=$test_colour_space"
        test_cmd_working_dir="$test_dir/blackmagic"
        test_cmd_unit="$test_cmd $test_cmd_working_dir -m unit"
        test_cmd_observer="$test_cmd --frame-rate=$frame_rate --observers=3"
        test_cmd_observer="$test_cmd_observer $test_cmd_working_dir -m observer_pattern"
        if [ "$1" = "blackmagic-decklink4kextreme12g" ]; then
            test_cmd_stereo="$test_cmd -m stereo_frames"
        fi
        test_cmd="$test_cmd_unit && $test_cmd_observer"
        if [ "$1" = "blackmagic-decklink4kextreme12g" ]; then
            test_cmd="$test_cmd && $test_cmd_stereo"
        fi
    fi
else
    args_ok=false
fi


# Display synopsis in case there is a problem
if [ "$args_ok" != true ];
then
    printf "Usage:\t${THIS_SCRIPT} encode | decode CODEC COLOUR\n"
    printf "\tCODEC should be one of hevc, xvid, or vp9\n"
    printf " or:\tCOLOUR should be one of bgra, i420, or uyvy\n"
    printf " or:\t${THIS_SCRIPT} numpy bgra | i420\n"
    printf " or:\t${THIS_SCRIPT} epiphan-dvi2pcieduo bgra | i420 dvi | sdi\n"
    printf " or:\t${THIS_SCRIPT} network\n"
    printf " or:\t${THIS_SCRIPT} blackmagic-decklinksdi4k uyvy\n"
    printf " or:\t${THIS_SCRIPT} blackmagic-decklink4kextreme12g bgra|uyvy\n"
    exit 1
fi


# Run the command, as everything seems fine
echo "Will run: $test_cmd"
eval $test_cmd
exit_code=$?
echo "Exit code was: $exit_code"
exit $exit_code
