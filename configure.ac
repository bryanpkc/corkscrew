AC_INIT(corkscrew, 2.0)
AC_CONFIG_SRCDIR(corkscrew.c)
AM_CONFIG_HEADER(config.h:config.in)
AM_INIT_AUTOMAKE

AC_PROG_CC
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_LN_S

AC_CANONICAL_HOST
case "$host_os" in
  solaris*)
    AC_CHECK_LIB(xnet, socket)
    AC_CHECK_LIB(xnet, gethostbyname)
    AC_CHECK_LIB(xnet, inet_addr)
    AC_CHECK_LIB(xnet, connect)
    AC_CHECK_HEADERS(sys/filio.h)
  ;;
esac

AC_HEADER_STDC
AC_HEADER_TIME
dnl AC_CHECK_HEADERS(fcntl.h sys/ioctl.h sys/time.h unistd.h arpa/inet.h errno.h netdb.h netinet/in.h stdio.h stdlib.h string.h sys/socket.h sys/types.h)

dnl AC_CHECK_TYPE

AC_CHECK_FUNCS(select socket strerror)
dnl check for working const (hpux)
AC_C_CONST

AC_OUTPUT(Makefile)
