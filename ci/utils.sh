#!/usr/bin/env bash

# so that CI script will fail when e.g. make -j fails:
function exit_on_fail
{
    exit_status=$?
    if [[ "$exit_status" -ne "0" ]]; then
        echo "Build failed due to last exit status being $exit_status"
        exit $exit_status
    fi
}

function check_epiphan_alive
{
    if [ $(v2u | grep --count "no signal detected") -gt 0 ]; then
        echo "No alive device seems to be connected to Epiphan"
        echo "Please connect both ports of Storz, and retry build"
        exit 1
    fi
}

function check_epiphan_pearl_alive
{
    if [ -n "$EPIPHAN_PEARL_IP" ]; then
        echo "EPIPHAN_PEARL_IP not set"
        exit 1
    fi

    # TODO: v2u --address 128.16..... does not work,
    # so using ping instead
    ping -c 5 -v -W 5 $EPIPHAN_PEARL_IP

    exit_status=$?
    if [[ "$exit_status" -ne "0" ]]; then
        echo "Could not get data from Epiphan Pearl located at $EPIPHAN_PEARL_IP"
        echo "Please make sure Epiphan Pearl is connected properly, and retry build"
        exit 1
    fi
}
