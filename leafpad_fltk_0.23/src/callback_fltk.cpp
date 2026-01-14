/*
 *  Leafpad - FLTK based simple text editor
 */

#include "callback_fltk.h"
#include <FL/fl_ask.H>
#include <FL/Fl_File_Chooser.H>
#include <stdio.h>

void on_file_new(Fl_Widget* w, void* v) {
    MainWindow* window = (MainWindow*)v;
    window->editor->buffer()->text("");
    window->set_current_filename(NULL);
    window->changed = false;
}

void on_file_open(Fl_Widget* w, void* v) {
    MainWindow* window = (MainWindow*)v;
    char* filename = fl_file_chooser("Open File", "*", "");
    if (filename) {
        window->editor->buffer()->loadfile(filename);
        window->set_current_filename(filename);
        window->update_linenumber_width();
    }
}


void on_file_save(Fl_Widget* w, void* v) {
    MainWindow* window = (MainWindow*)v;

    // 1. Check if the string is empty instead of checking for a null pointer
    if (window->current_filename.empty()) {
        char* filename = fl_file_chooser("Save File", "*", "");
        if (filename) {
            window->set_current_filename(filename);
        } else {
            return;
        }
    }

    // 2. Use .c_str() to provide the const char* that FLTK expects
    window->editor->buffer()->savefile(window->current_filename.c_str());
    window->changed = false;
}

void on_file_save_as(Fl_Widget* w, void* v) {
    MainWindow* window = (MainWindow*)v;
    char* filename = fl_file_chooser("Save File As", "*", "");
    if (filename) {
        window->set_current_filename(filename);
        // 2. Use .c_str() here as well
        window->editor->buffer()->savefile(window->current_filename.c_str());
        window->changed = false;
    }
}

#include <stdlib.h>

void on_file_quit(Fl_Widget* w, void* v) {
    MainWindow* window = (MainWindow*)v;
    if (window->changed) {
        int result = fl_choice("The document has been modified. Do you want to save your changes?", "Cancel", "Save", "Don't Save");
        if (result == 0) { // Cancel
            return;
        } else if (result == 1) { // Save
            on_file_save(w, v);
        }
    }
    exit(0);
}

void on_edit_undo(Fl_Widget* w, void* v) {
    MainWindow* window = (MainWindow*)v;
    window->undo_manager->undo();
}

void on_edit_redo(Fl_Widget* w, void* v) {
    MainWindow* window = (MainWindow*)v;
    window->undo_manager->redo();
}

void on_edit_cut(Fl_Widget* w, void* v) {
    MainWindow* window = (MainWindow*)v;
    Fl_Text_Editor::kf_cut(0, window->editor);
}

void on_edit_copy(Fl_Widget* w, void* v) {
    MainWindow* window = (MainWindow*)v;
    Fl_Text_Editor::kf_copy(0, window->editor);
}

void on_edit_paste(Fl_Widget* w, void* v) {
    MainWindow* window = (MainWindow*)v;
    Fl_Text_Editor::kf_paste(0, window->editor);
}

void on_edit_select_all(Fl_Widget* w, void* v) {
    MainWindow* window = (MainWindow*)v;
    window->editor->take_focus();
    Fl_Text_Editor::kf_select_all(0, window->editor);
}
