/*
 *  Leafpad - FLTK based simple text editor
 */

#include "window_fltk.h"
#include "view_fltk.h"
#include "callback_fltk.h"
#include "dialog_fltk.h"
#include "search_dialog.h"
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Text_Buffer.H>

// Callback for the "About" menu item
static void on_about_cb(Fl_Widget*, void*) {
    show_about_dialog();
}

static void on_options_line_wrap(Fl_Widget* w, void* data) {
    MainWindow* win = (MainWindow*)data;
    win->line_wrap_enabled = !win->line_wrap_enabled;
    if (win->line_wrap_enabled) {
        win->editor->wrap_mode(Fl_Text_Display::WRAP_AT_BOUNDS, 0);
    } else {
        win->editor->wrap_mode(Fl_Text_Display::WRAP_NONE, 0);
    }
    Fl_Menu_Bar* menu_bar = (Fl_Menu_Bar*)w;
    Fl_Menu_Item* item = (Fl_Menu_Item*)menu_bar->find_item("&Options/&Line Wrap");
    if (item) {
        if (win->line_wrap_enabled) {
            item->set();
        } else {
            item->clear();
        }
    }
}

void MainWindow::update_linenumber_width() {
    if (editor->linenumber_width() == 0) return;

    int num_lines = editor->buffer()->count_lines(0, editor->buffer()->length());
    int digits = 1;
    if (num_lines > 9) digits = 2;
    if (num_lines > 99) digits = 3;
    if (num_lines > 999) digits = 4;
    if (num_lines > 9999) digits = 5;
    if (num_lines > 99999) digits = 6;

    char str[10];
    for (int i = 0; i < digits; i++) {
        str[i] = '9';
    }
    str[digits] = 0;

    fl_font(editor->textfont(), editor->textsize());
    int width = (int)fl_width(str);
    editor->linenumber_width(width + 10);
}

static void on_edit_find(Fl_Widget* w, void* data) {
    MainWindow* win = (MainWindow*)data;
    SearchDialog* dialog = new SearchDialog(win);
    dialog->show();
}

MainWindow::~MainWindow() {
    if (current_filename) {
        free(current_filename);
    }
}

void MainWindow::set_current_filename(const char* f) {
    if (current_filename) {
        free(current_filename);
    }
    if (f) {
        current_filename = strdup(f);
    } else {
        current_filename = NULL;
    }
}

static void on_buffer_changed(int, int, int, int, const char*, void* data) {
    MainWindow* win = (MainWindow*)data;
    win->update_linenumber_width();
}

static void on_options_line_numbers(Fl_Widget* w, void* data) {
    MainWindow* win = (MainWindow*)data;
    if (win->editor->linenumber_width() > 0) {
        win->editor->linenumber_width(0);
    } else {
        win->editor->linenumber_width(40);
        win->update_linenumber_width();
    }
    win->redraw();
}

MainWindow::MainWindow(int w, int h, const char* title) : Fl_Window(w, h, title) {
    current_filename = NULL;
    changed = false;
    line_wrap_enabled = false;

    begin();

    editor = new EditorView(0, 30, w, h - 30, this);
    editor->linenumber_width(0);
    Fl_Text_Buffer *buff = new Fl_Text_Buffer();
    buff->add_modify_callback(on_buffer_changed, this);
    editor->buffer(buff);
    undo_manager = new UndoManager(buff, editor, this);

    Fl_Menu_Item menu_items[] = {
        { "&File", 0, 0, 0, FL_SUBMENU },
            { "&New", FL_CTRL + 'n', (Fl_Callback *)on_file_new, this },
            { "&Open...", FL_CTRL + 'o', (Fl_Callback *)on_file_open, this },
            { "&Save", FL_CTRL + 's', (Fl_Callback *)on_file_save, this },
            { "Save &As...", FL_CTRL + FL_SHIFT + 's', (Fl_Callback *)on_file_save_as, this },
            { "&Quit", FL_CTRL + 'q', (Fl_Callback *)on_file_quit, this },
            { 0 },
        { "&Edit", 0, 0, 0, FL_SUBMENU },
            { "Undo", FL_CTRL + 'z', (Fl_Callback *)on_edit_undo, this },
            { "Redo", FL_CTRL + FL_SHIFT + 'z', (Fl_Callback *)on_edit_redo, this },
            { "Cu&t", FL_CTRL + 'x', (Fl_Callback *)on_edit_cut, this },
            { "&Copy", FL_CTRL + 'c', (Fl_Callback *)on_edit_copy, this },
            { "&Paste", FL_CTRL + 'v', (Fl_Callback *)on_edit_paste, this },
            { "Select &All", FL_CTRL + 'a', (Fl_Callback *)on_edit_select_all, this },
            { "&Find...", FL_CTRL + 'f', (Fl_Callback *)on_edit_find, this },
            { 0 },
        { "&Options", 0, 0, 0, FL_SUBMENU },
            { "&Line Wrap", 0, (Fl_Callback *)on_options_line_wrap, this, FL_MENU_TOGGLE },
            { "&Line Numbers", 0, (Fl_Callback *)on_options_line_numbers, this, FL_MENU_TOGGLE },
            { 0 },
        { "&Help", 0, 0, 0, FL_SUBMENU },
            { "&About", 0, (Fl_Callback*)on_about_cb },
            { 0 },
        { 0 }
    };

    Fl_Menu_Bar* menu = new Fl_Menu_Bar(0, 0, w, 30);
    menu->copy(menu_items);

    end();
    resizable(editor);
}
