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
