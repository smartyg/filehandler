#ifndef _LIBGPSFILE2_TRAIT_DATAHANDLER_
#define _LIBGPSFILE2_TRAIT_DATAHANDLER_

#include <type_traits>

#include <libgpsfile2/Datahandler.hpp>
#include <libgpsfile2/traits/common.hpp>

namespace libgpsfile2 {
	inline namespace traits {
		template<class T>
		concept CONCEPT_RETURN HandlerReaderTrait = requires(const T t) {
			std::is_base_of<HandlerReaderBase, T>::value;
		};

		template<class T>
		concept CONCEPT_RETURN HandlerWriterTrait = requires(const T t) {
			std::is_base_of<HandlerWriterBase, T>::value;
		};
	}
}

#endif /* _LIBGPSFILE2_TRAIT_DATAHANDLER_ */
