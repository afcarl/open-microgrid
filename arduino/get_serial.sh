#!/bin/bash
set -o nounset
set -o errexit

if [[ $# != 2 ]]
then
    echo "Two arguments expected: ssh server, device number"
    exit 1
fi

ssh -t $1 "ino serial -p /dev/arduino$2" -b 57600
echo "$(tput setaf 1)Now going to sleep for 10 seconds to remind you that you will fuck \
it up if you start uploading code too early.$(tput sgr0)"
sleep 10s