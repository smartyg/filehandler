#include "config.h"

#include "Writer.hpp"

#include <memory>
#include <string>
#include <istream>
#include <functional>
#include <future>
#include <Logger.hpp>
#include <filehandler/provider/types/route/ProviderRouteWriterBase.hpp>

#include "Plugin.hpp"
#include "Parser.hpp"

#define GPX_TIME_FORMATS { "%Y-%m-{:d}T%TZ" }

using plugin::openambit::Plugin;
using plugin::openambit::Writer;
using plugin::openambit::Parser;
using filehandler::provider::ProviderRouteWriterBase;

Writer::Writer (const std::shared_ptr<Plugin> base, std::unique_ptr<ProviderRouteWriterBase> provider, const std::string& path) : HandlerBase (path), HandlerWriterBase (std::move (provider), path) {
	DEBUG_MSG ("Writer::{:s} ({:p}, {:p}, {:s})\n", __func__, fmt::ptr (base), fmt::ptr (this->_dp), path);
	this->_base_instance = base;
	this->_provider = dynamic_cast<ProviderRouteWriterBase*> (this->_dp.get ());
}

Writer::~Writer (void) {
	DEBUG_MSG ("Writer::{:s} ()\n", __func__);
	this->_base_instance = nullptr;
	this->_provider = nullptr;
}

bool Writer::write (std::istream *s, const bool& is_final) {
	DEBUG_MSG ("Writer::{:s} ({:p}, {:d})\n", __func__, fmt::ptr (s), is_final);

	if (is_final) {
		std::shared_ptr<Parser> parser = std::make_shared<Parser> (this->_provider, this, this->_base_instance);
		const bool ret = parser->parse ();
		if (ret)
			this->_provider->finished ();
		return ret;
	}

	return true;
}