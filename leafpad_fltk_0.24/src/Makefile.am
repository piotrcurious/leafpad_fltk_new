bin_PROGRAMS = leafpad-fltk
leafpad_fltk_SOURCES = \
    main_fltk.cpp \
    window_fltk.cpp \
    window_fltk.h \
    view_fltk.cpp \
    view_fltk.h \
    callback_fltk.cpp \
    callback_fltk.h \
    dialog_fltk.cpp \
    dialog_fltk.h \
    search_widget.cpp \
    search_widget.h \
    UndoManager.cpp \
    UndoManager.h \
    undo.cpp \
    undo.h

leafpad_fltk_CPPFLAGS = $(FLTK_CFLAGS)
leafpad_fltk_LDADD = $(FLTK_LIBS)
AM_CPPFLAGS = -DLOCALEDIR='"$(localedir)"'
