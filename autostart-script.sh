#!/bin/sh

### BEGIN INIT INFO
# Provides:     replica_set_verifier
# Required-Sart: $remote_fs $syslog
# Required-Stop: $remote_fs $syslog
# Default-Start: 2 3 4 5
# Default-Stop: 0 1 6
# Short-Description: replica_set_verifier service managment
# Description: Start, stop, restart replica_set_verifier service.
### END INIT INFO
set -e

PID=/tmp/replica_set_verifier.pid
CMD="/opt/replica_set_verifier/replica_set_verifier"
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

