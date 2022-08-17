#ifndef _LIBGPSFILE2_TRAITS_PROVIDER_
#define _LIBGPSFILE2_TRAITS_PROVIDER_

#include <type_traits>
#include <libgpsfile2/provider/provider.hpp>
#include <libgpsfile2/traits/common.hpp>

namespace libgpsfile2::provider {
	inline namespace traits {
		template<class T>
		concept CONCEPT_RETURN ProviderReaderTrait = requires(const T t) {
			std::is_base_of<ProviderReaderBase, T>::value;
		};

		template<class T>
		concept CONCEPT_RETURN ProviderWriterTrait = requires(const T t) {
			std::is_base_of<ProviderWriterBase, T>::value;
		};
	}
}

#endif /* _LIBGPSFILE2_TRAITS_PROVIDER_ */
