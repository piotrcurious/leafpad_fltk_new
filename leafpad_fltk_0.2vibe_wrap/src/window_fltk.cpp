/*
 * Leafpad - FLTK based simple text editor
 * Fixed Line Number Widget and Access Violations
 */

#include "window_fltk.h"
#include "view_fltk.h"
#include "callback_fltk.h"
#include "dialog_fltk.h"
#include "search_dialog.h"
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Scrollbar.H>
#include <FL/fl_draw.H>
#include <stdio.h>

/**
 * LineNumberWidget: 
 * Optimized to draw only visible lines and sync with EditorView
 */
class LineNumberWidget : public Fl_Widget {
    EditorView* editor;
public:
    LineNumberWidget(int x, int y, int w, int h, EditorView* e) : Fl_Widget(x, y, w, h, 0) {
        editor = e;
    }


void draw() override {
    fl_push_clip(x(), y(), w(), h());
    fl_color(FL_LIGHT2);
    fl_rectf(x(), y(), w(), h());
    
    fl_color(FL_GRAY0);
    fl_line(x() + w() - 1, y(), x() + w() - 1, y() + h());

    if (editor && editor->buffer()) {
        fl_color(FL_DARK3);
        fl_font(editor->textfont(), editor->textsize());
        
        int line_height = editor->textsize() + 2; 
        int last_drawn_line = -1;

        // Iterate through every pixel row of the editor to find where lines start
        for (int ly = editor->y(); ly < editor->y() + editor->h(); ly += 1) {
            // Get the character position at this specific Y coordinate
            int char_pos = editor->get_xy_pos(editor->x(), ly);
            
            // Logical line number (1-based)
            int line_num = editor->buffer()->count_lines(0, char_pos) + 1;

            // Only draw if:
            // 1. We haven't drawn this line number yet
            // 2. The character at this Y is the actual start of a logical line
            if (line_num != last_drawn_line && char_pos == editor->buffer()->line_start(char_pos)) {
                
                int sx, sy;
                // Double check the exact Y coordinate FLTK wants for this position
                if (editor->position_to_xy(char_pos, &sx, &sy)) {
                    // Ensure the text aligns with the editor's text baseline
                    char str[12];
                    snprintf(str, sizeof(str), "%d", line_num);
                    
                    int tw = 0, th = 0;
                    fl_measure(str, tw, th, 0);
                    
                    // sy is the top of the character; we adjust to align vertically
                    fl_draw(str, x() + w() - tw - 5, sy + editor->textsize());
                    
                    last_drawn_line = line_num;
                    // Skip ahead by line height to save CPU cycles
                    ly += line_height - 2; 
                }
            }
        }
    }
    fl_pop_clip();
}


};

/* --- Callbacks declared BEFORE MainWindow constructor --- */

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
        if (win->line_wrap_enabled) item->set();
        else item->clear();
    }
}

static void on_options_line_numbers(Fl_Widget* w, void* data) {
    MainWindow* win = (MainWindow*)data;
    if (win->line_numbers->visible()) {
        win->line_numbers->hide();
    } else {
        win->line_numbers->show();
    }
    win->redraw();
}

static void on_edit_find(Fl_Widget* w, void* data) {
    MainWindow* win = (MainWindow*)data;
    SearchDialog* dialog = new SearchDialog(win);
    dialog->show();
}

static void editor_sync_cb(int pos, int nInserted, int nDeleted, int nRestyled, const char* deletedText, void* cbArg) {
    MainWindow* win = (MainWindow*)cbArg;
    if (win->line_numbers && win->line_numbers->visible()) {
        win->line_numbers->redraw();
    }
}

static void scroll_cb(Fl_Widget* w, void* data) {
    MainWindow* win = (MainWindow*)data;
    // Redraw line numbers during scroll
    if (win->line_numbers && win->line_numbers->visible()) {
        win->line_numbers->redraw();
    }
}



/* --- MainWindow Implementation --- */
//helpers
void MainWindow::redraw_line_numbers() {
    if (line_numbers && line_numbers->visible()) {
        line_numbers->damage(FL_DAMAGE_ALL); // Mark as completely "dirty"
        line_numbers->redraw();
    }
}
//main
MainWindow::MainWindow(int w, int h, const char* title) : Fl_Window(w, h, title) {
    current_filename = NULL;
    changed = false;
    line_wrap_enabled = false;

    begin();

    editor = new EditorView(45, 30, w - 45, h - 30, this);
    line_numbers = new LineNumberWidget(0, 30, 45, h - 30, editor);
    line_numbers->hide();
    
    Fl_Text_Buffer *buff = new Fl_Text_Buffer();
    editor->buffer(buff);
    
    // Sync text changes
    buff->add_modify_callback(editor_sync_cb, this);
    
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
