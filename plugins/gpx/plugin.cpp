#include "config.h"

#include <memory>

#include "libgpsfile2.hpp"
#include "libgpsfile2/ConstValue.hpp"
#include "libgpsfile2/traits/HandlerProviderPair.hpp"


#include "gpxplugin.hpp"
#include "gpxreader.hpp"
#include "gpxwriter.hpp"

#define DATA_FILE_TYPE "gpx"

using libgpsfile2::provider::ProviderReaderBase;
using libgpsfile2::provider::ProviderWriterBase;
using libgpsfile2::provider::ProviderRouteReaderBase;
using libgpsfile2::provider::ProviderRouteWriterBase;
using libgpsfile2::HandlerReaderBase;
using libgpsfile2::HandlerWriterBase;
//using libgpsfile2::HandlerRouteReaderBase;
//using libgpsfile2::HandlerRouteWriterBase;
using libgpsfile2::pluginDetails;
using libgpsfile2::pluginDatahandler;
using libgpsfile2::GpsfilePlugin;
using libgpsfile2::BasePlugin;
using libgpsfile2::DatahandlerType;

void __attribute__((constructor)) register_handler();
void __attribute__((destructor)) remove_handler();

std::unique_ptr<HandlerReaderBase> create_read_class (const std::shared_ptr<const BasePlugin>&, std::unique_ptr<ProviderReaderBase>, const std::string&);
std::unique_ptr<HandlerWriterBase> create_write_class (const std::shared_ptr<const BasePlugin>&, std::unique_ptr<ProviderWriterBase>, const std::string&);

std::unique_ptr<HandlerReaderBase> create_read_class (const std::shared_ptr<const BasePlugin>& base, std::unique_ptr<ProviderReaderBase> dp, const std::string& path) {
	std::unique_ptr<HandlerReaderBase> reader = std::make_unique<GpxReader>(std::dynamic_pointer_cast<const GpxPlugin>(base), std::move (dp), path);
	return reader;
}

std::unique_ptr<HandlerWriterBase> create_write_class (const std::shared_ptr<const BasePlugin>& base, std::unique_ptr<ProviderWriterBase> dp, const std::string& path) {
	std::unique_ptr<HandlerWriterBase> writer = std::make_unique<GpxWriter>(std::dynamic_pointer_cast<const GpxPlugin>(base), std::move (dp), path);
	return writer;
}

void register_handler (void) {
	//std::cout << "call register handler of gpx plugin\n";
	auto details = std::make_shared<pluginDetails>();
	auto handlers = std::make_shared<pluginDatahandler>();

	details->name = "gpx datafile handler";
	details->author = "Martijn Goedhart";
	details->license = "GPLv2.0+";
	details->major_version = 0;
	details->minor_version = 1;

	DatahandlerType route_reader;
	DatahandlerType route_writer;
	GpsfilePlugin::registerReaderType<ProviderRouteReaderBase> (route_reader);
	GpsfilePlugin::registerWriterType<ProviderRouteWriterBase> (route_writer);

	handlers->instance = std::make_shared<GpxPlugin>();
	handlers->reader_creators[static_cast<int>(route_reader)] = create_read_class;
	handlers->writer_creators[static_cast<int>(route_writer)] = create_write_class;

	GpsfilePlugin::registerPlugin (DATA_FILE_TYPE, details, handlers);
}

void remove_handler (void) {
	GpsfilePlugin::removePlugin(DATA_FILE_TYPE);
}
