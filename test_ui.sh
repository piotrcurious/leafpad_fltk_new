#!/bin/bash
fluxbox &
./leafpad_fltk_0.23/src/leafpad &
sleep 2

WINDOW_ID=$(xwininfo -root -tree | grep "Untitled - Leafpad" | awk '{print $1}')

if [ -n "$WINDOW_ID" ]; then
    xdotool windowactivate $WINDOW_ID
    xdotool key ctrl+f
    sleep 1
    scrot test.png
else
    echo "Window not found"
fi

pkill leafpad
pkill fluxbox
