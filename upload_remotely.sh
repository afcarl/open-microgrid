#!/bin/bash
set -o nounset
set -o errexit

if [[ $# != 2 ]]
then
    echo "Missing arguments: ssh server, device"
    exit 1
fi

DIR="$( pushd  "$( dirname "${BASH_SOURCE[0]}" )" > /dev/null && pwd && popd > /dev/null)"

rsync -ar --delete --exclude=.build --itemize-changes  --human-readable --progress -e ssh code/ $1:~/.arduino_code
ssh $1 '
  cd .arduino_code
  ino build -m leonardo
  ino upload -m leonardo -p $2'