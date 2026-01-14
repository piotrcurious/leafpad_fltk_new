/*
 *  Leafpad - FLTK based simple text editor
 */

#include "view_fltk.h"
#include "window_fltk.h"
#include <FL/Fl.H>

EditorView::EditorView(int x, int y, int w, int h, MainWindow* win, const char* label) : Fl_Text_Editor(x, y, w, h, label) {
    last_key = 0;
    main_win = win;
}

int EditorView::handle(int event) {
    if (event == FL_KEYBOARD) {
        if (Fl::test_shortcut(FL_CTRL + 'z')) {
            main_win->undo_manager->undo();
            return 1;
        }
        if (Fl::test_shortcut(FL_CTRL + FL_SHIFT + 'z')) {
            main_win->undo_manager->redo();
            return 1;
        }
        last_key = Fl::event_key();
    }
    return Fl_Text_Editor::handle(event);
}
