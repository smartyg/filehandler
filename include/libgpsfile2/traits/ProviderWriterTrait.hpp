#ifndef _LIBGPSFILE2_PROVIDER_TRAITS_PROVIDERWRITERTRAIT_
#define _LIBGPSFILE2_PROVIDER_TRAITS_PROVIDERWRITERTRAIT_

#include <type_traits>
#include <libgpsfile2/provider/ProviderWriterBase.hpp>
#include <libgpsfile2/traits/common.hpp>

namespace libgpsfile2::provider {
	inline namespace traits {
		template<class T>
		concept CONCEPT_RETURN ProviderWriterTrait = requires(const T t) {
			std::is_base_of<ProviderWriterBase, T>::value;
		};

		template<class T>
		concept CONCEPT_RETURN ProviderWriterAbstractTrait = requires(const T t) {
			std::is_abstract<T>::value;
			std::is_base_of<ProviderWriterBase, T>::value;
		};

		template<class T>
		concept CONCEPT_RETURN ProviderWriterFinalTrait = requires(const T t) {
			std::is_final<T>::value;
			std::is_base_of<ProviderWriterBase, T>::value;
		};
	}
}

#endif /* _LIBGPSFILE2_PROVIDER_TRAITS_PROVIDERWRITERTRAIT_ */
