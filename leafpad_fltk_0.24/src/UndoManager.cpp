#include "UndoManager.h"
#include "view_fltk.h"
#include "window_fltk.h"

UndoManager::UndoManager(Fl_Text_Buffer* buffer, void* view, void* window) {
    this->buffer = buffer;
    undo_init(buffer, view, window,nullptr,nullptr);
}

UndoManager::~UndoManager() {
    undo_clear_all();
}

void UndoManager::undo() {
    undo_undo();
}

void UndoManager::redo() {
    undo_redo();
}
