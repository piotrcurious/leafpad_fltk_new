#ifndef UNDO_MANAGER_H
#define UNDO_MANAGER_H

#include <FL/Fl_Text_Buffer.H>

#include "undo.h"

class UndoManager {
public:
    UndoManager(Fl_Text_Buffer* buffer, void* view, void* window);
    ~UndoManager();

    void undo();
    void redo();

private:
    Fl_Text_Buffer* buffer;
};

#endif // UNDO_MANAGER_H
