#include "config.h"

#include <memory>
#include <string>
#include <PluginManager.hpp>
#include <gpsdata/utils/Logger.hpp>
#include <gpsdata/utils/GpsDataFactoryBasic.hpp>
#include <libgpsfile2.hpp>
#include <libgpsfile2/provider/impl/ProviderGpsRoute.hpp>

#include "PrintGpsRoute.hpp"

using libgpsfile2::GpsfileManager;
using libgpsfile2::handler::HandlerWriterBase;
using libgpsfile2::provider::ProviderGpsRouteWriter;
using libgpsfile2::provider::ProviderRouteWriterBase;

using GpsFactoryType = gpsdata::utils::GpsDataFactoryBasic;
using GpsPointType = gpsdata::GpsPoint<GpsFactoryType>;
using GpsSegmentType = gpsdata::GpsSegment<GpsFactoryType, GpsPointType>;
using GpsRouteType = gpsdata::GpsRoute<GpsFactoryType, GpsSegmentType>;

int main (void) {
	std::cout << "create plugin\n";
	// load gpsfile plugin
	const auto plugin = GpsfileManager::getPtr ();
	pluginmanager::Manager::getInstance ().addManager (plugin);

	std::cout << "create factory\n";
	// create factory for gps data
	const auto factory = GpsFactoryType::create ();

	std::cout << "create provider\n";
	// create provider
	auto provider = ProviderGpsRouteWriter<GpsRouteType>::create (factory);

	//std::cout << "register type\n";
	//libgpsfile2::HandlerType route_writer;
	//this->registerWriterType<ProviderRouteWriterBase> (route_writer);
	//std::cout << "register type\n";

	std::cout << "create handler\n";
	// get handler
	std::string path = "/tmp/test.gpx";
	std::unique_ptr<HandlerWriterBase> handler = plugin->createWriter<ProviderRouteWriterBase> (std::move (provider), path, path.substr (path.size () - 3, 3));

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
