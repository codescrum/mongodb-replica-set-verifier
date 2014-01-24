#!/bin/sh

trigger() {
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
	echo "$PID"
	echo "" > /tmp/token.rsv	
	echo "ready to launch.."
	service unicorn_sardjv start
	service resque start
}

trigger & 
PID=$!
echo "$PID" > /tmp/serviwer.pid