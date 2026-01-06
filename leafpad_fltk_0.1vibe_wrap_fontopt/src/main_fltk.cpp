/*
 *  Leafpad - FLTK based simple text editor
 */

#include <FL/Fl.H>
#include "window_fltk.h"

// Preload control to avoid fontconfig/freetype loading
#include <cstdlib>
#include <cstdio> // For snprintf

// Block problematic libraries from loading
static void block_heavy_libs() {
    // Set environment variables to disable Xft before FLTK initializes
    setenv("FLTK_USE_X11", "1", 0);  // Hint to use X11 fonts

    // These will be checked by FLTK when it tries to load fonts
    // Note: This works best if done before any FLTK initialization
}

// Use X11 XLFD (X Logical Font Description) format
// These bypass Xft/fontconfig entirely
static const char* get_fixed_font(int size) {
    static char fontname[256];

    // For sizes we commonly use, return specific fixed fonts
    // This uses the X11 "fixed" font family which is always available
    switch(size) {
        case 12:
        case 13:
        case 14:
            // Standard fixed font, medium weight
            snprintf(fontname, sizeof(fontname),
                    "-misc-fixed-medium-r-normal--%d-*-*-*-*-*-iso10646-1", size);
            break;
        default:
            // Fallback to basic fixed font
            snprintf(fontname, sizeof(fontname), "fixed");
    }
    return fontname;
}

static const char* get_unifont(int size) {
    static char fontname[256];
    // Unifont is available in limited sizes (16 typically)
    // But provides excellent Unicode coverage with low memory usage
    snprintf(fontname, sizeof(fontname),
            "-misc-unifont-medium-r-normal--16-*-*-*-*-*-iso10646-1");
    return fontname;
}

// Setup minimal fonts using X11 core font names (XLFD format)
static void setup_minimal_fonts() {
    // Using XLFD (X Logical Font Description) format forces X11 core fonts
    // This bypasses Xft completely even if FLTK was compiled with it

    // Option 1: Use standard fixed font (always available, minimal memory)
    const char* font = get_fixed_font(14);

    // Option 2: Uncomment for better Unicode support (requires xfonts-unifont)
    // const char* font = get_unifont(16);

    // Set all FLTK fonts to use our chosen X11 core font
    Fl::set_font(FL_COURIER, font);
    Fl::set_font(FL_HELVETICA, font);
    Fl::set_font(FL_TIMES, font);
    Fl::set_font(FL_SYMBOL, font);
}


int main(int argc, char **argv) {
    // CRITICAL: Do this before any FLTK initialization
    block_heavy_libs();

    // Setup fonts to use X11 core fonts only
    setup_minimal_fonts();
    MainWindow *window = new MainWindow(600, 400, "Leafpad");

    if (argc > 1) {
        window->editor->buffer()->loadfile(argv[1]);
        window->current_filename = argv[1];
        // Remove the filename from argv so FLTK doesn't parse it
        for (int i = 1; i < argc - 1; i++) {
            argv[i] = argv[i + 1];
        }
        argc--;
    }

    window->show(argc, argv);
    return Fl::run();
}
