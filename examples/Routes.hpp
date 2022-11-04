#ifndef _LIBGPSFILE2_EXAMPLES_ROUTES_
#define _LIBGPSFILE2_EXAMPLES_ROUTES_

#include <memory>
#include <chrono>
#include <gpsdata.hpp>

template <gpsdata::GpsRouteTrait R>
std::shared_ptr<R> create_route_1 (const std::shared_ptr<typename R::GpsFactory> factory = {}) {
	std::shared_ptr<R> route;

	gpsdata::ObjectId id = 1;

	if (!factory) {
		// create factory for gps data
		const auto new_factory = R::GpsFactory::create ();

		route = R::template create<R> (id, new_factory);
	} else
		route = R::template create<R> (id, factory);

	route->addSegment (0);

	gpsdata::ObjectTime t1 (2015, 3, 15, 10, 59, 58.012);
	std::chrono::milliseconds step {1024};

	//Amsterdam Central Station, the Netherlands
	route->addPointData (0, t1, "LAT", 490031600, true);
	route->addPointData (0, t1, "LON", 5237898800, true);

	route->addPointData (0, t1, "SPEED", 10, true);
	route->addPointData (0, t1, "ALT", 1000, true);
	route->addPointData (0, t1, "CADENCE", 98, true);

	gpsdata::ObjectTime t2 = t1 + step;
	// Amsterdam Dam Square, the Netherlands
	route->addPointData (0, t2, "LAT", 489306200, true);
	route->addPointData (0, t2, "LON", 5237267200, true);

	route->addPointData (0, t2, "SPEED", 20, true);
	route->addPointData (0, t2, "CADENCE", 90, true);

	gpsdata::ObjectTime t3 = t2 + step;
	// Amsterdam Dam Palace, the Netherlands
	// 52.373178, 4.891856
	route->addPointData (0, t3, "LAT", 489185600, true);
	route->addPointData (0, t3, "LON", 5237317800, true);

	route->addPointData (0, t3, "CADENCE", 101, true);

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

	gpsdata::ObjectTime t8 = t7 + step;
	// Rembrandt House Museum, Amsterdam, the Netherlands
	// 52.369423, 4.901314
	route->addPointData (0, t8, "LAT", 490131400, true);
	route->addPointData (0, t8, "LON", 5236942300, true);

	route->setTitle ("test ride");
	route->setSummary ("long test ride");
	route->setDetails ("A very nice ling test ride.");
	route->setActivityType ("CYCLING");

	return route;
}

#endif /* _LIBGPSFILE2_EXAMPLES_ROUTES_ */