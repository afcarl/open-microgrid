#!/bin/bash
set -o nounset
set -o errexit

if [[ $# != 1 ]]
then
    echo "one argument expected: ssh server"
    exit 1
fi

DIR="$( pushd  "$( dirname "${BASH_SOURCE[0]}" )" > /dev/null && pwd && popd > /dev/null)"

rsync -ar --delete --exclude=.build --itemize-changes  --human-readable --progress -e ssh code/ $1:~/.arduino_code
ssh $1 '
    set -o nounset
    set -o errexit
    cd .arduino_code
    ino build -m leonardo
    for x in /dev/arduino*
    do 
        echo "uploading code to $x"
        ino upload -m leonardo -p $x
        sleep 5s
    done'