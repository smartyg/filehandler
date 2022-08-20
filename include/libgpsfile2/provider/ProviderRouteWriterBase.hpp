#ifndef _LIBGPSFILE2_PROVIDER_PROVIDERROUTEWRITERBASE_
#define _LIBGPSFILE2_PROVIDER_PROVIDERROUTEWRITERBASE_

#include <string>
#include <gpsdata/types/ObjectTime.hpp>

#include <libgpsfile2/provider/ProviderWriterBase.hpp>

namespace libgpsfile2::provider {

	class ProviderRouteWriterBase : public ProviderWriterBase {
	public:
		virtual ~ProviderRouteWriterBase (void) = default;

		virtual bool setNumTracks (const unsigned short&) = 0;
		virtual bool setTrackTitle (const unsigned short&, const std::string&) = 0;
		virtual bool setTrackSummary (const unsigned short&, const std::string&) = 0;
		virtual bool setTrackDetails (const unsigned short&, const std::string&) = 0;
		virtual bool setTrackActivityType (const unsigned short&, const std::string&) = 0;
		virtual bool setTrackOriginalHash (const unsigned short&, const std::string&) = 0;
		virtual bool addTrackPoint (const unsigned short&, const unsigned short&, const gpsdata::ObjectTime&) = 0;
		virtual bool addTrackPointData (const unsigned short&, const unsigned short&, const gpsdata::ObjectTime&, const std::string&, const std::string&) = 0;
		virtual bool addTrackPointData (const unsigned short&, const unsigned short&, const gpsdata::ObjectTime&, const std::string&, int) = 0;
		virtual bool addTrackPointData (const unsigned short&, const unsigned short&, const gpsdata::ObjectTime&, const std::string&, long long) = 0;
		virtual bool addTrackPointData (const unsigned short&, const unsigned short&, const gpsdata::ObjectTime&, const std::string&, float) = 0;
		virtual bool addTrackPointData (const unsigned short&, const unsigned short&, const gpsdata::ObjectTime&, const std::string&, double) = 0;
	};
}

#endif /* _LIBGPSFILE2_PROVIDER_PROVIDERROUTEWRITERBASE_ */
