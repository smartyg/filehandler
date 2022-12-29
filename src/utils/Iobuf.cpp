#include "config.h"

#include "filehandler/utils/Iobuf.hpp"
#include "filehandler/utils/IobufBase.hpp"

template class filehandler::utils::IobufBase<char>;
template class filehandler::utils::IobufBase<uint8_t>;
