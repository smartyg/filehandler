#ifndef _FILEHANDLER_UTILS_STRINGS_
#define _FILEHANDLER_UTILS_STRINGS_

#include <filehandler/utils/StringsBase.hpp>

namespace filehandler::utils {
	extern template class StringsBase<char>;
	extern template class StringsBase<unsigned char>;
	extern template class StringsBase<signed char>;
	extern template class StringsBase<uint8_t>;
	typedef StringsBase<char> Strings;
	typedef StringsBase<unsigned char> StringsUnsigned;
	typedef StringsBase<signed char> StringsSigned;
	typedef StringsBase<uint8_t> StringsInt;
}

#endif /* _FILEHANDLER_UTILS_STRINGS_ */
