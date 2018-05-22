#!/bin/sh

/opt/rcagent/nas_service &
while true;
do
    /opt/rcagent/cgi_processor
done
