#ifndef _LIBGPSFILE2_TYPES_CREATOR_
#define _LIBGPSFILE2_TYPES_CREATOR_

#include <memory>
#include <functional>

//#include "libgpsfile2/GpsfileManager.hpp"
//#include "libgpsfile2/PluginHandler.hpp"

namespace libgpsfile2 {
	class GpsfileManager;
	class PluginHandler;

	inline namespace types {
		typedef std::function<void(const std::shared_ptr<GpsfileManager>&, const std::shared_ptr<PluginHandler>&)> Creator;
	}
}

#endif /* _LIBGPSFILE2_TYPES_CREATOR_ */