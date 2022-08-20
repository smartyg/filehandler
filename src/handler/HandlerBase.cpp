#include "config.h"
#include <features.h>

#include "libgpsfile2/handler/HandlerBase.hpp"

#include <string>
#include <gpsdata/utils/Logger.hpp>

using libgpsfile2::handler::HandlerBase;

HandlerBase::~HandlerBase (void) {
	DEBUG_MSG("HandlerBase::%s ()\n", __func__);
	this->_path.clear ();
	delete this->_buf;
}

const std::string HandlerBase::getPath (void) const {
	DEBUG_MSG("HandlerBase::%s ()\n", __func__);
	return this->_path;
}