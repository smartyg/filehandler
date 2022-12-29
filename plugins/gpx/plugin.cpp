#include "config.h"

#include <memory>
#include <pluginframework/plugins.h>
#include <filehandler/PluginHelper.hpp>
#include <filehandler/provider/types/route/ProviderRouteReaderBase.hpp>
#include <filehandler/provider/types/route/ProviderRouteWriterBase.hpp>

#include "Plugin.hpp"
#include "Reader.hpp"
#include "Writer.hpp"

#define DATA_FILE_TYPE "gpx"

using plugin::gpx::Plugin;
using plugin::gpx::Reader;
using plugin::gpx::Writer;
using filehandler::provider::ProviderRouteReaderBase;
using filehandler::provider::ProviderRouteWriterBase;
using filehandler::handler::HandlerReaderBase;
using filehandler::handler::HandlerWriterBase;

void __attribute__((constructor)) register_handler();
void __attribute__((destructor)) remove_handler();

static ::PluginDetails details;

void register_handler (void) {
	auto base = std::make_shared<Plugin>();

	filehandler::PluginHelper::constructPlugin (DATA_FILE_TYPE,
												"gpx datafile handler",
												"Martijn Goedhart",
												"GPLv2.0+",
												0,
												1)
		->addReader<ProviderRouteReaderBase, Reader> (base)
		->addWriter<ProviderRouteWriterBase, Writer> (base)
		->construct (details);

		pluginframework_register_plugin (filehandler::FileHandlerManager::TAG.data (), details);
}

void remove_handler (void) {
	pluginframework_remove_plugin (filehandler::FileHandlerManager::TAG.data (), DATA_FILE_TYPE);
	filehandler::PluginHelper::deleteData (details);
}
