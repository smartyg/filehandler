#ifndef _LIBGPSFILE2_PROVIDER_PROVIDERROUTEBASE_
#define _LIBGPSFILE2_PROVIDER_PROVIDERROUTEBASE_

#include <array>
#include <string_view>
#include <type_traits>

namespace libgpsfile2::provider::internal {

	namespace base {
		class ProviderRouteBase {
		public:
			typedef int8_t RouteData;

		protected:
			constexpr static const auto _type_map = std::to_array<const std::string_view>({
				"id", //Id number of this element.
				//https://www8.garmin.com/xmlschemas/TrackPointExtensionv1.xsd
				//https://www8.garmin.com/xmlschemas/GpxExtensionsv3.xsd
				// types from: http://www.cluetrust.com/XML/GPXDATA/1/0
				"heartrate", //HR represents the heart rate in beats per minute.
				"cadence", //Cadence represents the cadence in revolutions per minute.
				"temperature", //Temp represents the temperature in degrees celcius.
				"waterTemperature", //water_temperature represents the water temperature in degrees celcius.
				"power",
				"speed",
				"pressure",
				"abspressure",
				"distance", //Distance in meters as measured by GPS or wheel sensor.

				// Types from: http://www.topografix.com/GPX/1/1/gpx.xsd
				"lat", //The latitude of the point. Decimal degrees, WGS84 datum.
				"lon", //The latitude of the point. Decimal degrees, WGS84 datum.
				"ele", //Elevation (in meters) of the point.
				"time", //Creation/modification timestamp for element. Date and time in are in Univeral Coordinated Time (UTC), not local time! Conforms to ISO 8601 specification for date/time representation. Fractional seconds are allowed for millisecond timing in tracklogs.
				"magvar", //Magnetic variation (in degrees) at the point.
				"geoidheight", //Height (in meters) of geoid (mean sea level) above WGS84 earth ellipsoid. As defined in NMEA GGA message.
				"fix", //Type of GPX fix.
				"sat", //Number of satellites used to calculate the GPX fix.
				"hdop", //Horizontal dilution of precision.
				"vdop", //Vertical dilution of precision.
				"pdop", //Position dilution of precision.
				"ageofdgpsdata", //Number of seconds since last DGPS update.
				"dgpsid", //ID of DGPS station used in differential correction.
				"name", //The name of the point or segment or route.
				"comment", //GPS waypoint comment. Sent to GPS as comment.
				"description", //A text description of the element. Holds additional information about the element intended for the user, not the GPS.
				"type", //Type (classification) of the waypoint.
				"link", //URLs associated with the location described in the point, segment, route or file.

				// Only valid for summaries
				"sensor", //Set if there is a sensor present.
				"sport", //The type of sport being participated in.
				"programType", //The type of program being executed.
				"startPoint", //The starting point of the lap in Lat/Long.
				"endPoint", //The ending point of the lap in Lat/Long.
				"startTime", //The starting time of the lap.
				"elapsedTime", //The total elapsed time of the lap in seconds.
				"calories", //The number of calories burned.
				"intensity", //The intensity of the lap (whether resting or active).

				// only valid at file level
				"author", //The person or organization who created the GPX file.
				"createTime", //The creation date of the file.
				"keywords", //Keywords associated with the file. Search engines or databases can use this information to classify the data.
				"src", //Source of data. Included to give user some idea of reliability and accuracy of data. "Garmin eTrex", "USGS quad Boston North", e.g.
				"year", //Year of copyright.
				"license", //Link to external file containing license text.

				"deviceSerial",
				"deviceName",
				"deviceFWVersion",
				"deviceHWVersion",

				"ascent",
				"descent",
				"ascentTime",
				"descentTime",
				"recoveryTime",
				"SpeedAverage",
				"SpeedMax",
				"cadenceAverage",
				"cadenceMax",
				"altitudeMin",
				"altitudeMax",
				"heartrateAverage",
				"heartrateMin",
				"heartrateMax",
				"peakTrainingEffect",
				"temperatureMin",
				"temperatureMax",
				"verticalSpeed",
				"energy",
				"ehpe",
				"evpe",
			});

			constexpr static RouteData getType (const std::string_view name) {
				for (auto it = ProviderRouteBase::_type_map.cbegin (); it != ProviderRouteBase::_type_map.cend (); ++it) {
					if ((*it).compare (name) == 0) return std::distance (ProviderRouteBase::_type_map.cbegin (), it);
				}
				return -1;
			}

			constexpr static const std::string_view getTypeName (const RouteData& type) {
				if (type < 0) return std::string_view ();
				else return ProviderRouteBase::_type_map[static_cast<decltype(ProviderRouteBase::_type_map)::size_type>(type)];
			}
		};
	}

	class ProviderRouteBase : public base::ProviderRouteBase {
	public:
		static RouteData getType (const std::string_view name) {
			for (auto it = ProviderRouteBase::_type_map.cbegin (); it != ProviderRouteBase::_type_map.cend (); ++it) {
				if ((*it).compare (name) == 0) return std::distance (ProviderRouteBase::_type_map.cbegin (), it);
			}
			return -1;
		}

		constexpr static const std::string_view getTypeName (const RouteData& type) {
			if (type < 0) return {};
			else return ProviderRouteBase::_type_map[static_cast<decltype(ProviderRouteBase::_type_map)::size_type>(type)];
		}

		constexpr static const RouteData TYPE_NO_TYPE = -1;

		constexpr static const RouteData TYPE_ID = base::ProviderRouteBase::getType ("id");

		constexpr static const RouteData TYPE_HEARTRATE = base::ProviderRouteBase::getType ("heartrate");
		constexpr static const RouteData TYPE_CADENCE = base::ProviderRouteBase::getType ("cadence");
		constexpr static const RouteData TYPE_TEMPERATURE = base::ProviderRouteBase::getType ("temperature");
		constexpr static const RouteData TYPE_WATERTEMPERATURE = base::ProviderRouteBase::getType ("waterTemperature");
		constexpr static const RouteData TYPE_POWER = base::ProviderRouteBase::getType ("power");
		constexpr static const RouteData TYPE_SPEED = base::ProviderRouteBase::getType ("speed");
		constexpr static const RouteData TYPE_PRESSURE = base::ProviderRouteBase::getType ("pressure");
		constexpr static const RouteData TYPE_ABSPRESSURE = base::ProviderRouteBase::getType ("asbpressure");
		constexpr static const RouteData TYPE_DISTANCE = base::ProviderRouteBase::getType ("distance");

		// Types from: http://www.topografix.com/GPX/1/1/gpx.xsd
		constexpr static const RouteData TYPE_LAT = base::ProviderRouteBase::getType ("lat");
		constexpr static const RouteData TYPE_LON = base::ProviderRouteBase::getType ("lon");
		constexpr static const RouteData TYPE_ELE = base::ProviderRouteBase::getType ("ele");
		constexpr static const RouteData TYPE_TIME = base::ProviderRouteBase::getType ("time");
		constexpr static const RouteData TYPE_MAGVAR = base::ProviderRouteBase::getType ("magvar");
		constexpr static const RouteData TYPE_GEOIDHEIGHT = base::ProviderRouteBase::getType ("geoidheight");
		constexpr static const RouteData TYPE_FIX = base::ProviderRouteBase::getType ("fix");
		constexpr static const RouteData TYPE_SAT = base::ProviderRouteBase::getType ("sat");
		constexpr static const RouteData TYPE_HDOP = base::ProviderRouteBase::getType ("hdop");
		constexpr static const RouteData TYPE_VDOP = base::ProviderRouteBase::getType ("vdop");
		constexpr static const RouteData TYPE_PDOP = base::ProviderRouteBase::getType ("pdop");
		constexpr static const RouteData TYPE_AGEOFDGPSDATA = base::ProviderRouteBase::getType ("ageofdgpsdata");
		constexpr static const RouteData TYPE_DGPSID = base::ProviderRouteBase::getType ("dgpsid");
		constexpr static const RouteData TYPE_NAME = base::ProviderRouteBase::getType ("name");
		constexpr static const RouteData TYPE_COMMENT = base::ProviderRouteBase::getType ("comment");
		constexpr static const RouteData TYPE_DESCRIPTION = base::ProviderRouteBase::getType ("description");
		constexpr static const RouteData TYPE_TYPE = base::ProviderRouteBase::getType ("type");
		constexpr static const RouteData TYPE_LINK = base::ProviderRouteBase::getType ("link");

		// Only valid for summaries
		constexpr static const RouteData TYPE_SENSOR = base::ProviderRouteBase::getType ("sensor");
		constexpr static const RouteData TYPE_SPORT = base::ProviderRouteBase::getType ("sport");
		constexpr static const RouteData TYPE_PROGRAMTYPE = base::ProviderRouteBase::getType ("programType");
		constexpr static const RouteData TYPE_STARTPOINT = base::ProviderRouteBase::getType ("startPoint");
		constexpr static const RouteData TYPE_ENDPOINT = base::ProviderRouteBase::getType ("endPoint");
		constexpr static const RouteData TYPE_STARTTIME = base::ProviderRouteBase::getType ("startTime");
		constexpr static const RouteData TYPE_ELAPSEDTIME = base::ProviderRouteBase::getType ("elapsedTime");
		constexpr static const RouteData TYPE_CALORIES = base::ProviderRouteBase::getType ("calories");
		constexpr static const RouteData TYPE_INTENSITY = base::ProviderRouteBase::getType ("intensity");

		// only valid at file level
		constexpr static const RouteData TYPE_AUTHOR = base::ProviderRouteBase::getType ("author");
		constexpr static const RouteData TYPE_CREATETIME = base::ProviderRouteBase::getType ("createTime");
		constexpr static const RouteData TYPE_KEYWORDS = base::ProviderRouteBase::getType ("keywords");
		constexpr static const RouteData TYPE_SRC = base::ProviderRouteBase::getType ("src");
		constexpr static const RouteData TYPE_YEAR = base::ProviderRouteBase::getType ("year");
		constexpr static const RouteData TYPE_LICENSE = base::ProviderRouteBase::getType ("license");

		constexpr static const RouteData TYPE_DEVICESERIAL = base::ProviderRouteBase::getType ("deviceSerial");
		constexpr static const RouteData TYPE_DEVICENAME = base::ProviderRouteBase::getType ("deviceName");
		constexpr static const RouteData TYPE_DEVICEFWVERSION = base::ProviderRouteBase::getType ("deviceFWVersion");
		constexpr static const RouteData TYPE_DEVICEHWVERSION = base::ProviderRouteBase::getType ("deviceHWVersion");

		constexpr static const RouteData TYPE_ASCENT = base::ProviderRouteBase::getType ("ascent");
		constexpr static const RouteData TYPE_DESCENT = base::ProviderRouteBase::getType ("descent");
		constexpr static const RouteData TYPE_ASCENTTIME = base::ProviderRouteBase::getType ("ascentTime");
		constexpr static const RouteData TYPE_DESCENTTIME = base::ProviderRouteBase::getType ("descentTime");
		constexpr static const RouteData TYPE_RECOVERYTIME = base::ProviderRouteBase::getType ("recoveryTime");
		constexpr static const RouteData TYPE_SPEEDAVERAGE = base::ProviderRouteBase::getType ("SpeedAverage");
		constexpr static const RouteData TYPE_SPEEDMAX = base::ProviderRouteBase::getType ("SpeedMax");
		constexpr static const RouteData TYPE_CADENCEAVERAGE = base::ProviderRouteBase::getType ("cadenceAverage");
		constexpr static const RouteData TYPE_CADENCEMAX = base::ProviderRouteBase::getType ("cadenceMax");
		constexpr static const RouteData TYPE_ALTITUDEMIN = base::ProviderRouteBase::getType ("altitudeMin");
		constexpr static const RouteData TYPE_ALTITUDEMAX = base::ProviderRouteBase::getType ("altitudeMax");
		constexpr static const RouteData TYPE_HEARTRATEAVERAGE = base::ProviderRouteBase::getType ("heartrateAverage");
		constexpr static const RouteData TYPE_HEARTRATEMIN = base::ProviderRouteBase::getType ("heartrateMin");
		constexpr static const RouteData TYPE_HEARTRATEMAX = base::ProviderRouteBase::getType ("heartrateMax");
		constexpr static const RouteData TYPE_PEAKTRAININGEFFECT = base::ProviderRouteBase::getType ("peakTrainingEffect");
		constexpr static const RouteData TYPE_TEMPERATUREMIN = base::ProviderRouteBase::getType ("temperatureMin");
		constexpr static const RouteData TYPE_TEMPERATUREMAX = base::ProviderRouteBase::getType ("temperatureMax");
		constexpr static const RouteData TYPE_ENERGY = base::ProviderRouteBase::getType ("energy");

		constexpr static const RouteData TYPE_HEADING = base::ProviderRouteBase::getType ("heading");
		constexpr static const RouteData TYPE_VERTICALSPEED = base::ProviderRouteBase::getType ("verticalSpeed");
		constexpr static const RouteData TYPE_EHPE = base::ProviderRouteBase::getType ("ehpe");
		constexpr static const RouteData TYPE_EVPE = base::ProviderRouteBase::getType ("evpe");

	};
}

#endif /* _LIBGPSFILE2_PROVIDER_PROVIDERROUTEBASE_ */
