#include "config.h"

#include <features.h>

#include "Plugin.hpp"

using plugin::gpx::Plugin;

Plugin::Plugin (void) {
	this->init ();
}

Plugin::~Plugin (void) {
}
