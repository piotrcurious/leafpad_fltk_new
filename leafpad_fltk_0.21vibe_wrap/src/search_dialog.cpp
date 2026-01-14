/*
 *  Leafpad - FLTK based simple text editor
 */

#include "search_dialog.h"
#include <FL/Fl_Text_Buffer.H>
#include <cstring>

void SearchDialog::on_find_cb(Fl_Widget* w, void* data) {
    SearchDialog* dialog = (SearchDialog*)data;
    const char* find_text = dialog->find_input->value();
    if (strlen(find_text) == 0) return;

    Fl_Text_Buffer* buffer = dialog->main_win->editor->buffer();
    int pos = dialog->main_win->editor->insert_position();
    int found = buffer->search_forward(pos, find_text, &pos);

    if (found) {
        buffer->select(pos, pos + strlen(find_text));
        dialog->main_win->editor->insert_position(pos + strlen(find_text));
        dialog->main_win->editor->show_insert_position();
    }
}

void SearchDialog::on_replace_cb(Fl_Widget* w, void* data) {
    SearchDialog* dialog = (SearchDialog*)data;
    const char* find_text = dialog->find_input->value();
    const char* replace_text = dialog->replace_input->value();

    if (dialog->main_win->editor->buffer()->selected()) {
        // If the selected text matches the find text, replace it
        if (strcmp(dialog->main_win->editor->buffer()->selection_text(), find_text) == 0) {
            dialog->main_win->editor->buffer()->replace_selection(replace_text);
        }
    }
    on_find_cb(w, data);
}

void SearchDialog::on_replace_all_cb(Fl_Widget* w, void* data) {
    SearchDialog* dialog = (SearchDialog*)data;
    const char* find_text = dialog->find_input->value();
    const char* replace_text = dialog->replace_input->value();
    if (strlen(find_text) == 0) return;

    Fl_Text_Buffer* buffer = dialog->main_win->editor->buffer();
    int pos = 0;
    while (buffer->search_forward(pos, find_text, &pos)) {
        buffer->select(pos, pos + strlen(find_text));
        buffer->replace_selection(replace_text);
        pos += strlen(replace_text);
    }
}

SearchDialog::SearchDialog(MainWindow* win) : Fl_Window(300, 150, "Find and Replace") {
    main_win = win;

    begin();
    find_input = new Fl_Input(80, 20, 200, 25, "Find:");
    replace_input = new Fl_Input(80, 50, 200, 25, "Replace:");
    find_button = new Fl_Button(20, 90, 80, 25, "Find");
    replace_button = new Fl_Button(110, 90, 80, 25, "Replace");
    replace_all_button = new Fl_Button(200, 90, 80, 25, "Replace All");
    end();

    find_button->callback(on_find_cb, this);
    replace_button->callback(on_replace_cb, this);
    replace_all_button->callback(on_replace_all_cb, this);
}
