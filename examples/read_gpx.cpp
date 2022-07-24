#include "config.h"

#include <memory>
#include <string>

#include "libgpsfile2.hpp"
#include "libgpsfile2/provider/ProviderGpsRouteWriter.hpp"
#include "gpsdata/utils/GpsDataFactoryBasic.hpp"

using libgpsfile2::GpsfilePlugin;
using libgpsfile2::BaseDatahandlerWriterPlugin;
using libgpsfile2::provider::ProviderGpsRouteWriter;
using gpsdata::utils::GpsDataFactoryBasic;

int main (void) {
	// load gpsfile plugin
	auto plugin = GpsfilePlugin::create ();

	// create factory for gps data
	auto factory = GpsDataFactoryBasic::create ();

	// create provider
	auto provider = ProviderGpsRouteWriter<GpsDataFactoryBasic>::create (factory);

	// get handler
	std::string path = "/tmp/test.gpx";
	BaseDatahandlerWriterPlugin *handler = plugin->createWriter (provider, libgpsfile2::PLUGIN_TYPE_DATA_FILE_WRITE, path, path.substr (path.size () - 4, 4));

	// read the file given in the path argument of the constructor
	handler->readFile ();

	auto routes = provider->getRoutes ();

	for (const auto& route : routes) {
		std::cout << "--- route " << route->getTitle () << " ---" << std::endl;
		for (const auto& segment : *route) {
			std::cout << "segment " << std::to_string (segment->getSegmentNumber ()) << std::endl;
			for (const auto& point : *segment) {
				std::cout << "point (" << std::to_string (point->getTime ()) << ")" << std::endl;
				for (const auto& data : *point) {
					std::string str;
					if (factory->getValue (data, str, true))
						std::cout << "  " << factory->getDataTypeFullName (data.type) << ": " << str << std::endl;
				}
			}
		}
	}

	return 0;
}