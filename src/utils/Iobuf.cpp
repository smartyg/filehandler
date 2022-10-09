#include "config.h"
#include <features.h>

#include "libgpsfile2/utils/Iobuf.hpp"
#include "libgpsfile2/utils/IobufBase.hpp"

#include <gpsdata/utils/Logger.hpp>

template class libgpsfile2::utils::IobufBase<char>;
template class libgpsfile2::utils::IobufBase<uint8_t>;
//template class libgpsfile2::utils::IobufBase<int>;
//template class libgpsfile2::utils::IobufBase<unsigned int>;
