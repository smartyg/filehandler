#ifndef _LIBGPSFILE2_UTILS_STRINGS_
#define _LIBGPSFILE2_UTILS_STRINGS_

#include <libgpsfile2/utils/StringsBase.hpp>

namespace libgpsfile2::utils {
	extern template class StringsBase<char>;
	extern template class StringsBase<unsigned char>;
	extern template class StringsBase<signed char>;
	extern template class StringsBase<uint8_t>;
	typedef StringsBase<char> Strings;
	typedef StringsBase<unsigned char> StringsUnsigned;
	typedef StringsBase<signed char> StringsSigned;
	typedef StringsBase<uint8_t> StringsInt;
}

#endif /* _LIBGPSFILE2_UTILS_STRINGS_ */
