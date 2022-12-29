#include "config.h"

#include <gpsdata.hpp>
#include <gpsdata/utils/GpsDataFactoryBasic.hpp>
#include <filehandler/provider/impl/ProviderGpsRoute.hpp>

using GpsFactoryType = gpsdata::utils::GpsDataFactoryBasic;
using GpsPointType = gpsdata::GpsPoint<GpsFactoryType>;
using GpsSegmentType = gpsdata::GpsSegment<GpsFactoryType, GpsPointType>;
using GpsRouteType = gpsdata::GpsRoute<GpsFactoryType, GpsSegmentType>;

template class gpsdata::GpsRoute<GpsFactoryType, GpsSegmentType>;
template class filehandler::provider::ProviderGpsRouteReader<GpsRouteType>;
template class filehandler::provider::ProviderGpsRouteWriter<GpsRouteType>;
