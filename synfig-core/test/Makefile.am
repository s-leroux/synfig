# $Id$

MAINTAINERCLEANFILES=Makefile.in

check_LTLIBRARIES = libgtest.la
libgtest_la_SOURCES = ../../googletest/googletest/src/gtest-all.cc
libgtest_la_CPPFLAGS = -I$(top_srcdir)/../googletest/googletest/include -I$(top_srcdir)/../googletest/googletest
libgtest_la_LDFLAGS = -pthread

AM_CXXFLAGS=@CXXFLAGS@ @ETL_CFLAGS@ -I$(top_builddir) -I$(top_srcdir)/src
check_PROGRAMS=$(TESTS)

TESTS=gtest

gtest_SOURCES= \
  bone.cpp \
  gtest.cpp
gtest_LDADD = libgtest.la
gtest_LDFLAGS = -pthread
gtest_CPPFLAGS = -I$(top_srcdir)/../googletest/googletest/include -I$(top_srcdir)/../googletest/googletest -pthread
