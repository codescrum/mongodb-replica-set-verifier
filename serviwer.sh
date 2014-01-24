#!/bin/sh
READY=false
TOKEN=true
while : ; do
    sleep 1
    if grep -Fxq "$TOKEN" /tmp/token.rsv
	then
	    READY=true
	else
	    READY=false
	fi
	echo $READY
    ! $READY || break
done
echo "" > /tmp/token.rsv
echo "ready to launch.."
service unicorn_sardjv start
service resque start
