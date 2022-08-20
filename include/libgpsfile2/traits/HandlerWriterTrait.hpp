#ifndef _LIBGPSFILE2_HANDLER_TRAIT_HANDLERWRITERTRAIT_
#define _LIBGPSFILE2_HANDLER_TRAIT_HANDLERWRITERTRAIT_

#include <type_traits>

#include <libgpsfile2/handler/HandlerWriterBase.hpp>
#include <libgpsfile2/traits/common.hpp>

namespace libgpsfile2::handler {
	inline namespace traits {
		template<class T>
		concept CONCEPT_RETURN HandlerWriterTrait = requires(const T t) {
			std::is_base_of<HandlerWriterBase, T>::value;
		};
	}
}

#endif /* _LIBGPSFILE2_HANDLER_TRAIT_HANDLERWRITERTRAIT_ */
