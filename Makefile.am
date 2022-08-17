## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

## Additional flags to pass to aclocal when it is invoked automatically at
## make time. The ${ACLOCAL_FLAGS} variable is picked up from the environment
## to provide a way for the user to supply additional arguments.
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4

AM_CFLAGS =
AM_CXXFLAGS =

## Only use the compiler flags when we build a debug version.
if DEBUG
COMMON_DEBUG_OPTIONS = -Wall -Wextra -W -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align \
	-Wmissing-declarations -Wno-missing-braces -Wno-missing-field-initializers \
	-Wformat=1 -Wswitch-default -Wswitch-enum -Wstrict-overflow=5 -Winline \
	-Wundef -Wunreachable-code -Wlogical-op -Wfloat-equal -Wno-float-equal \
	-Wstrict-aliasing=2 -Wredundant-decls -Werror \
	-fno-omit-frame-pointer -ffloat-store -fno-common -fstrict-aliasing \
	-fvar-tracking -fvar-tracking-assignments -ggdb3 -O0
AM_CFLAGS += $(COMMON_DEBUG_OPTIONS) \
	-Wbad-function-cast -Wstrict-prototypes -Wmissing-prototypes -Wnested-externs \
	-Wold-style-definition
AM_CXXFLAGS += $(COMMON_DEBUG_OPTIONS) \
	-Wconversion-null  -Wctor-dtor-privacy -Wdelete-non-virtual-dtor \
	-Wnarrowing -Wnoexcept -Wnon-virtual-dtor  -Wreorder -Wstrict-null-sentinel \
	-Wno-non-template-friend  -Wold-style-cast -Woverloaded-virtual  -Wno-pmf-conversions \
	-Wsign-promo
else
AM_CFLAGS += -flto
AM_CXXFLAGS += -flto
endif

export AM_CFLAGS
export AM_CXXFLAGS

## Install the generated pkg-config file (.pc) into the expected location for
## architecture-dependent package configuration information.  Occasionally,
## pkg-config files are also used for architecture-independent data packages,
## in which case the correct install location would be $(datadir)/pkgconfig.
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libgpsfile2.pc

lib_LTLIBRARIES = libgpsfile2.la
libgpsfile2_la_CPPFLAGS = -Iinclude -DLOCALEDIR=\"$(localedir)\" -DPKGLIBDIR=\"$(pkglibdir)\" @GPSDATA_CFLAGS@ @GPSDATA_UTILS_CFLAGS@

## Define the source file list for the "libexample-@EXAMPLE_API_VERSION@.la"
## target.  Note that @EXAMPLE_API_VERSION@ is not interpreted by Automake and
## will therefore be treated as if it were literally part of the target name,
## and the variable name derived from that.
## Note that it is not necessary to list header files
## which are already listed elsewhere in a _HEADERS variable assignment.
libgpsfile2_la_SOURCES = src/GpsfilePlugin.cpp \
	src/Datahandler.cpp \
	src/DatahandlerPlain.cpp \
	src/utils/Iobuf.cpp

libgpsfile2_la_LIBADD = @DL_LIBS@ @GPSDATA_UTILS_LIBS@

## Instruct libtool to include ABI version information in the generated shared
## library file (.so).  The library ABI version is defined in configure.ac, so
## that all version information is kept in one place.
libgpsfile2_la_LDFLAGS = -version-info @LIBRARY_CURRENT_VERSION@:@LIBRARY_REVISION_VERSION@:@LIBRARY_AGE_VERSION@

noinst_HEADERS = src/DatahandlerPlain.hpp


## Define the list of public header files and their install location.  The
## nobase_ prefix instructs Automake to not strip the directory part from each
## filename, in order to avoid the need to define separate file lists for each
## installation directory.  This only works if the directory hierarchy in the
## source tree matches the hierarchy at the install location, however.
libgpsfile2_la_includedir = $(includedir)
nobase_libgpsfile2_la_include_HEADERS = include/libgpsfile2.hpp \
	include/libgpsfile2/GpsfilePlugin.hpp \
	include/libgpsfile2/BasePlugin.hpp \
	include/libgpsfile2/Datahandler.hpp \
	include/libgpsfile2/provider/provider.hpp \
	include/libgpsfile2/utils/Iobuf.hpp

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
gpx_la_CPPFLAGS = -Iinclude -DLOCALEDIR=\"$(localedir)\" @GPX_CFLAGS@ @GPSDATA_CFLAGS@ @GPSDATA_UTILS_CFLAGS@
gpx_la_LDFLAGS = -module -avoid-version -shared
gpx_la_LIBADD = libgpsfile2.la @GPX_LIBS@
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
read_gpx_LDADD = libgpsfile2.la
read_gpx_LDFLAGS = -Wl,-rpath=$(pkglibdir),--enable-new-dtags
read_gpx_CPPFLAGS = -Iinclude -DLOCALEDIR=\"$(localedir)\" -DPKGLIBDIR=\"./.libs/\" @GPSDATA_CFLAGS@ @GPSDATA_UTILS_CFLAGS@
read_gpx_SOURCES = examples/read_gpx.cpp

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
.PHONY: ChangeLog

## Dynamicly create the changelog file when a distribution tarball is created.
ChangeLog:
	-$(GIT) log --decorate > ChangeLog

VERSION:
	-$(GIT) describe --tags --match=v[0-9]* --long --dirty | $(SED) 's/^v//' > VERSION

distclean-local:
	@rm -f ChangeLog VERSION
endif

if GTEST
SUBDIRS = tests
endif
