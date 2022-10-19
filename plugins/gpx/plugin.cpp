#include "config.h"
#include <features.h>

#include <memory>
#include <cpluginmanager/plugins.h>
#include <libgpsfile2/PluginHelper.hpp>
#include <libgpsfile2/provider/ProviderRouteReaderBase.hpp>
#include <libgpsfile2/provider/ProviderRouteWriterBase.hpp>

#include "gpxplugin.hpp"
#include "gpxreader.hpp"
#include "gpxwriter.hpp"

#define DATA_FILE_TYPE "gpx"

using libgpsfile2::provider::ProviderRouteReaderBase;
using libgpsfile2::provider::ProviderRouteWriterBase;
using libgpsfile2::handler::HandlerReaderBase;
using libgpsfile2::handler::HandlerWriterBase;
using libgpsfile2::PluginDetails;

void __attribute__((constructor)) register_handler();
void __attribute__((destructor)) remove_handler();

void register_handler (void) {
	PluginDetails details;
	auto base = std::make_shared<GpxPlugin>();

	libgpsfile2::PluginHelper::constructPlugin (DATA_FILE_TYPE,
												"gpx datafile handler",
												"Martijn Goedhart",
												"GPLv2.0+",
												0,
												1)
		->addReader<ProviderRouteReaderBase, GpxReader> (base)
		->addWriter<ProviderRouteWriterBase, GpxWriter> (base)
		->construct (details);

	cpluginmanager_register_plugin (libgpsfile2::GpsfileManager::TAG.data (), details);
}

void remove_handler (void) {
	cpluginmanager_remove_plugin (libgpsfile2::GpsfileManager::TAG. data (), DATA_FILE_TYPE);
}
