/*
 *  Leafpad - FLTK based simple text editor
 */

#ifndef WINDOW_FLTK_H
#define WINDOW_FLTK_H

#include <FL/Fl_Window.H>
#include "view_fltk.h"
#include "UndoManager.h"

struct Fl_Menu_Item;

class MainWindow : public Fl_Window {
public:
    EditorView* editor;
    char* current_filename;
    UndoManager* undo_manager;
    bool changed;
    bool line_wrap_enabled;

    MainWindow(int w, int h, const char* title);
};

#endif // WINDOW_FLTK_H
