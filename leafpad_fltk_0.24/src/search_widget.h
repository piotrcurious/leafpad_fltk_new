/*
 *  Leafpad - FLTK based simple text editor
 */

#ifndef SEARCH_WIDGET_H
#define SEARCH_WIDGET_H

#include <FL/Fl_Group.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Return_Button.H>
#include "window_fltk.h"

class SearchWidget : public Fl_Group {
public:
    MainWindow* main_win;
    Fl_Input* find_input;
    Fl_Input* replace_input;
    Fl_Return_Button* find_next_button;
    Fl_Button* find_prev_button;
    Fl_Button* replace_button;
    Fl_Button* replace_all_button;
    Fl_Button* close_button;

    SearchWidget(MainWindow* win, int x, int y, int w, int h);

    void show_widget();
    void hide_widget();

    static void on_find_next_cb(Fl_Widget* w, void* data);
    static void on_find_prev_cb(Fl_Widget* w, void* data);
    static void on_replace_cb(Fl_Widget* w, void* data);
    static void on_replace_all_cb(Fl_Widget* w, void* data);
    static void on_close_cb(Fl_Widget* w, void* data);
};

#endif // SEARCH_WIDGET_H
