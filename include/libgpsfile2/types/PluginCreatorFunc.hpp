#ifndef _LIBGPSFILE2_TYPES_PLUGINCREATORFUNC_
#define _LIBGPSFILE2_TYPES_PLUGINCREATORFUNC_

#include <memory>
#include <functional>

namespace libgpsfile2 {
	class GpsfileManager;

	inline namespace types {
		typedef std::function<void(const std::shared_ptr<GpsfileManager>&)> PluginCreatorFunc;
	}
}

#endif /* _LIBGPSFILE2_TYPES_PLUGINCREATORFUNC_ */