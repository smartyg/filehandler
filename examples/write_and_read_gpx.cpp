#include "config.h"

#include <memory>
#include <string>
#include <pluginframework/Controller.hpp>
#include <gpsdata/utils/GpsDataFactoryBasic.hpp>
#include <libgpsfile2.hpp>
#include <libgpsfile2/provider/impl/ProviderGpsRoute.hpp>

#include "PrintGpsRoute.hpp"
#include "Routes.hpp"

using libgpsfile2::GpsfileManager;
using libgpsfile2::handler::HandlerReaderBase;
using libgpsfile2::handler::HandlerWriterBase;
using libgpsfile2::provider::ProviderGpsRouteReader;
using libgpsfile2::provider::ProviderGpsRouteWriter;
using libgpsfile2::provider::ProviderRouteReaderBase;
using libgpsfile2::provider::ProviderRouteWriterBase;

using GpsFactoryType = gpsdata::utils::GpsDataFactoryBasic;
using GpsPointType = gpsdata::GpsPoint<GpsFactoryType>;
using GpsSegmentType = gpsdata::GpsSegment<GpsFactoryType, GpsPointType>;
using GpsRouteType = gpsdata::GpsRoute<GpsFactoryType, GpsSegmentType>;

int main (void) {
	// load gpsfile plugin
	const auto plugin = GpsfileManager::getPtr ();
	pluginframework::Controller::getInstance ().addManager (plugin);
	pluginframework::Controller::getInstance ().scanDirectory ("./.libs");

	const auto factory = GpsFactoryType::create ();

	auto route = create_route_1<GpsRouteType> (factory);
	std::vector<std::shared_ptr<GpsRouteType>> routes;
	routes.insert (routes.begin (), route);

	std::cout << "create provider\n";
	// create provider
	auto provider_reader = ProviderGpsRouteReader<GpsRouteType>::create (routes);

	std::cout << "create handler\n";
	// get handler
	std::string path = "/tmp/test.gpx";
	std::unique_ptr<HandlerReaderBase> handler_reader = plugin->createReader<ProviderRouteReaderBase> (std::move (provider_reader), path, path.substr (path.size () - 3, 3));

	if (!handler_reader) throw std::runtime_error ("data handler is empty");

	std::cout << "write file\n";
	// read the file given in the path argument of the constructor
	handler_reader->writeFile ();

	// Now we will read the file back and write the result into the system

	std::cout << "create writer provider\n";
	// create provider
	auto provider_writer = ProviderGpsRouteWriter<GpsRouteType>::create (factory);

	std::cout << "create writer handler\n";
	// get handler
	std::unique_ptr<HandlerWriterBase> handler_writer = plugin->createWriter<ProviderRouteWriterBase> (std::move (provider_writer), path, path.substr (path.size () - 3, 3));

	if (!handler_writer) throw std::runtime_error ("data handler is empty");

	std::cout << "read to writer handler file\n";
	// read the file given in the path argument of the constructor
	handler_writer->readFile ();

	std::cout << "get provider\n";
	const auto provider_return = handler_writer->getProvider<ProviderGpsRouteWriter<GpsRouteType>> ();

	const auto routes_res = provider_return->getRoutes ();
	for (std::size_t i = 0; i < routes_res.size (); ++i) {
		printGpsRoute (routes[i]);
		printGpsRoute (routes_res[i]);
		//assert (*(routes_res[i]) == *(routes[i]));
		if (*(routes_res[i]) == *(routes[i])) std::cout << "routes are equal" << std::endl;
		else std::cout << "routes are NOT equal" << std::endl;
	}

	return 0;
}
