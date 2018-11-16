# This file is part of the "x0" project, http://github.com/christianparpart/x0>
#   (c) 2009-2017 Christian Parpart <christian@parpart.family>
#
# Licensed under the MIT License (the "License"); you may not use this
# file except in compliance with the License. You may obtain a copy of
# the License at: http://opensource.org/licenses/MIT

EXTRA_DIST = autogen.sh
bin_PROGRAMS =
lib_LIBRARIES =
noinst_LIBRARIES =
check_PROGRAMS =
TESTS =

####### compile opts ##########################################################

AM_CXXFLAGS = \
	$(BASE_CXXFLAGS) \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src

AM_LDFLAGS = \
	-lpthread -ldl

####### libXzero.a ############################################################

lib_LIBRARIES += libcmath.a

libcmath_a_SOURCES = \
	src/cmath/expr.cc \
	src/cmath/expr_parser.cc \
	src/cmath/real.cc

libcmathdir = $(includedir)
nobase_libcmath_HEADERS = \
	src/cmath/expr.h

install-data-hook:
	for file in $(DESTDIR)/$(libcmathdir)/src/*; do \
		mv -v $$file $(DESTDIR)/$(libcmathdir); \
	done
	rmdir $(DESTDIR)/$(libcmathdir)/src

####### TESTS #################################################################

TESTS += cmath_test
check_PROGRAMS += cmath_test

cmath_test_SOURCES = \
	src/cmath/expr_test.cc \
	src/cmath/real_test.cc

cmath_test_LDADD = \
	libcmath.a \
	$(AM_LDFLAGS)

####### bin ###################################################################

bin_PROGRAMS += cm

cm_CXXFLAGS = \
	-I$(top_srcdir)/src \
	$(AM_CXXFLAGS)

cm_LDADD = \
	libcmath.a \
	$(BASE_LDFLAGS) \
	$(AM_LDFLAGS)

if EDITLINE
cm_LDADD += $(EDITLINE_LIBS)
else
cm_LDADD += $(READLINE_LIBS)
endif

cm_SOURCES = \
	src/cm/console.cc \
	src/cm/main.cc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libcmath.pc

# vim:ts=8
# EOF
