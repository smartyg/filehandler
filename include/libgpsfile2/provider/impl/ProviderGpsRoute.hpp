#ifndef _LIBGPSFILE2_PROVIDER_PROVIDERGPSROUTE_
#define _LIBGPSFILE2_PROVIDER_PROVIDERGPSROUTE_

#include <gpsdata.hpp>
#include <gpsdata/utils/GpsDataFactoryBasic.hpp>
#include <libgpsfile2/provider/impl/ProviderGpsRouteReader.hpp>
#include <libgpsfile2/provider/impl/ProviderGpsRouteWriter.hpp>

namespace gpsdata {
	extern template class GpsRoute<utils::GpsDataFactoryBasic, GpsSegment<utils::GpsDataFactoryBasic, GpsPoint<utils::GpsDataFactoryBasic>>>;
}

namespace libgpsfile2::provider {
	extern template class ProviderGpsRouteReader<gpsdata::GpsRoute<gpsdata::utils::GpsDataFactoryBasic, gpsdata::GpsSegment<gpsdata::utils::GpsDataFactoryBasic, gpsdata::GpsPoint<gpsdata::utils::GpsDataFactoryBasic>>>>;
	extern template class ProviderGpsRouteWriter<gpsdata::GpsRoute<gpsdata::utils::GpsDataFactoryBasic, gpsdata::GpsSegment<gpsdata::utils::GpsDataFactoryBasic, gpsdata::GpsPoint<gpsdata::utils::GpsDataFactoryBasic>>>>;
}

#endif /* _LIBGPSFILE2_PROVIDER_PROVIDERGPSROUTE_ */