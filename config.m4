dnl config.m4 for extension easy_aop

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(easy_aop, for easy_aop support,
dnl Make sure that the comment is aligned:
dnl [  --with-easy_aop             Include easy_aop support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(easy_aop, whether to enable easy_aop support,
dnl Make sure that the comment is aligned:
[  --enable-easy_aop          Enable easy_aop support], no)

if test "$PHP_EASY_AOP" != "no"; then
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
  dnl PHP_EVAL_LIBLINE($LIBFOO_LIBDIR, EASY_AOP_SHARED_LIBADD)
  dnl PHP_EVAL_INCLINE($LIBFOO_CFLAGS)

  dnl # --with-easy_aop -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/easy_aop.h"  # you most likely want to change this
  dnl if test -r $PHP_EASY_AOP/$SEARCH_FOR; then # path given as parameter
  dnl   EASY_AOP_DIR=$PHP_EASY_AOP
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for easy_aop files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       EASY_AOP_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$EASY_AOP_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the easy_aop distribution])
  dnl fi

  dnl # --with-easy_aop -> add include path
  dnl PHP_ADD_INCLUDE($EASY_AOP_DIR/include)

  dnl # --with-easy_aop -> check for lib and symbol presence
  dnl LIBNAME=EASY_AOP # you may want to change this
  dnl LIBSYMBOL=EASY_AOP # you most likely want to change this

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $EASY_AOP_DIR/$PHP_LIBDIR, EASY_AOP_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_EASY_AOPLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong easy_aop lib version or lib not found])
  dnl ],[
  dnl   -L$EASY_AOP_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(EASY_AOP_SHARED_LIBADD)

  dnl # In case of no dependencies
  AC_DEFINE(HAVE_EASY_AOP, 1, [ Have easy_aop support ])

  PHP_REQUIRE_CXX()
  PHP_ADD_LIBRARY(stdc++, 1, EASY_AOP_SHARED_LIBADD)

  easy_aop_source_file="src/entrance.cc \
    src/php_internal.cc \
    src/EasyAop.cc \
    src/AopManager.cc \
    src/AopExtension.cc \
    src/Advice.cc"

  PHP_NEW_EXTENSION(easy_aop, $easy_aop_source_file, $ext_shared,,, cxx)
fi

if test -z "$PHP_DEBUG"; then
        AC_ARG_ENABLE(debug,
                [--enable-debg  compile with debugging system],
                [PHP_DEBUG=$enableval], [PHP_DEBUG=no]
        )
fi
