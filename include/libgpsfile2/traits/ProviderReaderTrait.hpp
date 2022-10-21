#ifndef _LIBGPSFILE2_PROVIDER_TRAITS_PROVIDERREADERTRAIT_
#define _LIBGPSFILE2_PROVIDER_TRAITS_PROVIDERREADERTRAIT_

#include <type_traits>
#include <libgpsfile2/provider/ProviderReaderBase.hpp>
#include <libgpsfile2/traits/common.hpp>

namespace libgpsfile2::provider {
	inline namespace traits {
		template<class T>
		concept CONCEPT_RETURN ProviderReaderTrait = requires(const T t) {
			std::is_base_of<ProviderReaderBase, T>::value;
		};
	}

	template<class T>
	concept CONCEPT_RETURN ProviderReaderAbstractTrait = requires(const T t) {
		std::is_abstract<T>::value;
		std::is_base_of<ProviderReaderBase, T>::value;
	};

	template<class T>
	concept CONCEPT_RETURN ProviderReaderFinalTrait = requires(const T t) {
		std::is_final<T>::value;
		std::is_base_of<ProviderReaderBase, T>::value;
	};
}

#endif /* _LIBGPSFILE2_PROVIDER_TRAITS_PROVIDERREADERTRAIT_ */
