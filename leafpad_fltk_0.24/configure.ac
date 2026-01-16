AC_INIT([leafpad-fltk], [0.24], [jason@jasonwysotski.com])
AC_CONFIG_SRCDIR([src/main_fltk.cpp])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_PROG_CXX
AC_PROG_CC

PKG_CHECK_MODULES([FLTK], [fltk >= 1.3])
AC_SUBST(FLTK_CFLAGS)
AC_SUBST(FLTK_LIBS)

GETTEXT_PACKAGE="leafpad-fltk"
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Gettext package])

IT_PROG_INTLTOOL([0.35.0])

AC_CONFIG_FILES([
    Makefile
    src/Makefile
    data/Makefile
    data/leafpad-fltk.desktop.in
])

AC_OUTPUT
