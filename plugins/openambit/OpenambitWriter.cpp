#include "config.h"

#include "OpenambitWriter.hpp"

#include <memory>
#include <string>
#include <istream>
#include <functional>
#include <future>
#include <Logger.hpp>
#include <libgpsfile2/provider/ProviderRouteWriterBase.hpp>

#include "OpenambitPlugin.hpp"
#include "Parser.hpp"

#define GPX_TIME_FORMATS { "%Y-%m-{:d}T%TZ" }

using libgpsfile2::provider::ProviderRouteWriterBase;

OpenambitWriter::OpenambitWriter (const std::shared_ptr<OpenambitPlugin> base, std::unique_ptr<ProviderRouteWriterBase> provider, const std::string& path) : HandlerBase (path), HandlerWriterBase (std::move (provider), path) {
	DEBUG_MSG ("OpenambitWriter::{:s} ({:p}, {:p}, {:s})\n", __func__, fmt::ptr (base), fmt::ptr (this->_dp), path);
	this->_base_instance = base;
	this->_provider = dynamic_cast<ProviderRouteWriterBase*> (this->_dp.get ());
}

OpenambitWriter::~OpenambitWriter (void) {
	DEBUG_MSG ("OpenambitWriter::{:s} ()\n", __func__);
	this->_base_instance = nullptr;
	this->_provider = nullptr;
}

bool OpenambitWriter::write (std::istream *s, const bool& is_final) {
	DEBUG_MSG ("OpenambitWriter::{:s} ({:p}, {:d})\n", __func__, fmt::ptr (s), is_final);

	if (is_final) {
		std::shared_ptr<plugin::openambit::Parser> parser = std::make_shared<plugin::openambit::Parser> (this->_provider, this, this->_base_instance);
		const bool ret = parser->parse ();
		if (ret)
			this->_provider->finished ();
		return ret;
	}

	return true;
}