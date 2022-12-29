#ifndef _FILEHANDLER_UTILS_IOBUF_
#define _FILEHANDLER_UTILS_IOBUF_

#include <filehandler/utils/IobufBase.hpp>

namespace filehandler::utils {
	extern template class IobufBase<char>;
	extern template class IobufBase<uint8_t>;
	typedef IobufBase<char> Iobuf;
	typedef IobufBase<uint8_t> IobufInt;
}

#endif /* _FILEHANDLER_UTILS_IOBUF_ */
