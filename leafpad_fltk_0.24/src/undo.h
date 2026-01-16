#ifndef UNDO_H
#define UNDO_H

#include <FL/Fl_Text_Buffer.H>

// forward declaration for pointer usage in header
class Fl_Widget;

#ifdef __cplusplus
extern "C" {
#endif

void undo_init(Fl_Text_Buffer *buffer,
               void* view,
               void* window,
               Fl_Widget* undo_button,
               Fl_Widget* redo_button);

void undo_clear_all();
void undo_undo();
void undo_redo();
void undo_reset_modified_step();
void undo_set_sequency(bool seq);
void undo_set_sequency_reserve();

// --- Grouped transactions API ---
void undo_begin_group();
void undo_end_group();

#ifdef __cplusplus
}
#endif

#endif // UNDO_H
