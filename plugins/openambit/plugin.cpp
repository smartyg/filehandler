#include "config.h"

#include <memory>
#include <pluginframework/plugins.h>
#include <filehandler/PluginHelper.hpp>
#include <filehandler/provider/types/route/ProviderRouteWriterBase.hpp>

#include "Plugin.hpp"
#include "Writer.hpp"

#define DATA_FILE_TYPE "log"

using filehandler::provider::ProviderRouteWriterBase;
using filehandler::handler::HandlerWriterBase;
using plugin::openambit::Writer;
using plugin::openambit::Plugin;

void __attribute__((constructor)) register_handler();
void __attribute__((destructor)) remove_handler();

static ::PluginDetails details;

void register_handler (void) {
	auto base = std::make_shared<Plugin>();

	filehandler::PluginHelper::constructPlugin (DATA_FILE_TYPE,
												"openambit logfile datafile handler",
												"Martijn Goedhart",
												"GPLv3.0+",
												0,
												0)
		->addWriter<ProviderRouteWriterBase, Writer> (base)
		->construct (details);

		pluginframework_register_plugin (filehandler::FileHandlerManager::TAG.data (), details);
}

void remove_handler (void) {
	pluginframework_remove_plugin (filehandler::FileHandlerManager::TAG. data (), DATA_FILE_TYPE);
	filehandler::PluginHelper::deleteData (details);
}
