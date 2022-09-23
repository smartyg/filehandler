#ifndef _X_GPSDATA_EXAMPLE_PRINTGPSROUTE_
#define _X_GPSDATA_EXAMPLE_PRINTGPSROUTE_

#include <gpsdata/traits/GpsRoute.hpp>

template<gpsdata::GpsRouteTrait R>
void printGpsRoute (const std::shared_ptr<R>& route) {
	std::cout << "--- route: " << route->getTitle () << " (" << std::to_string (route->getId ()) << ") ---" << std::endl;
	std::cout << " summery: " << route->getSummary () << std::endl;
	std::cout << " details: " << route->getDetails () << std::endl;
	std::cout << " activity type: " << route->getFactory ()->getActivityTypeString (route->getActivityType ()) << std::endl;
	std::cout << " tz offset: " << std::to_string (route->getTimezoneOffset ()) << std::endl;

	std::cout << " --- statistics ---" << std::endl;
	for (const auto& data : route->getStatistics ()) {
		std::string str;
		if (route->getFactory ()->getValue (data, str, true))
			std::cout << "  " << route->getFactory ()->getDataTypeFullName (data.type) << ": " << str << std::endl;
	}

	std::cout << " --- segments ---" << std::endl;
	for (const auto& segment : *route) {
		std::cout << "  --- segment " << std::to_string (segment->getSegmentNumber ()) << " ---" << std::endl;
		std::cout << "   --- statistics ---" << std::endl;
		for (const auto& data : route->getStatistics ()) {
			std::string str;
			if (segment->getFactory ()->getValue (data, str, true))
				std::cout << "    " << segment->getFactory ()->getDataTypeFullName (data.type) << ": " << str << std::endl;
		}

		std::cout << "   --- points ---" << std::endl;
		for (const auto& point : *segment) {
			std::cout << "    point (" << std::to_string (point->getTime ().get ()) << ")" << std::endl;
			for (const auto& data : *point) {
				std::string str;
				if (point->getFactory ()->getValue (data, str, true))
					std::cout << "     " << point->getFactory ()->getDataTypeFullName (data.type) << ": " << str << std::endl;
			}
		}
	}
}

#endif /* _X_GPSDATA_EXAMPLE_PRINTGPSROUTE_ */
