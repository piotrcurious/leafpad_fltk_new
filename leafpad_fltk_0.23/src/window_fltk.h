/*
 * Leafpad - FLTK based simple text editor
 */

#ifndef WINDOW_FLTK_H
#define WINDOW_FLTK_H

#include <FL/Fl_Window.H>
#include <string>           // Added for std::string
#include "view_fltk.h"
#include "UndoManager.h"

struct Fl_Menu_Item;
class SearchWidget;

class MainWindow : public Fl_Window {
public:
    EditorView* editor;
    SearchWidget* search_widget;
    std::string current_filename; // Changed from char* to std::string
    UndoManager* undo_manager;
    bool changed;
    bool line_wrap_enabled;

    MainWindow(int w, int h, const char* title);
    ~MainWindow();
    void update_linenumber_width();
    void set_current_filename(const char* f);
    int handle(int event) override;
    void resize(int x, int y, int w, int h) override;
};

#endif // WINDOW_FLTK_H
