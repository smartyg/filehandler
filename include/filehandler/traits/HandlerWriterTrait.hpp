#ifndef _FILEHANDLER_HANDLER_TRAIT_HANDLERWRITERTRAIT_
#define _FILEHANDLER_HANDLER_TRAIT_HANDLERWRITERTRAIT_

#include <type_traits>

#include <filehandler/handler/HandlerWriterBase.hpp>
#include <filehandler/traits/common.hpp>

namespace filehandler::handler {
	inline namespace traits {
		template<class T>
		concept CONCEPT_RETURN HandlerWriterTrait = requires(const T t) {
			std::is_base_of<HandlerWriterBase, T>::value;
		};

		template<class T>
		concept CONCEPT_RETURN HandlerWriterAbstractTrait = requires(const T t) {
			std::is_abstract<T>::value;
			std::is_base_of<HandlerWriterBase, T>::value;
		};

		template<class T>
		concept CONCEPT_RETURN HandlerWriterFinalTrait = requires(const T t) {
			std::is_final<T>::value;
			std::is_base_of<HandlerWriterBase, T>::value;
		};
	}
}

#endif /* _FILEHANDLER_HANDLER_TRAIT_HANDLERWRITERTRAIT_ */
