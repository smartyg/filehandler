#include "config.h"

#include "filehandler/handler/HandlerBase.hpp"

#include <string>
#include <Logger.hpp>

using filehandler::handler::HandlerBase;

HandlerBase::~HandlerBase (void) {
	DEBUG_MSG ("HandlerBase::{:s} ()\n", __func__);
	delete this->_buf;
}

const std::string HandlerBase::getPath (void) const {
	DEBUG_MSG ("HandlerBase::{:s} ()\n", __func__);
	return this->_path;
}