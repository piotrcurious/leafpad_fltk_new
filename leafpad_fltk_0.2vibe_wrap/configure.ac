AC_INIT(leafpad, 0.8.19, tarot@sdf.org)
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS(config.h)

AC_PROG_CC
AC_PROG_CXX
if test "$GCC" = "yes"; then
	CFLAGS="$CFLAGS -Wall"
fi
AM_PROG_CC_C_O

AC_PATH_PROG(FLTK_CONFIG, fltk-config, no)
if test "$FLTK_CONFIG" = "no"; then
   AC_MSG_ERROR([fltk-config not found. Please install the FLTK development package.])
fi

FLTK_CFLAGS=`$FLTK_CONFIG --cxxflags`
FLTK_LIBS=`$FLTK_CONFIG --ldflags`

AC_SUBST(FLTK_CFLAGS)
AC_SUBST(FLTK_LIBS)

AC_ARG_ENABLE(emacs,
	AC_HELP_STRING(--enable-emacs, [implement Emacs key theme (experimental)]),
	AC_DEFINE(ENABLE_EMACS, 1, [Define if Emacs key theme is enabled.])
)

GETTEXT_PACKAGE="$PACKAGE"
AC_SUBST(GETTEXT_PACKAGE)

AC_OUTPUT([
	Makefile
	src/Makefile
	data/Makefile
	data/icons/Makefile
	data/icons/16x16/Makefile
	data/icons/22x22/Makefile
	data/icons/24x24/Makefile
	data/icons/32x32/Makefile
	data/icons/scalable/Makefile
])
