/*
 *  Leafpad - FLTK based simple text editor
 */

#ifndef VIEW_FLTK_H
#define VIEW_FLTK_H

#include <FL/Fl_Text_Editor.H>

class MainWindow;

class EditorView : public Fl_Text_Editor {
public:
    int last_key;
    MainWindow* main_win;

    EditorView(int x, int y, int w, int h, MainWindow* win, const char* label = 0);
    int handle(int event) override;
    int get_top_line_num() { return mTopLineNum; }
};

#endif // VIEW_FLTK_H
