check_PROGRAMS = test-Iobuf
test_Iobuf_SOURCES = test-Iobuf.cpp
test_Iobuf_LDADD = @GTEST_LIBS@ @GMOCK_LIBS@ ../libfilehandler.la
test_Iobuf_LDFLAGS = -pthread
test_Iobuf_CPPFLAGS = @GTEST_CFLAGS@ @GMOCK_CFLAGS@ -pthread -I../include -I../src

check_PROGRAMS += test-Iostream-Iobuf
test_Iostream_Iobuf_SOURCES = test-Iostream-Iobuf.cpp
test_Iostream_Iobuf_LDADD = @GTEST_LIBS@ @GMOCK_LIBS@ ../libfilehandler.la
test_Iostream_Iobuf_LDFLAGS = -pthread
test_Iostream_Iobuf_CPPFLAGS = @GTEST_CFLAGS@ @GMOCK_CFLAGS@ -pthread -I../include -I../src

check_PROGRAMS += test-Strings
test_Strings_SOURCES = test-Strings.cpp
test_Strings_LDADD = @GTEST_LIBS@ @GMOCK_LIBS@ ../libfilehandler.la
test_Strings_LDFLAGS = -pthread
test_Strings_CPPFLAGS = @GTEST_CFLAGS@ @GMOCK_CFLAGS@ -pthread -I../include -I../src

TESTS = test-Iobuf test-Iostream-Iobuf test-Strings
