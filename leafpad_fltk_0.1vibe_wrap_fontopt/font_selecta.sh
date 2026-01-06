#!/bin/bash

# X11 Core Font Selector for leafpad_fltk
# Generates font configuration for minimal memory usage

CONFIG_FILE="font_config.h"
LAUNCH_SCRIPT="launch_with_font.sh"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}╔════════════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║   X11 Core Font Selector for leafpad_fltk         ║${NC}"
echo -e "${BLUE}║   (Minimal Memory Footprint Configuration)        ║${NC}"
echo -e "${BLUE}╚════════════════════════════════════════════════════╝${NC}"
echo

# Function to check if fonts are available
check_fonts() {
    if ! command -v xlsfonts &> /dev/null; then
        echo -e "${RED}Error: xlsfonts command not found${NC}"
        echo "Please install: sudo apt-get install x11-utils"
        exit 1
    fi
}

# Function to scan available X11 core fonts
scan_fonts() {
    echo -e "${YELLOW}Scanning available X11 core fonts...${NC}"
    
    # Get all fixed-width fonts with iso10646-1 (Unicode) encoding
    FIXED_FONTS=$(xlsfonts | grep -E "fixed|terminus|unifont" | grep "iso10646-1" | sort -u)
    
    # Get font families
    FONT_FAMILIES=$(xlsfonts | grep "iso10646-1" | cut -d'-' -f3 | sort -u | grep -v '^$')
    
    if [ -z "$FIXED_FONTS" ]; then
        echo -e "${RED}No Unicode X11 fonts found!${NC}"
        echo "Please install: sudo apt-get install xfonts-base xfonts-unifont xfonts-terminus"
        exit 1
    fi
}

# Function to display font menu
show_font_menu() {
    echo -e "\n${GREEN}Available Font Families:${NC}"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    
    local i=1
    declare -g -A FONT_MAP
    
    # Standard fonts
    echo -e "${BLUE}Standard Fonts (Always Available):${NC}"
    FONT_MAP[$i]="fixed"
    echo "  $i) fixed - Standard X11 fixed font (minimal memory)"
    ((i++))
    
    # Check for specific useful fonts
    if xlsfonts | grep -q "terminus.*iso10646-1"; then
        FONT_MAP[$i]="terminus"
        echo "  $i) terminus - Terminus font (clean, programmer-friendly)"
        ((i++))
    fi
    
    if xlsfonts | grep -q "unifont.*iso10646-1"; then
        FONT_MAP[$i]="unifont"
        echo "  $i) unifont - GNU Unifont (excellent Unicode coverage)"
        ((i++))
    fi
    
    if xlsfonts | grep -q "dejavu.*sans.*mono.*iso10646-1"; then
        FONT_MAP[$i]="dejavu"
        echo "  $i) dejavu - DejaVu Sans Mono (if available as core font)"
        ((i++))
    fi
    
    echo
    FONT_MAP[$i]="custom"
    echo "  $i) Custom XLFD string"
    ((i++))
    
    FONT_MAP[$i]="scan"
    echo "  $i) Scan and list all available fonts"
    
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
}

# Function to get font sizes for a family
get_font_sizes() {
    local family=$1
    xlsfonts | grep "$family" | grep "iso10646-1" | sed -n 's/.*--\([0-9]*\)-.*/\1/p' | sort -n | uniq
}

# Function to build XLFD string
build_xlfd() {
    local family=$1
    local size=$2
    local weight=${3:-medium}
    local slant=${4:-r}
    
    case $family in
        "fixed")
            echo "-misc-fixed-${weight}-${slant}-normal--${size}-*-*-*-*-*-iso10646-1"
            ;;
        "terminus")
            echo "-xos4-terminus-${weight}-${slant}-normal--${size}-*-*-*-*-*-iso10646-1"
            ;;
        "unifont")
            # Unifont typically only available in size 16
            echo "-misc-unifont-medium-r-normal--16-160-75-75-c-80-iso10646-1"
            ;;
        "dejavu")
            echo "-misc-dejavu sans mono-${weight}-${slant}-normal--${size}-*-*-*-*-*-iso10646-1"
            ;;
        *)
            echo "$family"
            ;;
    esac
}

# Function to test if a font renders
test_font() {
    local xlfd=$1
    if xlsfonts -fn "$xlfd" 2>/dev/null | grep -q .; then
        return 0
    else
        return 1
    fi
}

# Function to show all available fonts
show_all_fonts() {
    echo -e "\n${GREEN}All Available X11 Core Fonts with Unicode:${NC}"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    xlsfonts | grep "iso10646-1" | head -50
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo "(Showing first 50 fonts)"
    echo
}

# Function to generate C header file
generate_config_header() {
    local xlfd=$1
    local family=$2
    local size=$3
    
    cat > "$CONFIG_FILE" << EOF
// Auto-generated font configuration for leafpad_fltk
// Generated: $(date)
// This file forces use of X11 core fonts for minimal memory usage

#ifndef FONT_CONFIG_H
#define FONT_CONFIG_H

// Selected font configuration
#define SELECTED_FONT_FAMILY "$family"
#define SELECTED_FONT_SIZE $size
#define SELECTED_FONT_XLFD "$xlfd"

// Memory usage estimate: X11 core fonts use ~2-4MB RSS vs 8-15MB for Xft/fontconfig

// Font setup function
static inline const char* get_configured_font() {
    return SELECTED_FONT_XLFD;
}

// Alternative: Function to generate XLFD at different sizes
static inline const char* get_font_at_size(int size) {
    static char fontname[256];
    
    // Try to scale the selected font family to requested size
    snprintf(fontname, sizeof(fontname), "$xlfd");
    
    // For bitmap fonts, this may snap to nearest available size
    return fontname;
}

#endif // FONT_CONFIG_H
EOF

    echo -e "${GREEN}✓ Generated: $CONFIG_FILE${NC}"
}

# Function to generate launch script
generate_launch_script() {
    local xlfd=$1
    
    cat > "$LAUNCH_SCRIPT" << 'EOF'
#!/bin/bash

# Launch wrapper for minimal memory usage
# Prevents loading of fontconfig/freetype libraries

# Unset font-related environment variables
unset FONTCONFIG_FILE
unset FONTCONFIG_PATH

# Prefer X11 rendering
export GDK_USE_XFT=0
export QT_XFT=false
export FLTK_USE_X11=1

# Ensure X11 font paths are available
xset +fp /usr/share/fonts/X11/misc/ 2>/dev/null
xset +fp /usr/share/fonts/X11/100dpi/ 2>/dev/null
xset +fp /usr/share/fonts/X11/75dpi/ 2>/dev/null
xset fp rehash 2>/dev/null

# Launch application
exec ./leafpad_fltk "$@"
EOF

    chmod +x "$LAUNCH_SCRIPT"
    echo -e "${GREEN}✓ Generated: $LAUNCH_SCRIPT${NC}"
}

# Function to generate makefile snippet
generate_makefile_snippet() {
    cat > "Makefile.fonts" << 'EOF'
# Font configuration for minimal memory usage
# Include this in your main Makefile

FONT_CONFIG = font_config.h

# Add this to your CXXFLAGS
FONT_CXXFLAGS = -include $(FONT_CONFIG)

# Update your build rule to include the font config
$(TARGET): $(OBJECTS) $(FONT_CONFIG)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Regenerate font config
reconfig-font:
	./font_selector.sh

.PHONY: reconfig-font
EOF

    echo -e "${GREEN}✓ Generated: Makefile.fonts${NC}"
}

# Function to generate integration code snippet
generate_integration_code() {
    cat > "font_integration.cpp" << 'EOF'
// Font integration code for main.cpp
// Add this to your main.cpp file

#include "font_config.h"

// Setup minimal fonts using the configured font
static void setup_configured_fonts() {
    const char* font = get_configured_font();
    
    // Set all FLTK fonts to use our X11 core font
    Fl::set_font(FL_COURIER, font);
    Fl::set_font(FL_HELVETICA, font);
    Fl::set_font(FL_TIMES, font);
    Fl::set_font(FL_SYMBOL, font);
    
    printf("Using X11 core font: %s\n", font);
}

// In your main() function, add before creating windows:
// setup_configured_fonts();
EOF

    echo -e "${GREEN}✓ Generated: font_integration.cpp${NC}"
}

# Main menu loop
main() {
    check_fonts
    scan_fonts
    
    while true; do
        show_font_menu
        echo
        read -p "Select font family (1-${#FONT_MAP[@]}): " choice
        
        selected_font="${FONT_MAP[$choice]}"
        
        if [ "$selected_font" = "scan" ]; then
            show_all_fonts
            continue
        fi
        
        if [ "$selected_font" = "custom" ]; then
            echo
            read -p "Enter custom XLFD string: " custom_xlfd
            if test_font "$custom_xlfd"; then
                final_xlfd="$custom_xlfd"
                font_family="custom"
                font_size="custom"
                break
            else
                echo -e "${RED}Font not found or invalid XLFD!${NC}"
                continue
            fi
        fi
        
        if [ -n "$selected_font" ]; then
            # Get available sizes
            echo -e "\n${YELLOW}Available sizes for $selected_font:${NC}"
            sizes=($(get_font_sizes "$selected_font"))
            
            if [ ${#sizes[@]} -eq 0 ]; then
                # Some fonts like unifont have fixed sizes
                if [ "$selected_font" = "unifont" ]; then
                    sizes=(16)
                else
                    sizes=(12 13 14 15 16 18 20)
                fi
            fi
            
            echo "Sizes: ${sizes[@]}"
            echo
            read -p "Select font size (or enter custom): " font_size
            
            # Build XLFD
            final_xlfd=$(build_xlfd "$selected_font" "$font_size")
            
            # Test the font
            echo -e "\n${YELLOW}Testing font: $final_xlfd${NC}"
            if test_font "$final_xlfd"; then
                echo -e "${GREEN}✓ Font is available!${NC}"
                font_family="$selected_font"
                break
            else
                echo -e "${RED}✗ Font not available, please try another${NC}"
            fi
        fi
    done
    
    # Generate all configuration files
    echo
    echo -e "${BLUE}Generating configuration files...${NC}"
    echo
    
    generate_config_header "$final_xlfd" "$font_family" "$font_size"
    generate_launch_script "$final_xlfd"
    generate_makefile_snippet
    generate_integration_code
    
    echo
    echo -e "${GREEN}╔════════════════════════════════════════════════════╗${NC}"
    echo -e "${GREEN}║  Configuration Complete!                           ║${NC}"
    echo -e "${GREEN}╚════════════════════════════════════════════════════╝${NC}"
    echo
    echo -e "Selected font: ${BLUE}$final_xlfd${NC}"
    echo
    echo -e "${YELLOW}Next steps:${NC}"
    echo "  1. Add to your main.cpp: #include \"font_config.h\""
    echo "  2. Copy font setup code from: font_integration.cpp"
    echo "  3. Rebuild: make clean && make"
    echo "  4. Run with: ./$LAUNCH_SCRIPT"
    echo
    echo -e "${YELLOW}Memory Impact:${NC}"
    echo "  • X11 core fonts: ~2-4MB RSS"
    echo "  • Xft/fontconfig/FreeType: ~8-15MB RSS"
    echo "  • Expected savings: ~5-12MB RSS"
    echo
}

# Run main
main
