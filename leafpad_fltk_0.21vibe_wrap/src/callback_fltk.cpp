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
    window->current_filename = NULL;
    window->changed = false;
}

void on_file_open(Fl_Widget* w, void* v) {
    MainWindow* window = (MainWindow*)v;
    char* filename = fl_file_chooser("Open File", "*", "");
    if (filename) {
        window->editor->buffer()->loadfile(filename);
        window->current_filename = filename;
    }
}

void on_file_save(Fl_Widget* w, void* v) {
    MainWindow* window = (MainWindow*)v;
    if (!window->current_filename) {
        char* filename = fl_file_chooser("Save File", "*", "");
        if (filename) {
            window->current_filename = filename;
        } else {
            return;
        }
    }
    window->editor->buffer()->savefile(window->current_filename);
    window->changed = false;
}

void on_file_save_as(Fl_Widget* w, void* v) {
    MainWindow* window = (MainWindow*)v;
    char* filename = fl_file_chooser("Save File As", "*", "");
    if (filename) {
        window->current_filename = filename;
        window->editor->buffer()->savefile(window->current_filename);
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
