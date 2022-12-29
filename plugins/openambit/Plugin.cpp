#include "config.h"

#include "Plugin.hpp"

#include <libxml/xmlreader.h>

using plugin::openambit::Plugin;

Plugin::Plugin (void) {
	/*
	 * this initialize the library and check potential ABI mismatches
	 * between the version it was compiled for and the actual shared
	 * library used.
	 */
	LIBXML_TEST_VERSION
}

Plugin::~Plugin (void) {
}
