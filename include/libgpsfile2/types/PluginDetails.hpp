#ifndef _LIBGPSFILE2_TYPES_PLUGINDETAILS_
#define _LIBGPSFILE2_TYPES_PLUGINDETAILS_

#include "libgpsfile2/types/PluginCreatorFunc.hpp"

namespace libgpsfile2 {
	inline namespace types {

		struct PluginDetails {
			const std::vector<PluginCreatorFunc> handlers;
		};
	}
}

#endif /* _LIBGPSFILE2_TYPES_PLUGINDETAILS_ */
