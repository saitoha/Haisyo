#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([haisyo], [0.2.0], [user@zuse.jp])
AC_CONFIG_SRCDIR([main.c])
AC_CONFIG_HEADERS([config.h])

cross_compiling=yes

# Checks for mingw compiler.
AC_PROG_CC([i686-pc-mingw32-gcc \
            i586-pc-mingw32-gcc \
            i486-pc-mingw32-gcc \
            i386-pc-mingw32-gcc \
            i686-mingw32-gcc \
            i586-mingw32-gcc \
            i486-mingw32-gcc \
            i386-mingw32-gcc \
            i686-w64-mingw32-gcc \
            i586-w64-mingw32-gcc \
            i486-w64-mingw32-gcc \
            i386-w64-mingw32-gcc])

# Checks for mingw resource compiler.
AC_CHECK_PROGS([RC],
              [i686-pc-mingw32-windres \
               i586-pc-mingw32-windres \
               i486-pc-mingw32-windres \
               i386-pc-mingw32-windres \
               i686-mingw32-windres \
               i586-mingw32-windres \
               i486-mingw32-windres \
               i386-mingw32-windres \
               i686-w64-mingw32-windres \
               i586-w64-mingw32-windres \
               i486-w64-mingw32-windres \
               i386-w64-mingw32-windres])
if test -z "${RC}"; then
    AC_MSG_ERROR(windres command is not found.);
fi

AC_CHECK_PROG(ZIP, zip, zip)
if test -z "${ZIP}"; then
    AC_MSG_ERROR(zip is not found.);
fi

# Checks for libraries.
AC_CHECK_LIB([gdi32], [main], [], AC_MSG_ERROR([gdi32 is missing]))
AC_CHECK_LIB([ole32], [main], [], AC_MSG_ERROR([ole32 is missing]))
AC_CHECK_LIB([shell32], [main], [], AC_MSG_ERROR([shell32 is missing]))
AC_CHECK_LIB([user32], [main], [], AC_MSG_ERROR([user32 is missing]))
AC_CHECK_LIB([uuid], [main], [], AC_MSG_ERROR([uuid is missing]))

# Checks for header files.
AC_CHECK_HEADERS([windows.h shlwapi.h shlobj.h],
                [],
                [AC_MSG_ERROR([shit!]) ])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_DECLS([lstrlen],
               [AC_DEFINE(HAVE_SHGETFOLDERPATH)],
               [AC_MSG_ERROR([oops! lstrlen is missing])],
               [
                #include <shlwapi.h>
               ])
AC_CHECK_DECLS([DSTERASE],
               [AC_DEFINE(HAVE_DSTERASE)],
               [],
               [
                #include <windows.h>
               ])
AC_CHECK_DECLS([SHGetFolderPathA],
               [AC_DEFINE(HAVE_SHGETFOLDERPATH)],
               [
                AC_CHECK_DECLS([SHGetSpecialFolderPathA],
                               [AC_DEFINE(HAVE_SHGETSPECIALFOLDERPATH)],
                               [AC_MSG_ERROR([fuck! both of SHGetFolderPathA and SHGetSpecialFolderPathA are missing])],
                               [
                                #include <shlwapi.h>
                               ])
               ],
               [
                #include <shlwapi.h>
               ])

# Checks for library functions.
AC_CHECK_FUNCS([strstr strcat strcmp memcpy], [], [AC_MSG_ERROR([omg!])])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
