#include "config.h"

#include <features.h>
#include <gpsdata.hpp>
#include <gpsdata/utils/GpsDataFactoryBasic.hpp>
#include <libgpsfile2/provider/impl/ProviderGpsRoute.hpp>

using GpsFactoryType = gpsdata::utils::GpsDataFactoryBasic;
using GpsPointType = gpsdata::GpsPoint<GpsFactoryType>;
using GpsSegmentType = gpsdata::GpsSegment<GpsFactoryType, GpsPointType>;
using GpsRouteType = gpsdata::GpsRoute<GpsFactoryType, GpsSegmentType>;

template class gpsdata::GpsRoute<GpsFactoryType, GpsSegmentType>;
template class libgpsfile2::provider::ProviderGpsRouteReader<GpsRouteType>;
template class libgpsfile2::provider::ProviderGpsRouteWriter<GpsRouteType>;
