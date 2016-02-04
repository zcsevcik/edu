#!/bin/sh
pidfile=/var/run/btntrigger_2.pid

# kill previous instance and exit
[ -f "$pidfile" ] && { kill `cat "$pidfile"` && unlink "$pidfile" && exit 0; }

# ogg123 supports flac, speex, ogg vorbis
ogg123 -q --random --repeat -@ "/root/playlist/2.m3u" &
echo -n "$!" >"$pidfile"
