#ifndef _LIBGPSFILE2_EXAMPLES_WRITE_PROVIDER_
#define _LIBGPSFILE2_EXAMPLES_WRITE_PROVIDER_

#include <gpsdata.hpp>
#include <gpsdata/utils/GpsDataFactoryBasic.hpp>
#include <libgpsfile2/provider/ProviderGpsRouteReader.hpp>

//extern template class gpsdata::GpsPoint<gpsdata::utils::GpsDataFactoryBasic>;
//extern template class gpsdata::GpsSegment<gpsdata::utils::GpsDataFactoryBasic, gpsdata::GpsPoint<gpsdata::utils::GpsDataFactoryBasic>>;
extern template class gpsdata::GpsRoute<gpsdata::utils::GpsDataFactoryBasic, gpsdata::GpsSegment<gpsdata::utils::GpsDataFactoryBasic, gpsdata::GpsPoint<gpsdata::utils::GpsDataFactoryBasic>>>;
extern template class libgpsfile2::provider::ProviderGpsRouteReader<gpsdata::GpsRoute<gpsdata::utils::GpsDataFactoryBasic, gpsdata::GpsSegment<gpsdata::utils::GpsDataFactoryBasic, gpsdata::GpsPoint<gpsdata::utils::GpsDataFactoryBasic>>>>;

#endif /* _LIBGPSFILE2_EXAMPLES_WRITE_PROVIDER_ */