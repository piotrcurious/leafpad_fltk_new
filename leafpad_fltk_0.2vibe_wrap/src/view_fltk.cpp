#include "view_fltk.h"
#include "window_fltk.h"
#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <stdio.h>

/* 1. MOVE CLASS DEFINITION TO THE TOP */
/* This ensures EditorView knows the methods of LineNumberWidget */
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
            // Calculate top position based on current scroll
            int top_pos = editor->get_xy_pos(editor->x(), editor->y());
            
            for (int ly = editor->y(); ly < editor->y() + editor->h(); ly += line_height) {
                int char_pos = editor->get_xy_pos(editor->x(), ly);
                int line_start = editor->buffer()->line_start(char_pos);
                
                // Only draw if this Y-coord corresponds to the START of a logical line
                if (char_pos == line_start) {
                    int line_num = editor->buffer()->count_lines(0, char_pos) + 1;
                    int sx, sy;
                    if (editor->position_to_xy(char_pos, &sx, &sy)) {
                        char str[12];
                        snprintf(str, sizeof(str), "%d", line_num);
                        int tw = 0, th = 0;
                        fl_measure(str, tw, th, 0);
                        fl_draw(str, x() + w() - tw - 5, sy + editor->textsize());
                    }
                }
            }
        }
        fl_pop_clip();
    }
};

/* 2. EditorView Implementation */

EditorView::EditorView(int x, int y, int w, int h, MainWindow* win, const char* label) 
    : Fl_Text_Editor(x, y, w, h, label) {
    last_key = 0;
    main_win = win;
}

void EditorView::draw() {
    Fl_Text_Editor::draw(); // Draw the editor first
    
    // Now that LineNumberWidget is fully defined above, this will compile!
    if (main_win && main_win->line_numbers && main_win->line_numbers->visible()) {
        main_win->line_numbers->damage(FL_DAMAGE_ALL);
    }
}


int EditorView::handle(int event) {
    int ret = Fl_Text_Editor::handle(event);
    
    // FL_DRAG catches the scrollbar thumb being moved
    // FL_MOUSEWHEEL catches the wheel
    // FL_KEYDOWN catches cursor movement
    if (event == FL_DRAG || event == FL_MOUSEWHEEL || event == FL_KEYDOWN) {
        if (main_win) {
            main_win->redraw_line_numbers();
        }
    }
    
    return ret;
}




int EditorView::get_xy_pos(int x, int y) { 
    return xy_to_position(x, y); 
}

Fl_Scrollbar* EditorView::get_v_scrollbar() { 
    return mVScrollBar; 
}
