#ifndef _X_GPSPLUGIN_HPP_
#define _X_GPSPLUGIN_HPP_

#include <array>
#include <map>
#include <tuple>
#include <string_view>
#include <stdexcept>
#include <Logger.hpp>
#include <libgpsfile2/provider/ProviderRouteBase.hpp>
#include <libgpsfile2/types/BasePlugin.hpp>

class GpxPlugin : public libgpsfile2::BasePlugin {

	constexpr static const auto _list_namespaces = std::to_array<const std::tuple<const std::string_view, const std::string_view, const std::string_view>> ({
		{ "gpxdata", "http://www.cluetrust.com/XML/GPXDATA/1/0", "http://www.cluetrust.com/Schemas/gpxdata10.xsd" },
		{ "gpxtpx", "http://www.garmin.com/xmlschemas/TrackPointExtension/v2", "http://www.garmin.com/xmlschemas/TrackPointExtensionv2.xsd" },
		//{ "gpxdata", "http://www.cluetrust.com/XML/GPXDATA/1/0", "" },
	});

	constexpr static const auto _gpx_tag_map = std::to_array<const std::tuple<const std::string_view, const std::string_view, const libgpsfile2::provider::internal::ProviderRouteBase::RouteData>> ({
		{ "http://www.cluetrust.com/XML/GPXDATA/1/0", "hr", libgpsfile2::provider::internal::ProviderRouteBase::TYPE_HEARTRATE },
		{ "http://www.cluetrust.com/XML/GPXDATA/1/0", "cadence", libgpsfile2::provider::internal::ProviderRouteBase::TYPE_CADENCE },
		{ "http://www.cluetrust.com/XML/GPXDATA/1/0", "temp", libgpsfile2::provider::internal::ProviderRouteBase::TYPE_TEMPERATURE },
		{ "http://www.cluetrust.com/XML/GPXDATA/1/0", "distance", libgpsfile2::provider::internal::ProviderRouteBase::TYPE_DISTANCE },
		{ "http://www.cluetrust.com/XML/GPXDATA/1/0", "sensor", libgpsfile2::provider::internal::ProviderRouteBase::TYPE_SENSOR },
		{ "http://www.cluetrust.com/XML/GPXDATA/1/0", "sport", libgpsfile2::provider::internal::ProviderRouteBase::TYPE_SPORT },
		{ "http://www.cluetrust.com/XML/GPXDATA/1/0", "programType", libgpsfile2::provider::internal::ProviderRouteBase::TYPE_PROGRAMTYPE },
		{ "http://www.cluetrust.com/XML/GPXDATA/1/0", "startPoint", libgpsfile2::provider::internal::ProviderRouteBase::TYPE_STARTPOINT },
		{ "http://www.cluetrust.com/XML/GPXDATA/1/0", "endPoint", libgpsfile2::provider::internal::ProviderRouteBase::TYPE_ENDPOINT },
		{ "http://www.cluetrust.com/XML/GPXDATA/1/0", "startTime", libgpsfile2::provider::internal::ProviderRouteBase::TYPE_STARTTIME },
		{ "http://www.cluetrust.com/XML/GPXDATA/1/0", "elapsedTime", libgpsfile2::provider::internal::ProviderRouteBase::TYPE_ELAPSEDTIME },
		{ "http://www.cluetrust.com/XML/GPXDATA/1/0", "calories", libgpsfile2::provider::internal::ProviderRouteBase::TYPE_CALORIES },
		{ "http://www.cluetrust.com/XML/GPXDATA/1/0", "intensity", libgpsfile2::provider::internal::ProviderRouteBase::TYPE_INTENSITY },
		{ "http://www.garmin.com/xmlschemas/TrackPointExtension/v2", "atemp", libgpsfile2::provider::internal::ProviderRouteBase::TYPE_TEMPERATURE },
		{ "http://www.garmin.com/xmlschemas/TrackPointExtension/v2", "wtemp", libgpsfile2::provider::internal::ProviderRouteBase::TYPE_WATERTEMPERATURE },
		{ "http://www.garmin.com/xmlschemas/TrackPointExtension/v2", "hr", libgpsfile2::provider::internal::ProviderRouteBase::TYPE_HEARTRATE },
		{ "http://www.garmin.com/xmlschemas/TrackPointExtension/v2", "cad", libgpsfile2::provider::internal::ProviderRouteBase::TYPE_CADENCE },
		{ "http://www.garmin.com/xmlschemas/TrackPointExtension/v2", "speed", libgpsfile2::provider::internal::ProviderRouteBase::TYPE_SPEED },
		{ "http://www.garmin.com/xmlschemas/TrackPointExtension/v1", "atemp", libgpsfile2::provider::internal::ProviderRouteBase::TYPE_TEMPERATURE },
		{ "http://www.garmin.com/xmlschemas/TrackPointExtension/v1", "wtemp", libgpsfile2::provider::internal::ProviderRouteBase::TYPE_WATERTEMPERATURE },
		{ "http://www.garmin.com/xmlschemas/TrackPointExtension/v1", "hr", libgpsfile2::provider::internal::ProviderRouteBase::TYPE_HEARTRATE },
		{ "http://www.garmin.com/xmlschemas/TrackPointExtension/v1", "cad", libgpsfile2::provider::internal::ProviderRouteBase::TYPE_CADENCE },
	});

	std::map<const std::string_view, std::map<const libgpsfile2::provider::internal::ProviderRouteBase::RouteData, const std::string_view>> _lookup_reader;
	std::map<const std::string_view, std::map<const std::string_view, const libgpsfile2::provider::internal::ProviderRouteBase::RouteData>> _lookup_writer;

public:
	GpxPlugin (void);
	~GpxPlugin (void);

	constexpr inline void init (void) {
		for (const std::tuple<const std::string_view, const std::string_view, const libgpsfile2::provider::internal::ProviderRouteBase::RouteData>& e : GpxPlugin::_gpx_tag_map) {
			this->_lookup_reader[std::get<0>(e)].insert (std::pair (std::get<2>(e), std::get<1>(e)));
			this->_lookup_writer[std::get<0>(e)].insert (std::pair (std::get<1>(e), std::get<2>(e)));
		}
	}

	inline libgpsfile2::provider::internal::ProviderRouteBase::RouteData namespaceTypeLookup (const std::string_view ns_uri, const std::string_view name) const {
		DEBUG_MSG ("GpxPlugin::{:s} ({:s}, {:s})\n", __func__, ns_uri, name);
		for (const auto& [key1, value1] : this->_lookup_writer) {
			if (ns_uri.compare (key1) == 0) {
				for (const auto& [key2, value2] : value1) {
					if (name.compare (key2) == 0) return value2;
				}
			}
		}
		return libgpsfile2::provider::internal::ProviderRouteBase::TYPE_NO_TYPE;
	}

	inline const std::string_view namespaceNameLookup (const std::string_view ns_uri, const libgpsfile2::provider::internal::ProviderRouteBase::RouteData& t) const {
		DEBUG_MSG("GpxPlugin::{:s} ({:s}, {:d})\n", __func__, ns_uri, t);
		for (const auto& [key1, value1] : this->_lookup_reader) {
			//DEBUG_MSG("  Compare against namespace %s\n", std::string (key1).c_str ());
			if (ns_uri.compare (key1) == 0) {
				try {
					//DEBUG_MSG("    check if %d is present\n", t);
					DEBUG_2_MSG(1, "    value of type {:d}: {:s}\n", t, value1.at (t));
					//const std::string_view ret = value1.at (t);
					//DEBUG_MSG("    value of type %d: %s\n", t, std::string (ret).c_str ());
					//return ret;
					return value1.at (t);
				} catch (const std::out_of_range& e) {
					(void)e;
					//DEBUG_MSG("    no match found for type %d\n", t);
					return std::string_view ();
				}
			}
		}
		return std::string_view ();
	}

	const auto& allNamespaces (void) {
		return this->_list_namespaces;
	}
};

#endif /* _X_GPSPLUGIN_HPP_ */
