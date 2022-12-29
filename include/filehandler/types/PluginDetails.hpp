#ifndef _FILEHANDLER_TYPES_PLUGINDETAILS_
#define _FILEHANDLER_TYPES_PLUGINDETAILS_

#include "filehandler/types/PluginCreatorFunc.hpp"

namespace filehandler {
	inline namespace types {

		struct PluginDetails {
			const std::vector<PluginCreatorFunc> handlers;
		};
	}
}

#endif /* _FILEHANDLER_TYPES_PLUGINDETAILS_ */
