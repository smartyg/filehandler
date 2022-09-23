#include "write_gpx_provider.hpp"
using libgpsfile2::provider::ProviderGpsRouteReader;

using GpsFactoryType = gpsdata::utils::GpsDataFactoryBasic;
using GpsPointType = gpsdata::GpsPoint<GpsFactoryType>;
using GpsSegmentType = gpsdata::GpsSegment<GpsFactoryType, GpsPointType>;
using GpsRouteType = gpsdata::GpsRoute<GpsFactoryType, GpsSegmentType>;

//template class gpsdata::GpsPoint<GpsFactoryType>;
//template class gpsdata::GpsSegment<GpsFactoryType, GpsPointType>;
template class gpsdata::GpsRoute<GpsFactoryType, GpsSegmentType>;
template class ProviderGpsRouteReader<GpsRouteType>;
