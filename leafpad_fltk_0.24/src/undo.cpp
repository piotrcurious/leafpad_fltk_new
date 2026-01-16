#include <FL/Fl.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Widget.H>
#include "undo.h"
#include "view_fltk.h"
#include "window_fltk.h"
#include <list>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>

#define DV(x) // empty for production; replace with printf(...) for debugging

typedef struct UndoInfo {
    char command;      // INS, BS, DEL
    int start;
    int end;
    bool seq;          // sequency flag
    char *str;         // payload for inserts/deletes
    bool is_group;     // is this entry a group container?
    std::list<UndoInfo*> *children; // valid if is_group == true
} UndoInfo;

enum {
    INS = 0,
    BS,
    DEL
};

static Fl_Text_Buffer* text_buffer = nullptr;
static EditorView* editor_view = nullptr;
static Fl_Widget* undo_w = nullptr;
static Fl_Widget* redo_w = nullptr;
static std::list<UndoInfo*> undo_list;
static std::list<UndoInfo*> redo_list;
static std::string undo_str;
static UndoInfo *ui_tmp = nullptr;
static int modified_step = 0;
static unsigned int prev_keyval = 0;
static bool seq_reserve = false;

// store MainWindow pointer (FLTK's buffer doesn't provide a modify_cb_arg getter)
static MainWindow* main_window = nullptr;

// Guard to prevent recording undo entries while we programmatically modify buffer
static bool undo_in_progress = false;

// Group stack to support nested grouped transactions. Each element is a pointer to a list of UndoInfo*
static std::vector<std::list<UndoInfo*>*> group_stack;

// Forward declarations
static void undo_flush_temporal_buffer();
static void undo_clear_info_list(std::list<UndoInfo*>& info_list);
static void undo_append_undo_info(UndoInfo* ui);
static void undo_create_undo_info(char command, int start, int end, const char* text);
static bool undo_undo_real();
static bool undo_redo_real();
static void undo_check_modified_step();
static void apply_undoinfo_action(UndoInfo* ui, bool is_undo);

// Helper: push an UndoInfo into either the current group (if any) or the main undo_list
static void push_undoinfo_to_container(UndoInfo* ui)
{
    if (!group_stack.empty()) {
        group_stack.back()->push_back(ui);
    } else {
        undo_list.push_back(ui);
    }
}

// Helper function to count UTF-8 characters (not bytes)
static int utf8_strlen(const char* str) {
    if (!str) return 0;
    int count = 0;
    const unsigned char *u = (const unsigned char*)str;
    while (*u) {
        if ((*u & 0xC0) != 0x80) count++;
        u++;
    }
    return count;
}

// Stubs for functions that need to be re-implemented in your port
static int get_current_keyval() {
    if (editor_view) {
        return editor_view->last_key;
    }
    return 0;
}

static void clear_current_keyval() {
    if (editor_view) {
        editor_view->last_key = 0;
    }
}

static void scroll_to_cursor(Fl_Text_Buffer*, double) {
    // no-op stub here; implement as needed for your view
}

// Buffer modified callback: records user edits (but not programmatic edits)
static void buffer_modified_cb(int pos, int nInserted, int nDeleted, int nRestyled, const char* deletedText, void* cbArg)
{
    if (undo_in_progress) {
        // we are performing undo/redo; ignore modifications triggered by us
        return;
    }

    MainWindow* window = (MainWindow*)cbArg;

    if (nInserted > 0) {
        char* text = text_buffer->text_range(pos, pos + nInserted);
        int char_count = utf8_strlen(text);
        undo_create_undo_info(INS, pos, pos + char_count, text);
        free(text);
    } else if (nDeleted > 0) {
        int keyval = get_current_keyval();
        char command = (keyval == FL_BackSpace) ? BS : DEL;
        int char_count = utf8_strlen(deletedText);
        undo_create_undo_info(command, pos, pos + char_count, deletedText);
    }

    // Update window changed flag (best-effort) — don't flip if main_window is nullptr
    if (window) {
        // user edits imply document changed; toggle appropriately in check function
        undo_check_modified_step();
    }
}

void undo_init(Fl_Text_Buffer *buffer, void* view, void* window, Fl_Widget* undo_button, Fl_Widget* redo_button)
{
    text_buffer = buffer;
    editor_view = (EditorView*)view;
    undo_w = undo_button;
    redo_w = redo_button;
    main_window = (MainWindow*)window;

    if (text_buffer)
        text_buffer->add_modify_callback(buffer_modified_cb, window);

    ui_tmp = (UndoInfo*)malloc(sizeof(UndoInfo));
    if (ui_tmp) {
        ui_tmp->command = INS;
        ui_tmp->start = ui_tmp->end = 0;
        ui_tmp->seq = false;
        ui_tmp->str = nullptr;
        ui_tmp->is_group = false;
        ui_tmp->children = nullptr;
    }
    undo_clear_all();
}

void undo_clear_all()
{
    // Clear both lists (freeing memory recursively)
    undo_clear_info_list(undo_list);
    undo_clear_info_list(redo_list);

    // reset internal state
    undo_reset_modified_step();

    if (undo_w) undo_w->deactivate();
    if (redo_w) redo_w->deactivate();

    if (ui_tmp) {
        ui_tmp->command = INS;
        ui_tmp->start = ui_tmp->end = 0;
    }
    undo_str.clear();
    prev_keyval = 0;

    // clear any group stack (shouldn't normally be non-empty)
    while (!group_stack.empty()) {
        std::list<UndoInfo*> *lst = group_stack.back();
        group_stack.pop_back();
        undo_clear_info_list(*lst);
        delete lst;
    }
}

void undo_reset_modified_step()
{
    undo_flush_temporal_buffer();
    modified_step = (int)undo_list.size();
    DV(printf("undo_reset_modified_step: Reseted modified_step to %d\n", modified_step));
}

static void undo_check_modified_step()
{
    if (!text_buffer) return;

    bool flag = (modified_step == (int)undo_list.size());
    MainWindow* window = main_window;
    if (window) {
        if (window->changed == flag) {
            window->changed = !flag;
        }
    }
}

void undo_set_sequency(bool seq)
{
    if (!undo_list.empty())
        undo_list.back()->seq = seq;
    DV(printf("<undo_set_sequency: %d>\n", seq));
}

void undo_set_sequency_reserve()
{
    seq_reserve = true;
}

//
// Grouped transactions API
//
void undo_begin_group()
{
    // create a new list for the current group and push onto stack
    std::list<UndoInfo*> *lst = new std::list<UndoInfo*>();
    group_stack.push_back(lst);
    DV(printf("undo_begin_group: depth=%zu\n", group_stack.size()));
}

void undo_end_group()
{
    if (group_stack.empty()) {
        DV(printf("undo_end_group: called without matching begin\n"));
        return;
    }

    // flush any buffered temporal partial operation (so it will become part of the group)
    undo_flush_temporal_buffer();

    // pop the group's child list
    std::list<UndoInfo*> *children = group_stack.back();
    group_stack.pop_back();

    // If group is empty, free and do nothing
    if (children->empty()) {
        delete children;
        return;
    }

    // Create a group container UndoInfo
    UndoInfo* group_ui = (UndoInfo*)malloc(sizeof(UndoInfo));
    group_ui->command = 'G'; // arbitrary indicator (not INS/BS/DEL)
    group_ui->start = 0;
    group_ui->end = 0;
    group_ui->seq = false;
    group_ui->str = nullptr;
    group_ui->is_group = true;
    group_ui->children = children;

    // push into parent group or main undo_list
    push_undoinfo_to_container(group_ui);

    // when a user makes a new edit after grouping, redo history should be cleared.
    // Important: only clear redo when we are not currently performing undo/redo.
    if (!undo_in_progress) {
        undo_clear_info_list(redo_list);
        if (redo_w) redo_w->deactivate();
    }

    if (undo_w) undo_w->activate();
    DV(printf("undo_end_group: created group with %zu children depth=%zu\n", children->size(), group_stack.size()));
}

static void undo_flush_temporal_buffer()
{
    if (undo_str.empty()) return;

    // Build a single UndoInfo for the buffered text and place it in the current container (group or undo_list)
    UndoInfo* ui = (UndoInfo*)malloc(sizeof(UndoInfo));
    ui->command = ui_tmp->command;
    ui->start = ui_tmp->start;
    ui->end = ui_tmp->end;
    ui->seq = seq_reserve;
    ui->str = strdup(undo_str.c_str());
    ui->is_group = false;
    ui->children = nullptr;

    seq_reserve = false;

    push_undoinfo_to_container(ui);
    undo_str.clear();

    // clear redo on real user edits (not during undo/redo)
    if (!undo_in_progress) {
        undo_clear_info_list(redo_list);
        if (redo_w) redo_w->deactivate();
    }
}

void undo_undo()
{
    while (undo_undo_real()) {}
}

void undo_redo()
{
    while (undo_redo_real()) {}
}

// Apply action for a single UndoInfo; if ui->is_group, iterate children appropriately
static void apply_undoinfo_action(UndoInfo* ui, bool is_undo)
{
    if (!ui) return;

    if (!ui->is_group) {
        switch (ui->command) {
        case INS:
            if (is_undo) {
                // undo of INSERT is remove
                text_buffer->remove(ui->start, ui->end);
            } else {
                // redo of INSERT is insert
                text_buffer->insert(ui->start, ui->str);
            }
            break;
        default:
            // BS/DEL are both deletions originally; undo (recreate text) => insert, redo => remove
            if (is_undo) {
                text_buffer->insert(ui->start, ui->str);
            } else {
                text_buffer->remove(ui->start, ui->end);
            }
            break;
        }
    } else {
        // group: either run children in reverse (undo) or forward (redo)
        if (is_undo) {
            // reverse order
            for (auto it = ui->children->rbegin(); it != ui->children->rend(); ++it) {
                apply_undoinfo_action(*it, true);
            }
        } else {
            for (auto it = ui->children->begin(); it != ui->children->end(); ++it) {
                apply_undoinfo_action(*it, false);
            }
        }
    }
}

static bool undo_undo_real()
{
    undo_flush_temporal_buffer();

    if (undo_list.empty()) {
        if (undo_w) undo_w->deactivate();
        return false;
    }

    // take last undo entry
    UndoInfo *ui = undo_list.back();
    undo_list.pop_back();

    // bracket programmatic edits to avoid generating new undo entries
    undo_in_progress = true;
    apply_undoinfo_action(ui, /*is_undo=*/true);
    undo_in_progress = false;

    // move group/item to redo_list as a single unit
    redo_list.push_back(ui);

    DV(printf("undo_undo_real: undo left = %zu, redo left = %zu\n", undo_list.size(), redo_list.size()));

    if (undo_list.empty()) {
        if (undo_w) undo_w->deactivate();
    } else {
        if (undo_list.back()->seq) {
            // there is more sequential undo entries, continue
            if (redo_w) redo_w->activate();
            if (undo_w) undo_w->activate();
            undo_check_modified_step();
            return true;
        }
    }

    if (redo_w) redo_w->activate();
    text_buffer->select(ui->start, ui->start);
    scroll_to_cursor(text_buffer, 0.05);

    undo_check_modified_step();
    return false;
}

static bool undo_redo_real()
{
    if (redo_list.empty()) {
        if (redo_w) redo_w->deactivate();
        return false;
    }

    // take last redo entry
    UndoInfo *ri = redo_list.back();
    redo_list.pop_back();

    undo_in_progress = true;
    apply_undoinfo_action(ri, /*is_undo=*/false);
    undo_in_progress = false;

    // move the unit back to undo_list
    undo_list.push_back(ri);

    DV(printf("undo_redo_real: undo left = %zu, redo left = %zu\n", undo_list.size(), redo_list.size()));

    if (!ri->seq) {
        // if not a chained sequence, stop here; else the caller will loop again
    }

    if (redo_list.empty()) {
        if (redo_w) redo_w->deactivate();
    }
    if (undo_w) undo_w->activate();

    text_buffer->select(ri->start, ri->start);
    scroll_to_cursor(text_buffer, 0.05);

    undo_check_modified_step();
    return ri->seq;
}

// Recursively free UndoInfo structures and their children
static void undo_clear_info_list(std::list<UndoInfo*>& info_list)
{
    for (auto const& i : info_list) {
        if (!i) continue;
        if (i->is_group) {
            if (i->children) {
                // recursively clear children
                undo_clear_info_list(*i->children);
                delete i->children;
                i->children = nullptr;
            }
        } else {
            if (i->str) {
                free(i->str);
                i->str = nullptr;
            }
        }
        free(i);
    }
    info_list.clear();
}

static void undo_append_undo_info(UndoInfo* ui)
{
    if (!ui) return;
    push_undoinfo_to_container(ui);
    seq_reserve = false;
    // clear redo if this is a new user edit (and not during undo/redo)
    if (!undo_in_progress) {
        undo_clear_info_list(redo_list);
        if (redo_w) redo_w->deactivate();
    }
    if (undo_w) undo_w->activate();
}

// Create a new UndoInfo for a user edit event. This respects grouping (pushes into current group if present).
static void undo_create_undo_info(char command, int start, int end, const char* text)
{
    bool seq_flag = false;
    char *str = strdup(text);
    int keyval = get_current_keyval();
    int text_len = utf8_strlen(text);

    // Attempt to merge into temporal buffer if applicable (simple char-by-char sequence merging)
    if (!undo_str.empty()) {
        if ((text_len == 1) && (command == ui_tmp->command)) {
            switch (keyval) {
            case FL_BackSpace:
                if (end == ui_tmp->start)
                    seq_flag = true;
                break;
            case FL_Delete:
                if (start == ui_tmp->start)
                    seq_flag = true;
                break;
            case FL_Tab:
            case ' ':
                if (start == ui_tmp->end)
                    seq_flag = true;
                break;
            default:
                if (start == ui_tmp->end)
                    if (keyval && keyval < 0xF000)
                        switch (prev_keyval) {
                        case FL_Enter:
                        case FL_Tab:
                        case ' ':
                            break;
                        default:
                            seq_flag = true;
                        }
            }
        }
        if (seq_flag) {
            switch (command) {
            case BS:
                undo_str.insert(0, str);
                ui_tmp->start--;
                break;
            default:
                undo_str.append(str);
                ui_tmp->end++;
            }
            // editing while typing: clear redo (only when not programmatic)
            if (!undo_in_progress) {
                undo_clear_info_list(redo_list);
                if (redo_w) redo_w->deactivate();
            }
            prev_keyval = keyval;
            if (undo_w) undo_w->activate();
            free(str);
            return;
        }
        // flush previous buffered temporal thing as a real UndoInfo
        UndoInfo* buffered = (UndoInfo*)malloc(sizeof(UndoInfo));
        buffered->command = ui_tmp->command;
        buffered->start = ui_tmp->start;
        buffered->end = ui_tmp->end;
        buffered->seq = ui_tmp->seq;
        buffered->str = strdup(undo_str.c_str());
        buffered->is_group = false;
        buffered->children = nullptr;
        undo_append_undo_info(buffered);
        undo_str.clear();
    }

    if (!keyval && prev_keyval)
        undo_set_sequency(true);

    if (text_len == 1 &&
        ((keyval && keyval < 0xF000) ||
         keyval == FL_BackSpace || keyval == FL_Delete || keyval == FL_Tab)) {
        ui_tmp->command = command;
        ui_tmp->start = start;
        ui_tmp->end = end;
        undo_str.clear();
        undo_str.append(str);
    } else {
        // allocate a single UndoInfo item and push into container/group
        UndoInfo* ui = (UndoInfo*)malloc(sizeof(UndoInfo));
        ui->command = command;
        ui->start = start;
        ui->end = end;
        ui->seq = seq_reserve;
        ui->str = strdup(str);
        ui->is_group = false;
        ui->children = nullptr;
        undo_append_undo_info(ui);
    }

    // After making a concrete edit, clear redo (unless undo_in_progress)
    if (!undo_in_progress) {
        undo_clear_info_list(redo_list);
        if (redo_w) redo_w->deactivate();
    }

    prev_keyval = keyval;
    clear_current_keyval();

    if (undo_w) undo_w->activate();
    if (redo_w) redo_w->deactivate();

    free(str);
}
