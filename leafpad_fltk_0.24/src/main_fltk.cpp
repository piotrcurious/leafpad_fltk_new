/*
 *  Leafpad - FLTK based simple text editor
 */

#include <FL/Fl.H>
#include "window_fltk.h"

int main(int argc, char **argv) {
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
