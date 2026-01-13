#!/bin/bash
set -e

xvfb-run --auto-servernum --server-args="-screen 0 1024x768x24" /bin/bash <<'EOF'
# Start fluxbox in the background
fluxbox &
FLUXBOX_PID=$!
sleep 1

# Start the application
leafpad_fltk_0.2vibe_wrap/src/leafpad &
APP_PID=$!

# Wait for the window to appear
timeout=10
start_time=$(date +%s)
WINDOW_ID=""
while [ -z "$WINDOW_ID" ]; do
    WINDOW_ID=$(xdotool search --pid $APP_PID 2>/dev/null | head -1)
    current_time=$(date +%s)
    elapsed_time=$((current_time - start_time))
    if [ $elapsed_time -ge $timeout ]; then
        echo "Timeout: Window not found."
        exit 1
    fi
    sleep 0.5
done

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
EOF
