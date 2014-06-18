#!/bin/bash
set -o nounset
set -o errexit

DIR="$( pushd  "$( dirname "${BASH_SOURCE[0]}" )" > /dev/null && pwd && popd > /dev/null)"

if [[ $# == 0 ]]
then
    echo "First argument must be a box name (gridbox, rpi etc.)"
    exit 1
elif [[ $# == 1 ]]
then
    TARGET_BOX=$1
    DEVICE="ALL"
elif [[ $# == 2 ]]
then
    TARGET_BOX=$1
    DEVICE=$2
else
    echo "Too many arguments"
    exit 1
fi

pushd $DIR/code
ino build -m leonardo
popd

rsync -ar --delete --itemize-changes --exclude=.build/environment.pickle --delete-excluded --human-readable --progress -e ssh $DIR/code/ "$TARGET_BOX:~/.arduino_code"
rsync -ar $DIR/reset-arduino.py "$TARGET_BOX:~/reset-arduino.py"


if [[ $DEVICE == "ALL" ]]
then
    ssh $TARGET_BOX '
        set -o nounset
        set -o errexit
        cd .arduino_code
        ino build -m leonardo
        if [[ $(hostname) == "raspberrypi" ]]
        then
            SLEEP_TIME=3
            echo "special treatment for retarded rpi"
        else
            SLEEP_TIME=2
        fi
        for x in /dev/arduino*
        do 
            echo "uploading code to $x (reset after ${SLEEP_TIME}s)"
            sleep ${SLEEP_TIME}s && python ~/reset-arduino.py $x &
            ino upload -m leonardo -p $x || {
                echo "ERROR uploading to $x!!!!"
            }
            sleep 3s
        done'
else
    ssh $TARGET_BOX '
        set -o nounset
        set -o errexit
        cd .arduino_code
        ino build -m leonardo
        if [[ $(hostname) == "raspberrypi" ]]
        then
            SLEEP_TIME=3
            echo "special treatment for retarded rpi"
        else
            SLEEP_TIME=2
        fi
        for x in /dev/arduino'$DEVICE'
        do 
            echo "uploading code to $x (reset after ${SLEEP_TIME}s)"
            sleep ${SLEEP_TIME}s && python ~/reset-arduino.py $x &
            ino upload -m leonardo -p $x
        done'
fi