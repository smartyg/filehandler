#ifndef _LIBGPSFILE2_HANDLER_TRAIT_HANDLERREADERTRAIT_
#define _LIBGPSFILE2_HANDLER_TRAIT_HANDLERREADERTRAIT_

#include <type_traits>

#include <libgpsfile2/handler/HandlerReaderBase.hpp>
#include <libgpsfile2/traits/common.hpp>

namespace libgpsfile2::handler {
	inline namespace traits {
		template<class T>
		concept CONCEPT_RETURN HandlerReaderTrait = requires(const T t) {
			std::is_base_of<HandlerReaderBase, T>::value;
		};
	}

	template<class T>
	concept CONCEPT_RETURN HandlerReaderAbstractTrait = requires(const T t) {
		std::is_abstract<T>::value;
		std::is_base_of<HandlerReaderBase, T>::value;
	};

	template<class T>
	concept CONCEPT_RETURN HandlerReaderFinalTrait = requires(const T t) {
		std::is_final<T>::value;
		std::is_base_of<HandlerReaderBase, T>::value;
	};
}

#endif /* _LIBGPSFILE2_HANDLER_TRAIT_HANDLERREADERTRAIT_ */
