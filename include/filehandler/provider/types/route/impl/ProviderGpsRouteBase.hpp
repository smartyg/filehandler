#ifndef _FILEHANDLER_PROVIDER_PROVIDERGPSROUTEBASE_
#define _FILEHANDLER_PROVIDER_PROVIDERGPSROUTEBASE_

#include <array>
#include <string_view>

namespace filehandler::provider::internal {

	class ProviderGpsRouteBase {
	protected:
		constexpr static const auto _type_pairs = std::to_array<const std::pair<const std::string_view, const std::string_view>> ({
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
	};
}

#endif /* _FILEHANDLER_PROVIDER_PROVIDERGPSROUTEBASE_ */
