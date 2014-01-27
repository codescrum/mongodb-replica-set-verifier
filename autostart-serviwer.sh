#!/bin/sh

### BEGIN INIT INFO
# Provides:     serviwer
# Required-Sart: $syslog
# Required-Stop: $remote_fs $syslog
# Default-Start: 2 3 4 5
# Default-Stop: 0 1 6
# Short-Description: serviwer service managment
# Description: Start, stop, restart serviwer service.
### END INIT INFO
set -e

PID=/tmp/serviwer.pid
CMD="cd /opt/replica_set_verifier; ./serviwer.sh"
AS_USER=deploy
set -u

sig () {
  test -s "$PID" && kill -$1 `cat $PID`
}

run () {
  su -c "$1"
}

case "$1" in
start)
  sig 0 && echo >&2 "Already running" && exit 0
  run "$CMD"
  ;;
status)
  sig 0 && echo >&2 "running" && exit 0
  echo >&2 "Stopped"
  ;;
stop)
  sig QUIT && exit 0
  echo >&2 "Not running"
  ;;
force-stop)
  sig TERM && exit 0
  echo >&2 "Not running"
  ;;
restart|reload)
  sig USR2 && echo reloaded OK && exit 0
  echo >&2 "Couldn't reload, starting '$CMD' instead"
  run "$CMD"
  ;;
*)
  echo >&2 "Usage: $0 <start|stop|restart|force-stop>"
  exit 1
  ;;
esac

