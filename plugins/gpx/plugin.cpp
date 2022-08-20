#include "config.h"
#include <features.h>

#include <memory>
#include <string>
#include <utility>
#include <libgpsfile2/GpsfilePlugin.hpp>
#include <libgpsfile2/PluginHandler.hpp>
#include <libgpsfile2/types/BasePlugin.hpp>
#include <libgpsfile2/types/HandlerType.hpp>
#include <libgpsfile2/types/PluginDetails.hpp>
#include <libgpsfile2/handler/HandlerReaderBase.hpp>
#include <libgpsfile2/handler/HandlerWriterBase.hpp>
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
using libgpsfile2::PluginHandler;
using libgpsfile2::GpsfilePlugin;
using libgpsfile2::BasePlugin;
using libgpsfile2::HandlerType;

void __attribute__((constructor)) register_handler();
void __attribute__((destructor)) remove_handler();

std::unique_ptr<HandlerReaderBase> create_reader_class (const std::shared_ptr<BasePlugin>, std::unique_ptr<ProviderRouteReaderBase>, const std::string&);
std::unique_ptr<HandlerWriterBase> create_writer_class (const std::shared_ptr<BasePlugin>, std::unique_ptr<ProviderRouteWriterBase>, const std::string&);

std::unique_ptr<HandlerReaderBase> create_reader_class (const std::shared_ptr<BasePlugin> base, std::unique_ptr<ProviderRouteReaderBase> dp, const std::string& path) {
	std::unique_ptr<HandlerReaderBase> reader = std::make_unique<GpxReader>(std::dynamic_pointer_cast<GpxPlugin>(base), std::move (dp), path);
	return reader;
}

std::unique_ptr<HandlerWriterBase> create_writer_class (const std::shared_ptr<BasePlugin> base, std::unique_ptr<ProviderRouteWriterBase> dp, const std::string& path) {
	std::unique_ptr<HandlerWriterBase> writer = std::make_unique<GpxWriter>(std::dynamic_pointer_cast<GpxPlugin>(base), std::move (dp), path);
	return writer;
}

void register_handler (void) {
	auto details = std::make_shared<PluginDetails>();
	auto handlers = std::make_shared<PluginHandler>(std::make_shared<GpxPlugin>());

	details->name = "gpx datafile handler";
	details->author = "Martijn Goedhart";
	details->license = "GPLv2.0+";
	details->major_version = 0;
	details->minor_version = 1;

	HandlerType route_reader;
	HandlerType route_writer;
	GpsfilePlugin::registerReaderType<ProviderRouteReaderBase> (route_reader);
	GpsfilePlugin::registerWriterType<ProviderRouteWriterBase> (route_writer);

	handlers->addReader (route_reader, create_reader_class);
	handlers->addWriter (route_writer, create_writer_class);

	GpsfilePlugin::registerPlugin (DATA_FILE_TYPE, details, handlers);
}

void remove_handler (void) {
	GpsfilePlugin::removePlugin (DATA_FILE_TYPE);
}
