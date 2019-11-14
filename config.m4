dnl config.m4 for extension phpaop

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(phpaop, for phpaop support,
dnl Make sure that the comment is aligned:
dnl [  --with-phpaop             Include phpaop support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(phpaop, whether to enable phpaop support,
dnl Make sure that the comment is aligned:
[  --enable-phpaop          Enable phpaop support], no)

if test "$PHP_PHPAOP" != "no"; then
  dnl Write more examples of tests here...

  dnl # get library FOO build options from pkg-config output
  dnl AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
  dnl AC_MSG_CHECKING(for libfoo)
  dnl if test -x "$PKG_CONFIG" && $PKG_CONFIG --exists foo; then
  dnl   if $PKG_CONFIG foo --atleast-version 1.2.3; then
  dnl     LIBFOO_CFLAGS=\`$PKG_CONFIG foo --cflags\`
  dnl     LIBFOO_LIBDIR=\`$PKG_CONFIG foo --libs\`
  dnl     LIBFOO_VERSON=\`$PKG_CONFIG foo --modversion\`
  dnl     AC_MSG_RESULT(from pkgconfig: version $LIBFOO_VERSON)
  dnl   else
  dnl     AC_MSG_ERROR(system libfoo is too old: version 1.2.3 required)
  dnl   fi
  dnl else
  dnl   AC_MSG_ERROR(pkg-config not found)
  dnl fi
  dnl PHP_EVAL_LIBLINE($LIBFOO_LIBDIR, PHPAOP_SHARED_LIBADD)
  dnl PHP_EVAL_INCLINE($LIBFOO_CFLAGS)

  dnl # --with-phpaop -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/phpaop.h"  # you most likely want to change this
  dnl if test -r $PHP_PHPAOP/$SEARCH_FOR; then # path given as parameter
  dnl   PHPAOP_DIR=$PHP_PHPAOP
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for phpaop files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       PHPAOP_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$PHPAOP_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the phpaop distribution])
  dnl fi

  dnl # --with-phpaop -> add include path
  dnl PHP_ADD_INCLUDE($PHPAOP_DIR/include)

  dnl # --with-phpaop -> check for lib and symbol presence
  dnl LIBNAME=PHPAOP # you may want to change this
  dnl LIBSYMBOL=PHPAOP # you most likely want to change this

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $PHPAOP_DIR/$PHP_LIBDIR, PHPAOP_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_PHPAOPLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong phpaop lib version or lib not found])
  dnl ],[
  dnl   -L$PHPAOP_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(PHPAOP_SHARED_LIBADD)

  dnl # In case of no dependencies
  AC_DEFINE(HAVE_PHPAOP, 1, [ Have phpaop support ])

  PHP_REQUIRE_CXX()
  PHP_ADD_LIBRARY(stdc++, 1, PHPAOP_SHARED_LIBADD)

  phpaop_source_file="src/entrance.cc \
    src/php_internal.cc \
    src/PHPAOP.cc \
    src/AopManager.cc \
    src/AopExtension.cc \
    src/Advice.cc"

  PHP_NEW_EXTENSION(phpaop, $phpaop_source_file, $ext_shared)
fi

if test -z "$PHP_DEBUG"; then
        AC_ARG_ENABLE(debug,
                [--enable-debg  compile with debugging system],
                [PHP_DEBUG=$enableval], [PHP_DEBUG=no]
        )
fi

