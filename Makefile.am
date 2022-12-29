## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

## Additional flags to pass to aclocal when it is invoked automatically at
## make time. The ${ACLOCAL_FLAGS} variable is picked up from the environment
## to provide a way for the user to supply additional arguments.
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4

AM_CFLAGS =
AM_CXXFLAGS =
AM_LDFLAGS =
AM_LIBS =

## Only use the compiler flags when we build a debug version.
if DEBUG
COMMON_DEBUG_OPTIONS = -Wall -Wextra -W -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align \
	-Wmissing-declarations -Wno-missing-braces -Wno-missing-field-initializers \
	-Wformat=1 -Wswitch-default -Wswitch-enum -Wstrict-overflow=2 \
	-Wundef -Wunreachable-code -Wlogical-op -Wfloat-equal -Wno-float-equal \
	-Wstrict-aliasing=2 -Wredundant-decls -Werror \
	-fno-omit-frame-pointer -ffloat-store -fno-common -fstrict-aliasing \
	-fvar-tracking -fvar-tracking-assignments -ggdb3 -O0
AM_CFLAGS += $(COMMON_DEBUG_OPTIONS) \
	-Wbad-function-cast -Wstrict-prototypes -Wmissing-prototypes -Wnested-externs \
	-Wold-style-definition
AM_CXXFLAGS += $(COMMON_DEBUG_OPTIONS) \
	-Wconversion-null -Wctor-dtor-privacy -Wdelete-non-virtual-dtor \
	-Wnarrowing -Wnoexcept -Wnon-virtual-dtor -Wreorder -Wstrict-null-sentinel \
	-Wno-non-template-friend -Wold-style-cast -Woverloaded-virtual -Wno-pmf-conversions \
	-Wsign-promo
else
AM_CFLAGS += -flto
AM_CXXFLAGS += -flto
endif

if RELEASE
AM_CFLAGS += -O3
AM_CXXFLAGS += -O3
endif

if COVERAGE
AM_CFLAGS += -fprofile-arcs -ftest-coverage
AM_CXXFLAGS += -fprofile-arcs -ftest-coverage
AM_LDFLAGS += -fprofile-arcs -ftest-coverage
AM_LIBS += -lgcov
endif

export AM_CFLAGS
export AM_CXXFLAGS
export AM_LDFLAGS
export AM_LIBS

## Install the generated pkg-config file (.pc) into the expected location for
## architecture-dependent package configuration information.  Occasionally,
## pkg-config files are also used for architecture-independent data packages,
## in which case the correct install location would be $(datadir)/pkgconfig.
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libfilehandler.pc

lib_LTLIBRARIES = libfilehandler.la
libfilehandler_la_CPPFLAGS = -Iinclude -DLOCALEDIR=\"$(localedir)\" -DPKGLIBDIR=\"$(pkglibdir)\" @PLUGINFRAMEWORK_CFLAGS@ @CPPLOGGER_CFLAGS@

## Define the source file list for the "libexample-@EXAMPLE_API_VERSION@.la"
## target.  Note that @EXAMPLE_API_VERSION@ is not interpreted by Automake and
## will therefore be treated as if it were literally part of the target name,
## and the variable name derived from that.
## Note that it is not necessary to list header files
## which are already listed elsewhere in a _HEADERS variable assignment.
libfilehandler_la_SOURCES = src/FileHandlerManager.cpp \
	src/handler/HandlerBase.cpp \
	src/handler/HandlerPlainReader.cpp \
	src/handler/HandlerPlainWriter.cpp \
	src/handler/HandlerReaderBase.cpp \
	src/handler/HandlerWriterBase.cpp \
	src/utils/Iobuf.cpp \
	src/utils/Strings.cpp

libfilehandler_la_LIBADD = @CPPLOGGER_LIBS@

## Instruct libtool to include ABI version information in the generated shared
## library file (.so).  The library ABI version is defined in configure.ac, so
## that all version information is kept in one place.
libfilehandler_la_LDFLAGS = -version-info @LIBRARY_CURRENT_VERSION@:@LIBRARY_REVISION_VERSION@:@LIBRARY_AGE_VERSION@

noinst_HEADERS = src/handler/HandlerPlainReader.hpp \
	src/handler/HandlerPlainWriter.hpp

## Define the list of public header files and their install location.  The
## nobase_ prefix instructs Automake to not strip the directory part from each
## filename, in order to avoid the need to define separate file lists for each
## installation directory.  This only works if the directory hierarchy in the
## source tree matches the hierarchy at the install location, however.
libfilehandler_includedir = $(includedir)
libfilehandler_include_HEADERS = include/filehandler.hpp

libfilehandler_sub_includedir = $(includedir)/filehandler
libfilehandler_sub_include_HEADERS = include/filehandler/FileHandlerManager.hpp \
	include/filehandler/PluginHelper.hpp

libfilehandler_sub_handler_includedir = $(includedir)/filehandler/handler
libfilehandler_sub_handler_include_HEADERS = include/filehandler/handler/HandlerBase.hpp \
	include/filehandler/handler/HandlerReaderBase.hpp \
	include/filehandler/handler/HandlerWriterBase.hpp

libfilehandler_sub_utils_includedir = $(includedir)/filehandler/utils
libfilehandler_sub_utils_include_HEADERS = include/filehandler/utils/Iobuf.hpp \
	include/filehandler/utils/IobufBase.hpp \
	include/filehandler/utils/Strings.hpp \
	include/filehandler/utils/StringsBase.hpp \
	include/filehandler/utils/dynamic_unique_ptr_cast.hpp

libfilehandler_sub_types_includedir = $(includedir)/filehandler/types
libfilehandler_sub_types_include_HEADERS = include/filehandler/types/PluginDetails.hpp \
	include/filehandler/types/HandlerCreatorFunc.hpp \
	include/filehandler/types/PluginCreatorFunc.hpp \
	include/filehandler/types/BasePlugin.hpp

libfilehandler_sub_traits_includedir = $(includedir)/filehandler/traits
libfilehandler_sub_traits_include_HEADERS = include/filehandler/traits/common.hpp \
	include/filehandler/traits/HandlerReaderTrait.hpp \
	include/filehandler/traits/HandlerWriterTrait.hpp \
	include/filehandler/traits/ProviderReaderTrait.hpp \
	include/filehandler/traits/ProviderWriterTrait.hpp

libfilehandler_sub_provider_includedir = $(includedir)/filehandler/provider
libfilehandler_sub_provider_include_HEADERS = include/filehandler/provider/ProviderBase.hpp \
	include/filehandler/provider/ProviderReaderBase.hpp \
	include/filehandler/provider/ProviderWriterBase.hpp

libfilehandler_sub_provider_types_route_includedir = $(includedir)/filehandler/provider/types/route
libfilehandler_sub_provider_types_route_include_HEADERS = include/filehandler/provider/types/route/ProviderRouteBase.hpp \
	include/filehandler/provider/types/route/ProviderRouteReaderBase.hpp \
	include/filehandler/provider/types/route/ProviderRouteWriterBase.hpp

## Define the list of public header files and their install location.  The
## nobase_ prefix instructs Automake to not strip the directory part from each
## filename, in order to avoid the need to define separate file lists for each
## installation directory.  This only works if the directory hierarchy in the
## source tree matches the hierarchy at the install location, however.
libfilehandler_sub_provider_types_route_impl_includedir = $(includedir)/filehandler/provider/types/route/impl
libfilehandler_sub_provider_types_route_impl_include_HEADERS = include/filehandler/provider/types/route/impl/ProviderGpsRoute.hpp \
	include/filehandler/provider/types/route/impl/ProviderGpsRouteBase.hpp \
	include/filehandler/provider/types/route/impl/ProviderGpsRouteReader.hpp \
	include/filehandler/provider/types/route/impl/ProviderGpsRouteWriter.hpp


### file handler plugins ###

pkglib_LTLIBRARIES = gpx.la
gpx_la_CPPFLAGS = -Iinclude -DLOCALEDIR=\"$(localedir)\" @GPX_CFLAGS@ @GPSDATA_CFLAGS@ @GPSDATA_UTILS_CFLAGS@ @PLUGINFRAMEWORK_CFLAGS@
gpx_la_LDFLAGS = -module -avoid-version -shared
gpx_la_LIBADD = @GPX_LIBS@ @GPSDATA_LIBS@ @GPSDATA_UTILS_LIBS@ @PLUGINFRAMEWORK_LIBS@
gpx_la_SOURCES = plugins/gpx/plugin.cpp \
	plugins/gpx/Plugin.cpp \
	plugins/gpx/Reader.cpp \
	plugins/gpx/Writer.cpp \
	plugins/gpx/Report.cpp
noinst_HEADERS += plugins/gpx/Plugin.hpp \
	plugins/gpx/Reader.hpp \
	plugins/gpx/Writer.hpp \
	plugins/gpx/Report.hpp

pkglib_LTLIBRARIES += openambit.la
openambit_la_CPPFLAGS = -Iinclude -DLOCALEDIR=\"$(localedir)\" @XML_CFLAGS@ @GPSDATA_CFLAGS@ @GPSDATA_UTILS_CFLAGS@ @PLUGINFRAMEWORK_CFLAGS@
openambit_la_LDFLAGS = -module -avoid-version -shared
openambit_la_LIBADD = @XML_LIBS@ @GPSDATA_LIBS@ @GPSDATA_UTILS_LIBS@ @PLUGINFRAMEWORK_LIBS@
openambit_la_SOURCES = plugins/openambit/plugin.cpp \
	plugins/openambit/Plugin.cpp \
	plugins/openambit/Writer.cpp \
	plugins/openambit/Parser.cpp
noinst_HEADERS += plugins/openambit/Plugin.hpp \
	plugins/openambit/Writer.hpp \
	plugins/openambit/Parser.hpp


### Examples ###
if EXAMPLE
bin_PROGRAMS = read_gpx
read_gpx_LDADD = libfilehandler.la @GPSDATA_LIBS@ @GPSDATA_UTILS_LIBS@ @PLUGINFRAMEWORK_LIBS@
read_gpx_LDFLAGS = -Wl,-rpath=$(pkglibdir),--enable-new-dtags
read_gpx_CPPFLAGS = -Iinclude -DLOCALEDIR=\"$(localedir)\" -DPKGLIBDIR=\"./.libs/\" @GPSDATA_CFLAGS@ @GPSDATA_UTILS_CFLAGS@
read_gpx_SOURCES = examples/read_gpx.cpp

bin_PROGRAMS += write_gpx
write_gpx_LDADD = libfilehandler.la @GPSDATA_LIBS@ @GPSDATA_UTILS_LIBS@ @PLUGINFRAMEWORK_LIBS@
write_gpx_LDFLAGS = -Wl,-rpath=$(pkglibdir),--enable-new-dtags
write_gpx_CPPFLAGS = -Iinclude -DLOCALEDIR=\"$(localedir)\" -DPKGLIBDIR=\"./.libs/\" @GPSDATA_CFLAGS@ @GPSDATA_UTILS_CFLAGS@
write_gpx_SOURCES = examples/write_gpx.cpp

bin_PROGRAMS += read_and_write_gpx
read_and_write_gpx_LDADD = libfilehandler.la @GPSDATA_LIBS@ @GPSDATA_UTILS_LIBS@ @PLUGINFRAMEWORK_LIBS@
read_and_write_gpx_LDFLAGS = -Wl,-rpath=$(pkglibdir),--enable-new-dtags
read_and_write_gpx_CPPFLAGS = -Iinclude -DLOCALEDIR=\"$(localedir)\" -DPKGLIBDIR=\"./.libs/\" @GPSDATA_CFLAGS@ @GPSDATA_UTILS_CFLAGS@
read_and_write_gpx_SOURCES = examples/read_and_write_gpx.cpp

bin_PROGRAMS += write_and_read_gpx
write_and_read_gpx_LDADD = libfilehandler.la @GPSDATA_LIBS@ @GPSDATA_UTILS_LIBS@ @PLUGINFRAMEWORK_LIBS@
write_and_read_gpx_LDFLAGS = -Wl,-rpath=$(pkglibdir),--enable-new-dtags
write_and_read_gpx_CPPFLAGS = -Iinclude -DLOCALEDIR=\"$(localedir)\" -DPKGLIBDIR=\"./.libs/\" @GPSDATA_CFLAGS@ @GPSDATA_UTILS_CFLAGS@ @PLUGINFRAMEWORK_CFLAGS@
write_and_read_gpx_SOURCES = examples/write_and_read_gpx.cpp

bin_PROGRAMS += read_openambit
read_openambit_LDADD = libfilehandler.la @GPSDATA_LIBS@ @GPSDATA_UTILS_LIBS@ @PLUGINFRAMEWORK_LIBS@
read_openambit_LDFLAGS = -Wl,-rpath=$(pkglibdir),--enable-new-dtags
read_openambit_CPPFLAGS = -Iinclude -DLOCALEDIR=\"$(localedir)\" -DPKGLIBDIR=\"./.libs/\" @GPSDATA_CFLAGS@ @GPSDATA_UTILS_CFLAGS@
read_openambit_SOURCES = examples/read_openambit.cpp
endif

EXTRA_DIST = VERSION ChangeLog README AUTHORS LICENSE @PROGRAM_NAME@.spec

## Only try to update the ChangeLog file when git is availible to report all the changes.
if HAVE_GIT
## Force to recreate the ChangeLog file every time
.PHONY: ChangeLog VERSION

## Dynamicly create the changelog file when a distribution tarball is created.
ChangeLog:
	-$(GIT) log --decorate > ChangeLog

VERSION:
	-$(GIT) describe --tags --match=[0-9]* --long --dirty | $(SED) 's/^v//' > VERSION

distclean-local:
	@rm -f ChangeLog VERSION
endif

if COVERAGE
COVERAGE_INFO_FILE = $(top_builddir)/coverage.info
COVERAGE_REPORT_DIR = $(top_builddir)/coverage

.PHONY: coverage coverage-report clean-coverage-report clean-coverage

coverage: clean-coverage all-am check-am check coverage-report

coverage-report: check
	@echo "Start to create coverage reports..."
	$(LCOV) --capture \
		--directory "$(top_builddir)/include" \
		--directory "$(top_builddir)/src" \
		--directory "$(top_builddir)/tests" \
		--base-directory "$(abspath $(top_builddir))" \
		--output-file $(COVERAGE_INFO_FILE) \
		--gcov-tool $(GCOV) \
		--compat-libtool --checksum
	$(LCOV) --extract $(COVERAGE_INFO_FILE) "$(abspath $(top_srcdir))/include/*" \
		--extract $(COVERAGE_INFO_FILE) "$(abspath $(top_srcdir))/src/*" \
		--extract $(COVERAGE_INFO_FILE) "$(abspath $(top_srcdir))/test/*" \
		--gcov-tool $(GCOV) \
		--output-file $(COVERAGE_INFO_FILE)
	$(GENHTML) --prefix "$(abspath $(top_srcdir))" \
		--output-directory $(COVERAGE_REPORT_DIR) \
                --title $(PACKAGE_NAME) \
		--legend --show-details \
		$(GENHTML_OPTIONS) \
		$(COVERAGE_INFO_FILE)
	@echo "Successfully created coverage reports into $(COVERAGE_REPORT_DIR) directory."

clean-coverage-report:
	-rm -rf $(COVERAGE_INFO_FILE)
	-rm -rf $(COVERAGE_REPORT_DIR)

clean-coverage: clean-coverage-report
	-$(LCOV) --gcov-tool $(GCOV) --zerocounters --directory $(top_builddir)
	@if xargs --version 2>/dev/null; then \
		find $(top_builddir) -name "*.gcno" | xargs --no-run-if-empty rm; \
	else \
		find $(top_builddir) -name "*.gcno" | xargs rm; \
	fi

clean-local: clean-coverage
endif

if GTEST
SUBDIRS = tests
endif
