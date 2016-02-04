#!/bin/sh
pidfile=/var/run/btntrigger_1.pid

# kill previous instance and exit
[ -f "$pidfile" ] && { kill `cat "$pidfile"` && unlink "$pidfile" && exit 0; }

# mpg123 for mpeg layer 1,2,3; repeat playlist indefinitely
# doesnt support .m3u format
mpg123 -q --random -@ "/root/playlist/1.m3u" &
echo -n "$!" >"$pidfile"
