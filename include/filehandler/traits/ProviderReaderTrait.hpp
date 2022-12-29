#ifndef _FILEHANDLER_PROVIDER_TRAITS_PROVIDERREADERTRAIT_
#define _FILEHANDLER_PROVIDER_TRAITS_PROVIDERREADERTRAIT_

#include <type_traits>
#include <filehandler/provider/ProviderReaderBase.hpp>
#include <filehandler/traits/common.hpp>

namespace filehandler::provider {
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

#endif /* _FILEHANDLER_PROVIDER_TRAITS_PROVIDERREADERTRAIT_ */
