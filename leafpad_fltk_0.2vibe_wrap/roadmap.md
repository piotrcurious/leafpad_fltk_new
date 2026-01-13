# Leafpad FLTK Porting Roadmap

This document outlines the progress of porting Leafpad from GTK+ to FLTK.

## Core Components

- [x] **Build System (`configure.ac`, `Makefile.am`)**
  - [x] Remove GTK+ dependencies.
  - [x] Add FLTK dependencies.
  - [x] Switch to C++ compilation.
  - [x] Clean up unused dependencies (intltool, gettext).

- [x] **Main Application (`main_fltk.cpp`)**
  - [x] Initialize FLTK.
  - [x] Create the main window.
  - [x] Enter the FLTK event loop.

- [x] **Main Window (`window_fltk.cpp`, `window_fltk.h`)**
  - [x] Create the main window.
  - [x] Create the menu bar.
  - [ ] Implement window resizing.

- [x] **Text View (`view_fltk.cpp`, `view_fltk.h`)**
  - [x] Create the text editor widget.
  - [x] Create the text buffer.

- [x] **Callbacks (`callback_fltk.cpp`, `callback_fltk.h`)**
  - [x] Implement "File -> New".
  - [x] Implement "File -> Open".
  - [x] Implement "File -> Save".
  - [x] Implement "File -> Save As".
  - [x] Implement "File -> Quit".
  - [x] Port "Edit -> Undo" from GTK source.
  - [x] Port "Edit -> Redo" from GTK source.
  - [x] Implement "Edit -> Cut".
  - [x] Implement "Edit -> Copy".
  - [x] Implement "Edit -> Paste".
  - [ ] Implement "Edit -> Delete".
  - [x] Implement "Edit -> Select All".

- [x] **Dialogs (`dialog_fltk.cpp`, `dialog_fltk.h`)**
  - [x] Implement "Help -> About".
  - [ ] Implement search dialog.
  - [ ] Implement jump to line dialog.
  - [ ] Implement font selection dialog.

## Feature Parity

- [ ] **Search and Replace**
- [ ] **Line Numbers**
- [ ] **Word Wrap**
- [ ] **Auto Indent**
- [ ] **Font Selection**
- [ ] **Internationalization (i18n)**

## Known Issues

- [ ] The "Select All" menu item is not working due to a focus issue.
- [ ] The "Edit" menu items are not yet functional.
- [ ] Advanced features from the original Leafpad are not yet implemented.
