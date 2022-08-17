#include "config.h"

#include <memory>
#include <string>

#include <gpsdata/utils/Logger.hpp>
#include "libgpsfile2.hpp"
#include "libgpsfile2/ConstValue.hpp"
#include "libgpsfile2/provider/ProviderGpsRouteWriter.hpp"
#include "gpsdata/utils/GpsDataFactoryBasic.hpp"

#include "PrintGpsRoute.hpp"

using libgpsfile2::GpsfilePlugin;
using libgpsfile2::HandlerWriterBase;
using libgpsfile2::provider::ProviderGpsRouteWriter;
using libgpsfile2::provider::ProviderRouteWriterBase;

using GpsFactoryType = gpsdata::utils::GpsDataFactoryBasic;
using GpsPointType = gpsdata::GpsPoint<GpsFactoryType>;
using GpsSegmentType = gpsdata::GpsSegment<GpsFactoryType, GpsPointType>;
using GpsRouteType = gpsdata::GpsRoute<GpsFactoryType, GpsSegmentType>;

int main (void) {
	std::cout << "create plugin\n";
	// load gpsfile plugin
	auto plugin = GpsfilePlugin::create ({std::string(PKGLIBDIR)});

	std::cout << "create factory\n";
	// create factory for gps data
	auto factory = GpsFactoryType::create ();

	std::cout << "create provider\n";
	// create provider
	auto provider = ProviderGpsRouteWriter<GpsRouteType>::create (factory);

	std::cout << "register type\n";
	libgpsfile2::DatahandlerType route_writer;
	GpsfilePlugin::registerWriterType<ProviderRouteWriterBase> (route_writer);
	//std::cout << "register type\n";

	std::cout << "create handler\n";
	// get handler
	std::string path = "/tmp/test.gpx";
	std::unique_ptr<HandlerWriterBase> handler = plugin->createWriter (std::move (provider), route_writer, path, path.substr (path.size () - 4, 4));

	if (!handler) throw std::runtime_error ("data handler is empty");

	std::cout << "read file\n";
	// read the file given in the path argument of the constructor
	handler->readFile ();

	std::cout << "get provider\n";
	auto provider_return = handler->getProvider<ProviderGpsRouteWriter<GpsRouteType>> ();

	std::cout << "get route\n";
	auto routes = provider_return->getRoutes ();

	for (const auto& route : routes) {
		printGpsRoute (route);
	}

	return 0;
}
