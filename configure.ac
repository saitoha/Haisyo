#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.59])
AC_INIT([haisyo], [1.2.0], [user@zuse.jp])
AC_CONFIG_SRCDIR([main.c])
AC_CONFIG_HEADERS([config.h])

cross_compiling=yes

AC_ARG_ENABLE([64bit],
               AS_HELP_STRING([--enable-64bit], [do not build 64bit binary]),
                              [],
                              [enable_64bit=yes])


# Checks for mingw compiler.
if test x$enable_64bit = xyes; then
        AC_PROG_CC([x86_64-w64-mingw32-gcc])
else
        AC_PROG_CC([x86_64-w64-mingw32-gcc mingw32-gcc \
                    i686-mingw32-gcc \
                    i586-mingw32-gcc \
                    i486-mingw32-gcc \
                    i386-mingw32-gcc \
                    i686-w64-mingw32-gcc \
                    i586-w64-mingw32-gcc \
                    i486-w64-mingw32-gcc \
                    i386-w64-mingw32-gcc \
                    i686-pc-mingw32-gcc \
                    i586-pc-mingw32-gcc \
                    i486-pc-mingw32-gcc \
                    i386-pc-mingw32-gcc])
fi

# Checks for mingw resource compiler.
if test x$enable_64bit = xyes; then
        AC_CHECK_PROGS([RC], [x86_64-w64-mingw32-windres])
else
        AC_CHECK_PROGS([RC],
                      [x86_64-w64-mingw32-windres mingw32-windres \
                       i686-mingw32-windres \
                       i586-mingw32-windres \
                       i486-mingw32-windres \
                       i386-mingw32-windres \
                       i686-w64-mingw32-windres \
                       i586-w64-mingw32-windres \
                       i486-w64-mingw32-windres \
                       i386-w64-mingw32-windres \
                       i686-pc-mingw32-windres \
                       i586-pc-mingw32-windres \
                       i486-pc-mingw32-windres \
                       i386-pc-mingw32-windres])
fi

if test -z "${RC}"; then
    AC_MSG_ERROR(windres command is not found.);
fi

AC_CHECK_PROG(ZIP, zip, zip)
AC_CHECK_PROG(TAR, tar, tar)

# Checks for header files.
AC_CHECK_HEADERS([windows.h],
                [],
                [AC_MSG_ERROR([shit!]) ])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_DECLS([lstrlen],
               [AC_DEFINE(HAVE_SHGETFOLDERPATH)],
               [AC_MSG_ERROR([oops! lstrlen is missing.])],
               [
                #include <windows.h>
               ])
AC_CHECK_DECLS([DSTERASE],
               [AC_DEFINE(HAVE_DSTERASE)],
               [],
               [
                #include <windows.h>
               ])
# Checks for library functions.
AC_CHECK_FUNCS([strstr strcmp memcpy], [], [AC_MSG_ERROR([omg!])])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
