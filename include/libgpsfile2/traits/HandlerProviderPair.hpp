#ifndef _LIBGPSFILE2_TRAIT_HANDLERPROVIDERPAIR_
#define _LIBGPSFILE2_TRAIT_HANDLERPROVIDERPAIR_

#include <type_traits>

#include <libgpsfile2/traits/Datahandler.hpp>
#include <libgpsfile2/traits/provider.hpp>
#include <libgpsfile2/traits/common.hpp>

namespace libgpsfile2 {
	template<HandlerReaderTrait H, provider::ProviderReaderTrait P>
	class HandlerProviderReaderPair {
	public:
		using Handler = H;
		using Provider = P;
		HandlerProviderReaderPair (void) = delete;
		~HandlerProviderReaderPair (void) = default;
	};

	template<HandlerWriterTrait H, provider::ProviderWriterTrait P>
	class HandlerProviderWriterPair {
	public:
		using Handler = H;
		using Provider = P;
		HandlerProviderWriterPair (void) = delete;
		~HandlerProviderWriterPair (void) = default;
	};

	inline namespace traits {
		template<class T>
		concept CONCEPT_RETURN HandlerProviderReaderPairTrait = requires(const T t) {
			typename T::Handler;
			typename T::Provider;
		};

		template<class T>
		concept CONCEPT_RETURN HandlerProviderWriterPairTrait = requires(const T t) {
			typename T::Handler;
			typename T::Provider;
		};
	}
}

#endif /* _LIBGPSFILE2_TRAIT_HANDLERPROVIDERPAIR_ */
