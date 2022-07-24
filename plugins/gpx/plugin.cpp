#include "config.h"

#include <memory>

#include "libgpsfile2.hpp"

#include "gpxplugin.hpp"
#include "gpxreader.hpp"
#include "gpxwriter.hpp"

#define DATA_FILE_TYPE "gpx"

using libgpsfile2::provider::ProviderRouteReaderBase;
using libgpsfile2::provider::ProviderRouteWriterBase;
using libgpsfile2::BaseDatahandlerRouteReaderPlugin;
using libgpsfile2::BaseDatahandlerRouteWriterPlugin;
using libgpsfile2::pluginDetails;
using libgpsfile2::pluginDatahandler;
using libgpsfile2::GpsfilePlugin;
using libgpsfile2::BasePlugin;

void __attribute__((constructor)) register_handler();
void __attribute__((destructor)) remove_handler();

BaseDatahandlerRouteReaderPlugin *create_read_class (const std::shared_ptr<const BasePlugin>&, std::shared_ptr<ProviderRouteReaderBase>, const std::string&);
BaseDatahandlerRouteWriterPlugin *create_write_class (const std::shared_ptr<const BasePlugin>&, std::shared_ptr<ProviderRouteWriterBase>, const std::string&);

BaseDatahandlerRouteReaderPlugin *create_read_class (const std::shared_ptr<const BasePlugin>& base, std::shared_ptr<ProviderRouteReaderBase> dp, const std::string& path) {
	GpxReader *reader = new GpxReader (std::dynamic_pointer_cast<const GpxPlugin>(base), dp, path);
	return reader;
}

BaseDatahandlerRouteWriterPlugin *create_write_class (const std::shared_ptr<const BasePlugin>& base, std::shared_ptr<ProviderRouteWriterBase> dp, const std::string& path) {
	GpxWriter *writer = new GpxWriter (std::dynamic_pointer_cast<const GpxPlugin>(base), dp, path);
	return writer;
}

void register_handler (void) {
	pluginDetails details;
	pluginDatahandler* data = new pluginDatahandler ();

	details.name = "gpx datafile handler";
	details.author = "Martijn Goedhart";
	details.license = "GPLv2.0+";
	details.major_version = 0;
	details.minor_version = 1;

	data->instance = std::make_shared<GpxPlugin>();
	data->route_read_class = create_read_class;
	data->route_write_class = create_write_class;

	GpsfilePlugin::registerPlugin (DATA_FILE_TYPE, details, data);
}

void remove_handler (void) {
	GpsfilePlugin::removePlugin(DATA_FILE_TYPE);
}
