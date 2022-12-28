#include "config.h"

#include "OpenambitPlugin.hpp"

#include <libxml/xmlreader.h>

OpenambitPlugin::OpenambitPlugin (void) {
	/*
	 * this initialize the library and check potential ABI mismatches
	 * between the version it was compiled for and the actual shared
	 * library used.
	 */
	LIBXML_TEST_VERSION
}

OpenambitPlugin::~OpenambitPlugin (void) {
}
