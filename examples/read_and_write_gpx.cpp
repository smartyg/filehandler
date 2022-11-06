#include "config.h"

#include <memory>
#include <string>
#include <pluginframework/Controller.hpp>
#include <gpsdata/utils/GpsDataFactoryBasic.hpp>
#include <libgpsfile2.hpp>
#include <libgpsfile2/provider/impl/ProviderGpsRoute.hpp>

#include "PrintGpsRoute.hpp"

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
	std::cout << "create plugin\n";
	// load gpsfile plugin
	const auto plugin = GpsfileManager::getPtr ();
	pluginframework::Controller::getInstance ().addManager (plugin);
	pluginframework::Controller::getInstance ().scanDirectory ("./.libs");

	std::cout << "create factory\n";
	// create factory for gps data
	const auto factory = GpsFactoryType::create ();

	std::cout << "create writer provider\n";
	// create provider
	auto provider_writer = ProviderGpsRouteWriter<GpsRouteType>::create (factory);

	std::cout << "create writer handler\n";
	// get handler
	std::string path_writer = "/tmp/test.gpx";
	std::unique_ptr<HandlerWriterBase> handler_writer = plugin->createWriter<ProviderRouteWriterBase> (std::move (provider_writer), path_writer, path_writer.substr (path_writer.size () - 3, 3));

	if (!handler_writer) throw std::runtime_error ("data handler is empty");

	std::cout << "read to writer handler file\n";
	// read the file given in the path argument of the constructor
	handler_writer->readFile ();

	std::cout << "get provider\n";
	const auto provider_return = handler_writer->getProvider<ProviderGpsRouteWriter<GpsRouteType>> ();

	std::cout << "get route\n";
	auto routes = provider_return->getRoutes ();

	// Now we will read the data from the system back into another file

	std::cout << "create reader provider\n";
	// create provider
	auto provider_reader = ProviderGpsRouteReader<GpsRouteType>::create (routes);

	std::cout << "create reader handler\n";
	// get handler
	std::string path_reader = "/tmp/test-out.gpx";
	std::unique_ptr<HandlerReaderBase> handler_reader = plugin->createReader<ProviderRouteReaderBase> (std::move (provider_reader), path_reader, path_reader.substr (path_reader.size () - 3, 3));

	if (!handler_reader) throw std::runtime_error ("data handler is empty");

	std::cout << "write from reader handler file\n";
	// read the file given in the path argument of the constructor
	handler_reader->writeFile ();

	return 0;
}
