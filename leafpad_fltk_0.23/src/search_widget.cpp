/*
 *  Leafpad - FLTK based simple text editor
 */

#include "search_widget.h"
#include <FL/Fl_Text_Buffer.H>
#include <cstring>

void SearchWidget::on_find_next_cb(Fl_Widget* w, void* data) {
    SearchWidget* widget = (SearchWidget*)data;
    const char* find_text = widget->find_input->value();
    if (strlen(find_text) == 0) return;

    Fl_Text_Buffer* buffer = widget->main_win->editor->buffer();
    int pos = widget->main_win->editor->insert_position();
    int found = buffer->search_forward(pos, find_text, &pos);

    if (found) {
        buffer->select(pos, pos + strlen(find_text));
        widget->main_win->editor->insert_position(pos + strlen(find_text));
        widget->main_win->editor->show_insert_position();
    }
}

void SearchWidget::on_find_prev_cb(Fl_Widget* w, void* data) {
    SearchWidget* widget = (SearchWidget*)data;
    const char* find_text = widget->find_input->value();
    if (strlen(find_text) == 0) return;

    Fl_Text_Buffer* buffer = widget->main_win->editor->buffer();
    int pos = widget->main_win->editor->insert_position();
    int found = buffer->search_backward(pos, find_text, &pos);

    if (found) {
        buffer->select(pos, pos + strlen(find_text));
        widget->main_win->editor->insert_position(pos);
        widget->main_win->editor->show_insert_position();
    }
}

void SearchWidget::on_replace_cb(Fl_Widget* w, void* data) {
    SearchWidget* widget = (SearchWidget*)data;
    const char* find_text = widget->find_input->value();
    const char* replace_text = widget->replace_input->value();

    if (widget->main_win->editor->buffer()->selected()) {
        if (strcmp(widget->main_win->editor->buffer()->selection_text(), find_text) == 0) {
            widget->main_win->editor->buffer()->replace_selection(replace_text);
        }
    }
    on_find_next_cb(w, data);
}

void SearchWidget::on_replace_all_cb(Fl_Widget* w, void* data) {
    SearchWidget* widget = (SearchWidget*)data;
    const char* find_text = widget->find_input->value();
    const char* replace_text = widget->replace_input->value();
    if (strlen(find_text) == 0) return;

    Fl_Text_Buffer* buffer = widget->main_win->editor->buffer();
    int pos = 0;
    while (buffer->search_forward(pos, find_text, &pos)) {
        buffer->select(pos, pos + strlen(find_text));
        buffer->replace_selection(replace_text);
        pos += strlen(replace_text);
    }
}

void SearchWidget::on_close_cb(Fl_Widget* w, void* data) {
    SearchWidget* widget = (SearchWidget*)data;
    widget->hide_widget();
}

SearchWidget::SearchWidget(MainWindow* win, int x, int y, int w, int h) : Fl_Group(x, y, w, h) {
    main_win = win;

    begin();
    find_input = new Fl_Input(80, 0, 200, 25, "Find:");
    replace_input = new Fl_Input(80, 30, 200, 25, "Replace:");
    find_next_button = new Fl_Return_Button(300, 0, 80, 25, "Next");
    find_prev_button = new Fl_Button(390, 0, 80, 25, "Previous");
    replace_button = new Fl_Button(300, 30, 80, 25, "Replace");
    replace_all_button = new Fl_Button(390, 30, 80, 25, "Replace All");
    close_button = new Fl_Button(w - 30, 0, 30, 30, "X");
    end();

    find_next_button->callback(on_find_next_cb, this);
    find_prev_button->callback(on_find_prev_cb, this);
    replace_button->callback(on_replace_cb, this);
    replace_all_button->callback(on_replace_all_cb, this);
    close_button->callback(on_close_cb, this);
}

void SearchWidget::show_widget() {
    this->show();
    main_win->resize(main_win->x(), main_win->y(), main_win->w(), main_win->h());
}

void SearchWidget::hide_widget() {
    this->hide();
    main_win->resize(main_win->x(), main_win->y(), main_win->w(), main_win->h());
}
