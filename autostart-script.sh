#!/bin/sh

### BEGIN INIT INFO
# Provides:     resque
# Required-Sart: $remote_fs $syslog
# Required-Stop: $remote_fs $syslog
# Default-Start: 2 3 4 5
# Default-Stop: 0 1 6
# Short-Description: resque service managment
# Description: Start, stop, restart resque service for a specific application.
### END INIT INFO
set -e

APP_ROOT=/home/deploy/sardjv/current
PID=/home/deploy/sardjv/current/resque.pid
CMD="cd /home/deploy/sardjv/current; PIDFILE=./resque.pid BACKGROUND=yes QUEUE=file_serve rake environment resque:work"
AS_USER=deploy
set -u

sig () {
  test -s "$PID" && kill -$1 `cat $PID`
}

run () {
  if [ "$(id -un)" = "$AS_USER" ]; then
    eval $1
  else
    su -c "$1" - $AS_USER
  fi
}

case "$1" in
start)
  sig 0 && echo >&2 "Already running" && exit 0
  run "$CMD"
  ;;
status)
  sig 0 && echo >&2 "running" && exit 0
  "stopped"
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

