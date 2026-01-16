#!/bin/bash
set -e

# Start fluxbox in the background
fluxbox &
FLUXBOX_PID=$!
sleep 1

# Start the application
./src/leafpad &
APP_PID=$!
sleep 1

# Find the window ID
WINDOW_ID=$(xdotool search --pid $APP_PID | head -1)

# Activate the window
xdotool windowactivate $WINDOW_ID

# Open the "Options" menu
xdotool key alt+o
sleep 0.5

# Select "Line Wrap"
xdotool key l
sleep 0.5

# Type a long line of text
xdotool type "This is a very long line of text that should wrap around the screen if the line wrap feature is working correctly, but it should not be saved with newlines."
sleep 0.5

# Save the file
xdotool key ctrl+s
sleep 0.5
xdotool type "test_output.txt"
sleep 0.5
xdotool key Return
sleep 0.5

# Quit the application
xdotool key ctrl+q

# Kill the background processes
kill $APP_PID
kill $FLUXBOX_PID
