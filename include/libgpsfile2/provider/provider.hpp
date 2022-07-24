#ifndef _X_LIBGPSFILE2_PROVIDER_
#define _X_LIBGPSFILE2_PROVIDER_

#include <list>
#include <string>
#include <memory>
#include <gpsdata/types/ObjectTime.hpp>

#include "../BasePlugin.hpp"

namespace libgpsfile2::provider {
	// Base data handler class
	class ProviderBase : public libgpsfile2::BasePlugin {
	public:
		virtual ~ProviderBase (void) = default;
	};

	// Base data handler class
	class ProviderReaderBase : virtual public ProviderBase {
	public:
		virtual ~ProviderReaderBase (void) = default;
		virtual const std::string getData (void) const = 0;
	};

	class ProviderWriterBase : virtual public ProviderBase {
	public:
		virtual ~ProviderWriterBase (void) = default;

		virtual bool setData (const std::string&) const = 0;
		virtual void finished (void) = 0;
	};

	// Data providers used for data handlers from plugins.
	class ProviderRouteReaderBase : public ProviderReaderBase {
	public:
		virtual ~ProviderRouteReaderBase (void) = default;

		//virtual GpsRoute_ptr_const getRoute (void) const = 0;
	};

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

	class ProviderWaypointReaderBase : public ProviderReaderBase {
	public:
		virtual ~ProviderWaypointReaderBase (void) = default;

		//virtual const std::list<GpsPoint_ptr> getWaypoints (void) const = 0;
	};

	class ProviderTableReaderBase : public ProviderReaderBase {
	public:
		virtual ~ProviderTableReaderBase (void) = default;

		//virtual GpsStatistics_ptr_const getTable (void) const = 0;
	};

	//DataProviderSimple *create_DataProviderSimple (Libsad *, const FilterInfoType, auto);
	//BaseDataProviderWriter_ptr create_DataProviderRouteWriter (Libsad_ptr, const FilterInfoType&);
	//BaseDataProviderReader_ptr create_DataProviderRouteReader (Libsad_ptr, const FilterInfoType&);
	//BaseDataProviderReader_ptr create_DataProviderTableReader (Libsad_ptr, const FilterInfoType&);
	//BaseDataProviderReader_ptr create_DataProviderWaypointReader (Libsad_ptr, const FilterInfoType&);
}

#endif /* _X_LIBGPSFILE2_PROVIDER_ */
