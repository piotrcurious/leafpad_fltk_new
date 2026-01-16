desktopdir=$(datadir)/applications
desktop_DATA=leafpad-fltk.desktop

man_MANS=leafpad-fltk.1

EXTRA_DIST=$(man_MANS) $(desktop_DATA)

dist-hook:
	cd $(distdir); rm -f $(desktop_DATA)

# Rule to substitute @VERSION@ in the desktop file
# This is a workaround to avoid a full gettext setup
%.desktop: %.desktop.in Makefile
	$(AM_V_GEN) sed -e 's|@VERSION@|$(VERSION)|g' $< > $@
