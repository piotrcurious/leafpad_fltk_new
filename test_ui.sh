#!/bin/bash
# Create a test file with 100 lines
for i in {1..100}; do
    echo "This is line $i" >> test_file.txt
done

# Start fluxbox and leafpad in the background
fluxbox &
FLUXBOX_PID=$!
./leafpad_fltk_0.23/src/leafpad test_file.txt > leafpad.log 2>&1 &
LEAFPAD_PID=$!

# Wait for the main window to appear using xwininfo
WINDOW_ID=""
ATTEMPTS=0
echo "Searching for Leafpad window..."
while [ -z "$WINDOW_ID" ] && [ $ATTEMPTS -lt 15 ]; do
    sleep 1
    # xwininfo is often more reliable for finding windows than xdotool search
    WINDOW_ID=$(xwininfo -root -tree | grep "test_file.txt" | awk '{print $1}')
    ATTEMPTS=$((ATTEMPTS + 1))
done

if [ -z "$WINDOW_ID" ]; then
    echo "Leafpad window did not appear."
    echo "--- leafpad.log ---"
    cat leafpad.log
    echo "-------------------"
else
    echo "Leafpad window found: $WINDOW_ID"

    # Take initial screenshot
    scrot screenshot_initial.png
    echo "Initial screenshot captured."

    # Test Search Widget (Ctrl+F)
    xdotool windowactivate $WINDOW_ID
    xdotool key --window $WINDOW_ID "ctrl+f"
    sleep 1
    scrot screenshot_search_widget.png
    echo "Search widget screenshot captured."

    # Test Jump to Line (Ctrl+J)
    xdotool windowactivate $WINDOW_ID
    xdotool key --window $WINDOW_ID "ctrl+j"
    sleep 1

    # Wait for the "Jump to line:" dialog and type "50"
    JUMP_WINDOW_ID=$(xdotool search --onlyvisible --name "Jump to line:")
    if [ -n "$JUMP_WINDOW_ID" ]; then
        xdotool type --window $JUMP_WINDOW_ID "50"
        xdotool key --window $JUMP_WINDOW_ID "Return"
        sleep 1
        scrot screenshot_jump.png
        echo "Jump to line screenshot captured."
    else
        echo "Jump to line dialog not found."
        scrot screenshot_jump_failed.png
    fi
fi

# Cleanup
kill $LEAFPAD_PID
kill $FLUXBOX_PID
rm -f test_file.txt leafpad.log
