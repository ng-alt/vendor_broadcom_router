#!/bin/sh

. /opt/broken/comm.sh

if [ -z "$1" ]; then
    echo "Please input alias name"
    exit $ERROR
fi

do_updatealias $1
