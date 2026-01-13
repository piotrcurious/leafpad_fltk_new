#!/bin/bash
set -e

xvfb-run --auto-servernum --server-args="-screen 0 1024x768x24" /bin/bash <<'EOF'
# Start the application
leafpad_fltk_0.2vibe_wrap/src/leafpad &
APP_PID=$!
sleep 3

# Check for the window
xwininfo -root -tree > /tmp/xwininfo.log

# Kill the background processes
kill $APP_PID
EOF
