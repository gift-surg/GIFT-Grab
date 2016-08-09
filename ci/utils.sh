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