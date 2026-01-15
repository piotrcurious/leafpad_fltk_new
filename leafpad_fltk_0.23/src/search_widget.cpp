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
    if (!find_text || find_text[0] == '\0') return;

    Fl_Text_Buffer* buffer = widget->main_win->editor->buffer();
    int len = buffer->length();
    if (len == 0) return;

    int sel_start, sel_end;
    int search_from;

    // Determine where to start looking backwards from
    if (buffer->selection_position(&sel_start, &sel_end)) {
        search_from = sel_start - 1; // Start before current selection
    } else {
        search_from = widget->main_win->editor->insert_position() - 1;
    }

    // Boundary check: If we are at the very top, wrap to the very bottom
    if (search_from < 0) {
        search_from = len;
    }

    int found_pos = -1;
    // search_backward(start_pos, text, found_pos_ptr)
    // start_pos is the index to start looking back FROM.
    int found = buffer->search_backward(search_from, find_text, &found_pos);

    // If not found in the first pass, wrap around and search from the end
    if (!found) {
        found = buffer->search_backward(len, find_text, &found_pos);
    }

    if (found) {
        buffer->select(found_pos, found_pos + (int)strlen(find_text));
        widget->main_win->editor->insert_position(found_pos);
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
    // We must add 'x' and 'y' to the child coordinates
    find_input = new Fl_Input(x + 80, y + 5, 200, 25, "Find:");
    replace_input = new Fl_Input(x + 80, y + 32, 200, 25, "Replace:");
    
    find_next_button = new Fl_Return_Button(x + 300, y + 5, 80, 25, "Next");
    find_prev_button = new Fl_Button(x + 390, y + 5, 80, 25, "Previous");
    
    replace_button = new Fl_Button(x + 300, y + 32, 80, 25, "Replace");
    replace_all_button = new Fl_Button(x + 390, y + 32, 80, 25, "Replace All");
    
    close_button = new Fl_Button(x + w - 35, y + 5, 30, 25, "X");
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
	main_win->redraw();
}

void SearchWidget::hide_widget() {
    this->hide();
    main_win->resize(main_win->x(), main_win->y(), main_win->w(), main_win->h());
	main_win->redraw();
}
