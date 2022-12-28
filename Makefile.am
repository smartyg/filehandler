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
pkgconfig_DATA = libgpsfile2.pc libgpsfile2-providergpsroute.pc

lib_LTLIBRARIES = libgpsfile2.la
libgpsfile2_la_CPPFLAGS = -Iinclude -DLOCALEDIR=\"$(localedir)\" -DPKGLIBDIR=\"$(pkglibdir)\" @PLUGINFRAMEWORK_CFLAGS@ @CPPLOGGER_CFLAGS@

## Define the source file list for the "libexample-@EXAMPLE_API_VERSION@.la"
## target.  Note that @EXAMPLE_API_VERSION@ is not interpreted by Automake and
## will therefore be treated as if it were literally part of the target name,
## and the variable name derived from that.
## Note that it is not necessary to list header files
## which are already listed elsewhere in a _HEADERS variable assignment.
libgpsfile2_la_SOURCES = src/GpsfileManager.cpp \
	src/handler/HandlerBase.cpp \
	src/handler/HandlerPlainReader.cpp \
	src/handler/HandlerPlainWriter.cpp \
	src/handler/HandlerReaderBase.cpp \
	src/handler/HandlerWriterBase.cpp \
	src/utils/Iobuf.cpp \
	src/utils/Strings.cpp

libgpsfile2_la_LIBADD = @CPPLOGGER_LIBS@

## Instruct libtool to include ABI version information in the generated shared
## library file (.so).  The library ABI version is defined in configure.ac, so
## that all version information is kept in one place.
libgpsfile2_la_LDFLAGS = -version-info @LIBRARY_CURRENT_VERSION@:@LIBRARY_REVISION_VERSION@:@LIBRARY_AGE_VERSION@

noinst_HEADERS = src/handler/HandlerPlainReader.hpp \
	src/handler/HandlerPlainWriter.hpp

## Define the list of public header files and their install location.  The
## nobase_ prefix instructs Automake to not strip the directory part from each
## filename, in order to avoid the need to define separate file lists for each
## installation directory.  This only works if the directory hierarchy in the
## source tree matches the hierarchy at the install location, however.
libgpsfile2_includedir = $(includedir)
libgpsfile2_include_HEADERS = include/libgpsfile2.hpp

libgpsfile2_sub_includedir = $(includedir)/libgpsfile2
libgpsfile2_sub_include_HEADERS = include/libgpsfile2/GpsfileManager.hpp \
	include/libgpsfile2/PluginHelper.hpp

libgpsfile2_sub_handler_includedir = $(includedir)/libgpsfile2/handler
libgpsfile2_sub_handler_include_HEADERS = include/libgpsfile2/handler/HandlerBase.hpp \
	include/libgpsfile2/handler/HandlerReaderBase.hpp \
	include/libgpsfile2/handler/HandlerWriterBase.hpp

libgpsfile2_sub_utils_includedir = $(includedir)/libgpsfile2/utils
libgpsfile2_sub_utils_include_HEADERS = include/libgpsfile2/utils/Iobuf.hpp \
	include/libgpsfile2/utils/IobufBase.hpp \
	include/libgpsfile2/utils/Strings.hpp \
	include/libgpsfile2/utils/StringsBase.hpp \
	include/libgpsfile2/utils/dynamic_unique_ptr_cast.hpp

libgpsfile2_sub_types_includedir = $(includedir)/libgpsfile2/types
libgpsfile2_sub_types_include_HEADERS = include/libgpsfile2/types/PluginDetails.hpp \
	include/libgpsfile2/types/HandlerCreatorFunc.hpp \
	include/libgpsfile2/types/PluginCreatorFunc.hpp \
	include/libgpsfile2/types/BasePlugin.hpp

libgpsfile2_sub_traits_includedir = $(includedir)/libgpsfile2/traits
libgpsfile2_sub_traits_include_HEADERS = include/libgpsfile2/traits/common.hpp \
	include/libgpsfile2/traits/HandlerReaderTrait.hpp \
	include/libgpsfile2/traits/HandlerWriterTrait.hpp \
	include/libgpsfile2/traits/ProviderReaderTrait.hpp \
	include/libgpsfile2/traits/ProviderWriterTrait.hpp

libgpsfile2_sub_provider_includedir = $(includedir)/libgpsfile2/provider
libgpsfile2_sub_provider_include_HEADERS = include/libgpsfile2/provider/ProviderBase.hpp \
	include/libgpsfile2/provider/ProviderReaderBase.hpp \
	include/libgpsfile2/provider/ProviderWriterBase.hpp \
	include/libgpsfile2/provider/ProviderRouteBase.hpp \
	include/libgpsfile2/provider/ProviderRouteReaderBase.hpp \
	include/libgpsfile2/provider/ProviderRouteWriterBase.hpp

lib_LTLIBRARIES += libgpsfile2-providergpsroute.la
libgpsfile2_providergpsroute_la_CPPFLAGS = -Iinclude -DLOCALEDIR=\"$(localedir)\" -DPKGLIBDIR=\"$(pkglibdir)\" @GPSDATA_CFLAGS@ @GPSDATA_UTILS_CFLAGS@
libgpsfile2_providergpsroute_la_LIBADD = @GPSDATA_LIBS@ @GPSDATA_UTILS_LIBS@
## Define the source file list for the "libexample-@EXAMPLE_API_VERSION@.la"
## target.  Note that @EXAMPLE_API_VERSION@ is not interpreted by Automake and
## will therefore be treated as if it were literally part of the target name,
## and the variable name derived from that.
## Note that it is not necessary to list header files
## which are already listed elsewhere in a _HEADERS variable assignment.
libgpsfile2_providergpsroute_la_SOURCES = src/ProviderGpsRoute/ProviderGpsRoute.cpp

#libgpsfile2_la_LIBADD = @GPSDATA_UTILS_LIBS@

## Instruct libtool to include ABI version information in the generated shared
## library file (.so).  The library ABI version is defined in configure.ac, so
## that all version information is kept in one place.
libgpsfile2_providergpsroute_la_LDFLAGS = -version-info @LIBRARY_CURRENT_VERSION@:@LIBRARY_REVISION_VERSION@:@LIBRARY_AGE_VERSION@

## Define the list of public header files and their install location.  The
## nobase_ prefix instructs Automake to not strip the directory part from each
## filename, in order to avoid the need to define separate file lists for each
## installation directory.  This only works if the directory hierarchy in the
## source tree matches the hierarchy at the install location, however.
libgpsfile2_sub_provider_impl_includedir = $(includedir)/libgpsfile2/provider/impl
libgpsfile2_sub_provider_impl_include_HEADERS = include/libgpsfile2/provider/impl/ProviderGpsRoute.hpp \
	include/libgpsfile2/provider/impl/ProviderGpsRouteBase.hpp \
	include/libgpsfile2/provider/impl/ProviderGpsRouteReader.hpp \
	include/libgpsfile2/provider/impl/ProviderGpsRouteWriter.hpp


# pkglib_LTLIBRARIES = gps.la
# gps_la_CPPFLAGS = -Isrc/libsad/include -DLOCALEDIR=\"$(localedir)\"
# gps_la_LDFLAGS = -module -avoid-version -shared
# gps_la_LIBADD = libsad-@LIBRARY_API_VERSION@.la
# gps_la_SOURCES = src/libsad/plugins/gps/plugin.cpp \
# 	src/libsad/plugins/gps/gpsplugin.cpp \
# 	src/libsad/plugins/gps/gpsreader.cpp \
# 	src/libsad/plugins/gps/gpswriter.cpp
# noinst_HEADERS += src/libsad/plugins/gps/gpsplugin.hpp \
# 	src/libsad/plugins/gps/gpsreader.hpp \
# 	src/libsad/plugins/gps/gpswriter.hpp

pkglib_LTLIBRARIES = gpx.la
gpx_la_CPPFLAGS = -Iinclude -DLOCALEDIR=\"$(localedir)\" @GPX_CFLAGS@ @GPSDATA_CFLAGS@ @GPSDATA_UTILS_CFLAGS@ @PLUGINFRAMEWORK_CFLAGS@
gpx_la_LDFLAGS = -module -avoid-version -shared
gpx_la_LIBADD = @GPX_LIBS@ @GPSDATA_LIBS@ @GPSDATA_UTILS_LIBS@ @PLUGINFRAMEWORK_LIBS@
gpx_la_SOURCES = plugins/gpx/plugin.cpp \
	plugins/gpx/gpxplugin.cpp \
	plugins/gpx/gpxreader.cpp \
	plugins/gpx/gpxwriter.cpp \
	plugins/gpx/gpxreport.cpp
noinst_HEADERS += plugins/gpx/gpxplugin.hpp \
	plugins/gpx/gpxreader.hpp \
	plugins/gpx/gpxwriter.hpp \
	plugins/gpx/gpxreport.hpp

# pkglib_LTLIBRARIES += log.la
# log_la_LDFLAGS = -module -avoid-version -shared
# log_la_LIBADD = libsad.la
# log_la_SOURCES = libsad/plugins/log/LogDataFilePlugin.cpp
# noinst_HEADERS += libsad/plugins/log/LogDataFilePlugin.hpp

if EXAMPLE
bin_PROGRAMS = read_gpx
read_gpx_LDADD = libgpsfile2.la libgpsfile2-providergpsroute.la @GPSDATA_LIBS@ @GPSDATA_UTILS_LIBS@ @PLUGINFRAMEWORK_LIBS@
read_gpx_LDFLAGS = -Wl,-rpath=$(pkglibdir),--enable-new-dtags
read_gpx_CPPFLAGS = -Iinclude -DLOCALEDIR=\"$(localedir)\" -DPKGLIBDIR=\"./.libs/\" @GPSDATA_CFLAGS@ @GPSDATA_UTILS_CFLAGS@
read_gpx_SOURCES = examples/read_gpx.cpp

bin_PROGRAMS += write_gpx
write_gpx_LDADD = libgpsfile2.la libgpsfile2-providergpsroute.la @GPSDATA_LIBS@ @GPSDATA_UTILS_LIBS@ @PLUGINFRAMEWORK_LIBS@
write_gpx_LDFLAGS = -Wl,-rpath=$(pkglibdir),--enable-new-dtags
write_gpx_CPPFLAGS = -Iinclude -DLOCALEDIR=\"$(localedir)\" -DPKGLIBDIR=\"./.libs/\" @GPSDATA_CFLAGS@ @GPSDATA_UTILS_CFLAGS@
write_gpx_SOURCES = examples/write_gpx.cpp

bin_PROGRAMS += read_and_write_gpx
read_and_write_gpx_LDADD = libgpsfile2.la libgpsfile2-providergpsroute.la @GPSDATA_LIBS@ @GPSDATA_UTILS_LIBS@ @PLUGINFRAMEWORK_LIBS@
read_and_write_gpx_LDFLAGS = -Wl,-rpath=$(pkglibdir),--enable-new-dtags
read_and_write_gpx_CPPFLAGS = -Iinclude -DLOCALEDIR=\"$(localedir)\" -DPKGLIBDIR=\"./.libs/\" @GPSDATA_CFLAGS@ @GPSDATA_UTILS_CFLAGS@
read_and_write_gpx_SOURCES = examples/read_and_write_gpx.cpp

bin_PROGRAMS += write_and_read_gpx
write_and_read_gpx_LDADD = libgpsfile2.la libgpsfile2-providergpsroute.la @GPSDATA_LIBS@ @GPSDATA_UTILS_LIBS@ @PLUGINFRAMEWORK_LIBS@
write_and_read_gpx_LDFLAGS = -Wl,-rpath=$(pkglibdir),--enable-new-dtags
write_and_read_gpx_CPPFLAGS = -Iinclude -DLOCALEDIR=\"$(localedir)\" -DPKGLIBDIR=\"./.libs/\" @GPSDATA_CFLAGS@ @GPSDATA_UTILS_CFLAGS@ @PLUGINFRAMEWORK_CFLAGS@
write_and_read_gpx_SOURCES = examples/write_and_read_gpx.cpp

# bin_PROGRAMS += read_write_gpx
# read_write_gpx_LDADD = libgpsfile2.la gpx.la $(gpx_la_LIBADD)
# read_write_gpx_LDFLAGS = -Wl,-rpath=$(pkglibdir),--enable-new-dtags
# read_write_gpx_CPPFLAGS = -Iinclude -DLOCALEDIR=\"$(localedir)\" -DPKGLIBDIR=\"$(pkglibdir)\" @GPSDATA_CFLAGS@ @GPSDATA_UTILS_CFLAGS@
# read_write_gpx_SOURCES = examples/read_write_gpx.cpp
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
