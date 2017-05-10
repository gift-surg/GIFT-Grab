#!/usr/bin/env bash

# so that CI script will fail when e.g. previous command succeeds:
function exit_on_success
{
    exit_status=$?
    if [[ "$exit_status" -eq "0" ]]; then
        echo "Build failed due to last exit status being $exit_status"
        exit 1
    fi
}

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
