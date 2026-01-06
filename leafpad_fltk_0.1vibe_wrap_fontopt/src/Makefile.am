bin_PROGRAMS = leafpad

leafpad_SOURCES = \
	main_fltk.cpp \
	window_fltk.h window_fltk.cpp \
	view_fltk.h view_fltk.cpp \
	callback_fltk.h callback_fltk.cpp \
	dialog_fltk.h dialog_fltk.cpp \
	UndoManager.h UndoManager.cpp \
	undo.h undo.cpp \
	i18n.h

leafpad_LDADD    = $(FLTK_LIBS) $(INTLLIBS)
leafpad_CXXFLAGS = $(FLTK_CFLAGS)
