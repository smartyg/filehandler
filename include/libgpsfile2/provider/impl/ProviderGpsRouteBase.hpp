#ifndef _LIBGPSFILE2_PROVIDER_PROVIDERGPSROUTEBASE_
#define _LIBGPSFILE2_PROVIDER_PROVIDERGPSROUTEBASE_

#include <array>
#include <string_view>

namespace libgpsfile2::provider::internal {

	class ProviderGpsRouteBase {
	protected:
		constexpr static const auto _type_pais = std::to_array<const std::pair<const std::string_view, const std::string_view>> ({
			{ "heartrate", "HEARTRATE" },
			{ "cadence", "CADENCE" },
			{ "temperature", "TEMPERATURE" },
			{ "waterTemperature", "WATERTEMPERATURE" },
			{ "power", "POWER" },
			{ "speed", "SPEED" },
			{ "pressure", "PRESSURE" },
			{ "distance", "DISTANCE" },
			{ "lat", "LAT" },
			{ "lon", "LON" },
			{ "ele", "ALT" },
			{ "time", "TIME" },
			{ "magvar", "MAGVAR" },
			{ "geoidheight", "GEOIDHEIGHT" },
			{ "fix", "FIX" },
			{ "sat", "SAT" },
			{ "hdop", "HDOP" },
			{ "vdop", "VDOP" },
			{ "pdop", "PDOP" },
			{ "ageofdgpsdata", "AGEOFDGPSDATA" },
			{ "dgpsid", "DGPSID" },
			{ "name", "NAME" },
			{ "comment", "COMMENT" },
			{ "description", "DESCRIPTION" },
			{ "type", "TYPE" },
			{ "link", "LINK" },
			{ "sensor", "SENSOR" },
			{ "sport", "SPORT" },
			{ "programType", "PROGRAMTYPE" },
			{ "startPoint", "STARTPOINT" },
			{ "endPoint", "ENDPOINT" },
			{ "startTime", "STARTTIME" },
			{ "elapsedTime", "ELAPSEDTIME" },
			{ "calories", "CALORIES" },
			{ "intensity", "INTENSITY" },
			{ "author", "AUTHOR" },
			{ "createTime", "CREATETIME" },
			{ "keywords", "KEYWORDS" },
			{ "src", "SRC" },
			{ "year", "YEAR" },
			{ "license", "LICENSE" },
		});
#if 0
		struct {
			std::pair<int, typename std::vector<std::shared_ptr<R>>::const_iterator> route;
			std::pair<int, typename std::vector<gpsdata::GpsValue<DataType>>::const_iterator> route_statistics;
			std::pair<int, typename R::const_iterator> segment;
			std::pair<int, typename std::vector<gpsdata::GpsValue<DataType>>::const_iterator> segment_statistics;
			std::pair<int, typename R::Segment::const_iterator> point;
			std::pair<int, typename R::Point::const_iterator> data;
		} _iterators;

		void initIterators (void) {
			auto& it = this->_iterators;
			it.route.first = 0;
			it.route.second = this->_routes.cbegin ();
			it.route_statistics.first = 0;
			it.route_statistics.second = (*(it.route.second))->getStatistics ().cbegin ();
			it.segment.first = 0;
			it.segment.second = (*(it.route.second))->cbegin ();
			it.segment_statistics.first = 0;
			it.segment_statistics.second = (*(it.segment.second))->getStatistics ().cbegin ();
			it.point.first = 0;
			it.point.second = (*(it.segment.second))->cbegin ();
			it.data.first = 0;
			it.data.second = (*(it.point.second))->cbegin ();
		}
#endif
	};
}

#endif /* _LIBGPSFILE2_PROVIDER_PROVIDERGPSROUTEBASE_ */
