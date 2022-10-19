#ifndef _LIBGPSFILE2_TYPES_PLUGINDETAILS_
#define _LIBGPSFILE2_TYPES_PLUGINDETAILS_

#include "libgpsfile2/types/Creator.hpp"

namespace libgpsfile2 {
	inline namespace types {

		struct PluginDetails {
			const std::vector<Creator> _handlers;
		};
	}
}

#endif /* _LIBGPSFILE2_TYPES_PLUGINDETAILS_ */
