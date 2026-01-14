/*
 * Leafpad - FLTK based simple text editor
 * MainWindow Header
 */

#ifndef WINDOW_FLTK_H
#define WINDOW_FLTK_H

#include <FL/Fl_Window.H>
#include "view_fltk.h"
#include "UndoManager.h"

// Forward declaration of the line number widget class
class LineNumberWidget;
struct Fl_Menu_Item;

class MainWindow : public Fl_Window {
public:
    EditorView* editor;               // The primary text display/editor area
    char* current_filename;           // Path to the currently open file
    UndoManager* undo_manager;        // Handles undo/redo history
    bool changed;                     // Flag to track unsaved modifications
    bool line_wrap_enabled;           // State of the word wrap option
    LineNumberWidget* line_numbers;   // The gutter widget for line numbering
	void redraw_line_numbers();

    /**
     * Constructor for the main application window.
     * @param w Width of the window
     * @param h Height of the window
     * @param title Title text for the window frame
     */
    MainWindow(int w, int h, const char* title);
};

#endif // WINDOW_FLTK_H
