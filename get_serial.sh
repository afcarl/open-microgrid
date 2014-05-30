#!/bin/bash
set -o nounset
set -o errexit

if [[ $# != 2 ]]
then
    echo "Two arguments expected: ssh server, device number"
    exit 1
fi

ssh -t $1 "ino serial -p /dev/arduino$2"

