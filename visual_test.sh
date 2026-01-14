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

# Type some text
xdotool type "line 1
line 2
line 3
line 4
line 5
line 6
line 7
line 8
line 9
line 10
line 11
line 12
line 13
line 14
line 15"
sleep 0.5

# Show line numbers
xdotool key alt+o
sleep 0.5
xdotool key n
sleep 0.5

# Scroll down
xdotool key Page_Down
sleep 0.5

# Take a screenshot
scrot /tmp/screenshot.png

# Quit the application
xdotool key ctrl+q

# Kill the background processes
kill $APP_PID
kill $FLUXBOX_PID
EOF
