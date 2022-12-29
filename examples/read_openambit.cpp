#include "config.h"

#include <memory>
#include <string>
#include <Logger.hpp>
#include <pluginframework/Controller.hpp>
#include <gpsdata/utils/GpsDataFactoryBasic.hpp>
#include <filehandler.hpp>
#include <filehandler/provider/types/route/impl/ProviderGpsRoute.hpp>

#include "PrintGpsRoute.hpp"

using filehandler::FileHandlerManager;
using filehandler::handler::HandlerReaderBase;
using filehandler::handler::HandlerWriterBase;
using filehandler::provider::ProviderGpsRouteReader;
using filehandler::provider::ProviderGpsRouteWriter;
using filehandler::provider::ProviderRouteReaderBase;
using filehandler::provider::ProviderRouteWriterBase;

using GpsFactoryType = gpsdata::utils::GpsDataFactoryBasic;
using GpsPointType = gpsdata::GpsPoint<GpsFactoryType>;
using GpsSegmentType = gpsdata::GpsSegment<GpsFactoryType, GpsPointType>;
using GpsRouteType = gpsdata::GpsRoute<GpsFactoryType, GpsSegmentType>;

int main (void) {
	cpplogger::Logger::setFile ("./read_openambit.log");
	cpplogger::Logger::setLoglevel (cpplogger::Level::DEBUG + 5);
	cpplogger::Logger::setSplit (true);

	std::cout << "create plugin\n";
	// load gpsfile plugin
	const auto plugin = FileHandlerManager::getPtr ();
	pluginframework::Controller::getInstance ().addManager (plugin);
	pluginframework::Controller::getInstance ().scanDirectory ("./.libs");

	std::cout << "create factory\n";
	// create factory for gps data
	const auto factory = GpsFactoryType::create ();

	std::cout << "create provider\n";
	// create provider
	auto provider = ProviderGpsRouteWriter<GpsRouteType>::create (factory);

	//std::cout << "register type\n";
	//filehandler::HandlerType route_writer;
	//this->registerWriterType<ProviderRouteWriterBase> (route_writer);
	//std::cout << "register type\n";

	std::cout << "create handler\n";
	// get handler
	std::string path = "/tmp/test.log";
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
//#if 0
	std::cout << "create provider\n";
	// create provider
	auto out_provider = ProviderGpsRouteReader<GpsRouteType>::create (routes);

	std::cout << "create handler\n";
	// get handler
	std::string out_path = "/tmp/test.gpx";
	std::unique_ptr<HandlerReaderBase> out_handler = plugin->createReader<ProviderRouteReaderBase> (std::move (out_provider), out_path, out_path.substr (out_path.size () - 3, 3));

	if (!out_handler) throw std::runtime_error ("data handler is empty");

	std::cout << "write file\n";
	// read the file given in the path argument of the constructor
	out_handler->writeFile ();
//#endif
	return 0;
}
