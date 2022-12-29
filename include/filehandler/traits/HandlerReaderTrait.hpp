#ifndef _FILEHANDLER_HANDLER_TRAIT_HANDLERREADERTRAIT_
#define _FILEHANDLER_HANDLER_TRAIT_HANDLERREADERTRAIT_

#include <type_traits>

#include <filehandler/handler/HandlerReaderBase.hpp>
#include <filehandler/traits/common.hpp>

namespace filehandler::handler {
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

#endif /* _FILEHANDLER_HANDLER_TRAIT_HANDLERREADERTRAIT_ */
