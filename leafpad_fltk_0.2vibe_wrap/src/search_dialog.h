/*
 *  Leafpad - FLTK based simple text editor
 */

#ifndef SEARCH_DIALOG_H
#define SEARCH_DIALOG_H

#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include "window_fltk.h"

class SearchDialog : public Fl_Window {
public:
    MainWindow* main_win;
    Fl_Input* find_input;
    Fl_Input* replace_input;
    Fl_Button* find_button;
    Fl_Button* replace_button;
    Fl_Button* replace_all_button;

    SearchDialog(MainWindow* win);

    static void on_find_cb(Fl_Widget* w, void* data);
    static void on_replace_cb(Fl_Widget* w, void* data);
    static void on_replace_all_cb(Fl_Widget* w, void* data);
};

#endif // SEARCH_DIALOG_H
