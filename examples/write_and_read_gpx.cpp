#include "config.h"

#include <memory>
#include <string>

#include <gpsdata/utils/Logger.hpp>
#include <gpsdata/utils/GpsDataFactoryBasic.hpp>
#include <libgpsfile2.hpp>
#include <libgpsfile2/provider/impl/ProviderGpsRoute.hpp>

#include "PrintGpsRoute.hpp"

using libgpsfile2::GpsfilePlugin;
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
	auto plugin = GpsfilePlugin::create ({std::string(PKGLIBDIR)});

	// create factory for gps data
	auto factory = GpsFactoryType::create ();

	gpsdata::ObjectId id = 1;
	auto route = GpsRouteType::create<GpsRouteType> (id, factory);
	route->addSegment (0);

	//gpsdata::ObjectTime t1 (static_cast<gpsdata::ObjectTime::internalTimeType>(10));
	gpsdata::ObjectTime t1 (2015, 3, 15, 10, 59, 58.012);
	std::chrono::milliseconds step {1024};
	route->addPointData (0, t1, "SPEED", 10, true);
	route->addPointData (0, t1, "ALT", 1000, true);
	route->addPointData (0, t1, "CADANCE", 98, true);
	//Amsterdam Central Station, the Netherlands
	route->addPointData (0, t1, "LAT", 490031600, true);
	route->addPointData (0, t1, "LON", 5237898800, true);

	gpsdata::ObjectTime t2 = t1 + step;
	route->addPointData (0, t2, "SPEED", 20, true);
	route->addPointData (0, t2, "CADANCE", 90, true);
	// Amsterdam Dam Square, the Netherlands
	route->addPointData (0, t2, "LAT", 489306200, true);
	route->addPointData (0, t2, "LON", 5237267200, true);

	gpsdata::ObjectTime t3 = t2 + step;
	route->addPointData (0, t3, "CADANCE", 101, true);
	// Amsterdam Dam Palace, the Netherlands
	// 52.373178, 4.891856
	route->addPointData (0, t3, "LAT", 489185600, true);
	route->addPointData (0, t3, "LON", 5237317800, true);

	gpsdata::ObjectTime t4 = t3 + step;
	// Anne Frank House, Amsterdam, the Netherlands
	// 52.375226, 4.883885
	route->addPointData (0, t4, "LAT", 488388500, true);
	route->addPointData (0, t4, "LON", 5237522600, true);

	gpsdata::ObjectTime t5 = t4 + step;
	// Vondel Park entrance, Amsterdam, the Netherlands
	// 52.361735, 4.881794
	route->addPointData (0, t5, "LAT", 488179400, true);
	route->addPointData (0, t5, "LON", 5236173500, true);

	gpsdata::ObjectTime t6 = t5 + step;
	// Van Gogh Museum, Amsterdam, the Netherlands
	// 52.358520, 4.880948
	route->addPointData (0, t6, "LAT", 488094800, true);
	route->addPointData (0, t6, "LON", 5235852000, true);

	gpsdata::ObjectTime t7 = t6 + step;
	// Rijksmuseum, Amsterdam, the Netherlands
	// 52.360272, 4.885635
	route->addPointData (0, t7, "LAT", 488563500, true);
	route->addPointData (0, t7, "LON", 5236027200, true);

	gpsdata::ObjectTime t8 = t7+ step;
	// Rembrandt House Museum, Amsterdam, the Netherlands
	// 52.369423, 4.901314
	route->addPointData (0, t8, "LAT", 490131400, true);
	route->addPointData (0, t8, "LON", 5236942300, true);

	route->setTitle ("test ride");
	route->setSummary ("long test ride");
	route->setDetails ("A very nice ling test ride.");
	route->setActivityType ("CYCLING");
	route->setTimezoneOffset (3600);

	std::vector<std::shared_ptr<GpsRouteType>> routes;
	routes.insert (routes.begin (), route);

	std::cout << "create provider\n";
	// create provider
	auto provider_reader = ProviderGpsRouteReader<GpsRouteType>::create (routes);

	std::cout << "register type\n";
	libgpsfile2::HandlerType route_reader;
	GpsfilePlugin::registerReaderType<ProviderRouteReaderBase> (route_reader);
	//std::cout << "register type\n";

	std::cout << "create handler\n";
	// get handler
	std::string path = "/tmp/test.gpx";
	std::unique_ptr<HandlerReaderBase> handler_reader = plugin->createReader (std::move (provider_reader), route_reader, path, path.substr (path.size () - 4, 4));

	if (!handler_reader) throw std::runtime_error ("data handler is empty");

	std::cout << "write file\n";
	// read the file given in the path argument of the constructor
	handler_reader->writeFile ();

	// Now we will read the file back and write the result into the system

	std::cout << "create writer provider\n";
	// create provider
	auto provider_writer = ProviderGpsRouteWriter<GpsRouteType>::create (factory);

	std::cout << "register writer type\n";
	libgpsfile2::HandlerType route_writer;
	GpsfilePlugin::registerWriterType<ProviderRouteWriterBase> (route_writer);
	//std::cout << "register type\n";

	std::cout << "create writer handler\n";
	// get handler
	std::unique_ptr<HandlerWriterBase> handler_writer = plugin->createWriter (std::move (provider_writer), route_writer, path, path.substr (path.size () - 4, 4));

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
