#include "config.h"

#include <memory>
#include <string>
#include <pluginframework/Controller.hpp>
#include <gpsdata/utils/GpsDataFactoryBasic.hpp>
#include <libgpsfile2.hpp>
#include <libgpsfile2/provider/impl/ProviderGpsRoute.hpp>

//#include "PrintGpsRoute.hpp"
#include "Routes.hpp"

using libgpsfile2::GpsfileManager;
using libgpsfile2::handler::HandlerReaderBase;
using libgpsfile2::provider::ProviderGpsRouteReader;
using libgpsfile2::provider::ProviderRouteReaderBase;

using GpsFactoryType = gpsdata::utils::GpsDataFactoryBasic;
using GpsPointType = gpsdata::GpsPoint<GpsFactoryType>;
using GpsSegmentType = gpsdata::GpsSegment<GpsFactoryType, GpsPointType>;
using GpsRouteType = gpsdata::GpsRoute<GpsFactoryType, GpsSegmentType>;

int main (void) {
	cpplogger::Logger::setFile ("./write_gpx.log");
	cpplogger::Logger::setLoglevel (cpplogger::Level::DEBUG + 5);

	// load gpsfile plugin
	const auto plugin = GpsfileManager::getPtr ();

	pluginframework::Controller::getInstance ().addManager (plugin);
	pluginframework::Controller::getInstance ().scanDirectory ("./.libs");

	std::vector<std::shared_ptr<GpsRouteType>> routes;
	routes.insert (routes.begin (), create_route_1<GpsRouteType> ());

	std::cout << "create provider\n";
	// create provider
	auto provider = ProviderGpsRouteReader<GpsRouteType>::create (routes);

	std::cout << "create handler\n";
	// get handler
	std::string path = "/tmp/test.gpx";
	std::unique_ptr<HandlerReaderBase> handler = plugin->createReader<ProviderRouteReaderBase> (std::move (provider), path, path.substr (path.size () - 3, 3));

	if (!handler) throw std::runtime_error ("data handler is empty");

	std::cout << "write file\n";
	// read the file given in the path argument of the constructor
	handler->writeFile ();

	return 0;
}
