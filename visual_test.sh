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
line 15
line 16
line 17
line 18
line 19
line 20
line 21
line 22
line 23
line 24
line 25
line 26
line 27
line 28
line 29
line 30
line 31
line 32
line 33
line 34
line 35
line 36
line 37
line 38
line 39
line 40
line 41
line 42
line 43
line 44
line 45
line 46
line 47
line 48
line 49
line 50
line 51
line 52
line 53
line 54
line 55
line 56
line 57
line 58
line 59
line 60
line 61
line 62
line 63
line 64
line 65
line 66
line 67
line 68
line 69
line 70
line 71
line 72
line 73
line 74
line 75
line 76
line 77
line 78
line 79
line 80
line 81
line 82
line 83
line 84
line 85
line 86
line 87
line 88
line 89
line 90
line 91
line 92
line 93
line 94
line 95
line 96
line 97
line 98
line 99
line 100"
sleep 0.5

# Show line numbers
xdotool key alt+o
sleep 0.5
xdotool key n
sleep 0.5

# Take a screenshot
scrot /tmp/screenshot1.png

# Type some more text
xdotool type "
line 101"
sleep 0.5

# Take another screenshot
scrot /tmp/screenshot2.png

# Quit the application
xdotool key ctrl+q

# Kill the background processes
kill $APP_PID
kill $FLUXBOX_PID
EOF
